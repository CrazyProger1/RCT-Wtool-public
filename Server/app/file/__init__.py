import os

import flask

from ..user.tools import is_token_wrong as is_user_token_wrong
from ..bot.tools import is_token_wrong as is_bot_token_wrong
from ..models import Users, Bots, Files
from flask import request, abort, jsonify, send_from_directory
from ..config import *
from .. import db_session, db
import shutil


def save_file(file, filename):
    path_to_file = os.path.join("app", FILES_SAVING_PATH, filename)

    if os.path.exists(path_to_file):
        os.remove(path_to_file)

    file.save(path_to_file)


class File:
    @staticmethod
    def process_input_file() -> flask.Response:
        sender_type = request.args.get("sender_type", default="user", type=str)

        filename = request.args.get("filename", default="hello.txt", type=str)

        if sender_type == "user":
            access_token = request.headers.get("access_token")
            if is_user_token_wrong(access_token, ACCESS_TOKEN_SECRET_KEY):
                abort(401)

            sender: Users = Users.select_where(access_token=access_token).first()

        else:
            token = request.headers.get("token")
            if is_bot_token_wrong(token):
                abort(401)

            sender: Bots = Bots.select_where(token=token).first()

        if sender is None:
            abort(401)

        file = Files(filename)
        db_session.add(file)
        db_session.commit()

        save_file(
            file=request.files['file'],
            filename=str(file.id)
        )

        db_session.commit()
        return jsonify([file.id])

    @staticmethod
    def get_file(file_id: int) -> flask.Response:
        receiver_type = request.args.get("receiver_type", default="user", type=str)
        if receiver_type == "user":
            access_token = request.headers.get("access_token")

            if is_user_token_wrong(access_token, ACCESS_TOKEN_SECRET_KEY):
                abort(401)

            receiver: Users = Users.select_where(access_token=access_token).first()
        else:
            token = request.headers.get("token")

            if is_bot_token_wrong(token):
                abort(401)

            receiver: Bots = Bots.select_where(token=token).first()

        file: Files = Files.select_where(id=file_id).first()

        print("LIST OF FILES:", os.listdir(os.path.join("app", FILES_SAVING_PATH)))
        print("PATH TO FILE:", os.path.join("app", FILES_SAVING_PATH, str(file_id)))
        print("FILE OBJECT:", file)
        print("FILE EXISTS:", os.path.exists(os.path.join("app", FILES_SAVING_PATH, str(file_id))))
        if file is None or not os.path.exists(os.path.join("app", FILES_SAVING_PATH, str(file_id))):
            abort(404)

        if receiver is None:
            abort(401)

        db_session.delete(file)
        db_session.commit()
        return send_from_directory(FILES_SAVING_PATH, str(file_id), as_attachment=True)

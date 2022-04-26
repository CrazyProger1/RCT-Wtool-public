import os

import flask

from ..user.tools import is_token_wrong as is_user_token_wrong
from ..bot.tools import is_token_wrong as is_bot_token_wrong
from ..models import Users, Bots, Updates
from flask import request, abort, jsonify, send_from_directory
from ..config import *
from .. import db_session, db
import shutil


def save_file(file, name: str, version: str = ""):
    path_to_folder = os.path.join("app" + UPDATES_SAVING_PATH, name)
    path_to_file = os.path.join(path_to_folder, version)

    if os.path.exists(path_to_file):
        os.remove(path_to_file)

    if not os.path.exists(path_to_folder):
        os.mkdir(path_to_folder)

    file.save(path_to_file)


class Update:
    @staticmethod
    def process_input_update() -> flask.Response:

        name = request.args.get("name", default="classic", type=str)
        version = request.args.get("version", default="0.0.0-pre_realize", type=str)
        description = request.args.get("description", default=None, type=str)
        data_format = request.args.get("data_format", default="zip", type=str)

        access_token = request.headers.get("access_token")
        if is_user_token_wrong(access_token, ACCESS_TOKEN_SECRET_KEY):
            abort(401)

        update = Updates(name, version, description, data_format)
        db_session.add(update)
        db_session.commit()

        save_file(
            file=request.files['file'],
            name=name,
            version=version
        )

        db_session.commit()
        return jsonify([update.id])

    @staticmethod
    def get_update(update_name: str, update_version: str) -> flask.Response:

        update: Updates = Updates.select_where(name=update_name, version=update_version).first()

        if update is None \
                or not os.path.exists(os.path.join("app", UPDATES_SAVING_PATH, update_name, update_version)):
            abort(404)

        return send_from_directory(os.path.join(UPDATES_SAVING_PATH, update_name), update_version, as_attachment=True)

    @staticmethod
    def get_updates():
        updates = {}

        for update in Updates.get_all():
            update: Updates
            updates.update({update.id: {
                "name": update.name,
                "version": update.version,
                "description": update.description,
                "data_format": update.data_format
            }})

        return jsonify(updates)

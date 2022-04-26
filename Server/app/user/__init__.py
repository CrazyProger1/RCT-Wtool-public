import flask
from ..models import Users
from .. import db, db_session
from flask import request, jsonify, abort
from .tools import *
from ..config import *


class User:
    @staticmethod
    def register() -> flask.Response:
        json_data = request.get_json()

        username = json_data.get("username")
        password = json_data.get("password")

        if Users.is_username_used(username) \
                or not is_password_valid(password) \
                or not is_username_valid(username):
            abort(403)

        user = Users(username, password)
        print(user)

        db_session.add(user)
        db_session.commit()

        return jsonify({"id": user.get_id(), "api_key": user.get_api_key(), **user.get_tokens()})

    @staticmethod
    def delete(user_id: int) -> flask.Response:
        access_token = request.headers.get("access_token")
        if is_token_wrong(access_token, ACCESS_TOKEN_SECRET_KEY):
            abort(401)

        user: Users = Users.select_where(id=user_id, access_token=access_token).first()

        if user is None:
            abort(403)

        db_session.delete(user)
        db_session.commit()

        return jsonify({})

    @staticmethod
    def login() -> flask.Response:
        json_data = request.get_json()

        username = json_data.get("username")
        password = json_data.get("password")

        user: Users = Users.select_where(username=username).first()

        if user is None:
            abort(401)

        if not user.is_password_right(password):
            abort(401)

        user.update_lot()
        user.refresh_tokens()

        db_session.commit()

        return jsonify({"id": user.get_id(), "api_key": user.get_api_key(), **user.get_tokens()})

    @staticmethod
    def refresh_tokens(user_id: int) -> flask.Response:
        refresh_token = request.headers.get("refresh_token")

        if is_token_wrong(refresh_token, REFRESH_TOKEN_SECRET_KEY, "refresh"):
            abort(401)

        user: Users = Users.select_where(refresh_token=refresh_token, id=user_id).first()

        user.refresh_tokens()

        db_session.commit()

        return jsonify(user.get_tokens())

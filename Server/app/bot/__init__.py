import flask

from ..user.tools import is_token_wrong as is_user_token_wrong
from ..models import Users, Bots
from flask import request, abort, jsonify
from ..config import *
from .. import db_session, db


class Bot:
    @staticmethod
    def register() -> flask.Response:
        is_api_requesting = request.args.get("is_api_requesting", default=False, type=bool)

        if not is_api_requesting:
            access_token = request.headers.get("access_token")
            if is_user_token_wrong(access_token, ACCESS_TOKEN_SECRET_KEY):
                abort(401)

            user_parent: Users = Users.select_where(access_token=access_token).first()
        else:
            api_key = request.headers.get("api_key")
            bot_token = request.headers.get("token")

            bot_creator: Bots = Bots.select_where(token=bot_token).first()
            user_parent: Users = Users.select_where(api_key=api_key).first()

            if user_parent is None or bot_creator is None:
                abort(401)

            if bot_creator.get_parent_id() != user_parent.get_id():
                abort(403)

        json_data = request.get_json()
        name = json_data.get("name")
        is_private = json_data.get("is_private")

        bot = Bots(name, is_private, user_parent.get_id())

        db_session.add(bot)
        db_session.commit()
        return jsonify({"token": bot.get_token()})

    @staticmethod
    def login():
        json_data = request.get_json()
        token = json_data.get("token")
        system_id = request.args.get("system_id", None, int)

        bot: Bots = Bots.select_where(token=token).first()

        if bot is None:
            abort(401)

        if bot.get_system_id() is None:
            bot.system_id = system_id

        bot.update_lot()

        db_session.commit()

        return jsonify({"name": bot.get_name(),
                        "id": bot.get_id(),
                        "parent_id": bot.get_parent_id(),
                        "system_id": bot.get_system_id()})

    @staticmethod
    def get_available_bots(bot_id: None | int = None) -> flask.Response:
        access_token = request.headers.get("access_token")

        if is_user_token_wrong(access_token, ACCESS_TOKEN_SECRET_KEY):
            abort(401)

        user: Users = Users.select_where(access_token=access_token).first()

        if bot_id is not None:
            bot = Bots.select_where(id=bot_id).first()
            if bot is None:
                abort(404)
            elif bot.parent_id != user.id and bot.is_private:
                abort(403)

            return jsonify({bot.get_id(): {
                "name": bot.get_name(),
                "lot": str(bot.lot),
                "is_private": True,
                "parent_id": bot.get_parent_id(),
                "token": bot.get_token() if bot.parent_id == user.id else None
            }})

        bots = {}

        for bot in Bots.select_where(is_private=False).all():
            bot: Bots
            bots.update({bot.get_id(): {
                "name": bot.get_name(),
                "lot": str(bot.lot),
                "is_private": False,
                "parent_id": bot.get_parent_id(),
            }})

            if bot.parent_id == user.id:
                bots.get(bot.get_id()).update({"token": bot.get_token()})

        for bot in Bots.select_where(is_private=True, parent_id=user.id).all():
            bot: Bots
            bots.update({bot.get_id(): {
                "name": bot.get_name(),
                "lot": str(bot.lot),
                "is_private": True,
                "parent_id": bot.get_parent_id(),
                "token": bot.get_token()
            }})
        return jsonify(bots)

    @staticmethod
    def delete(bot_id: int):
        access_token = request.headers.get("access_token")

        if is_user_token_wrong(access_token, ACCESS_TOKEN_SECRET_KEY):
            abort(401)

        user: Users = Users.select_where(access_token=access_token).first()

        bot = Bots.select_where(id=bot_id).first()

        if bot is None:
            abort(404)

        if bot.parent_id != user.id:
            abort(403)

        db_session.delete(bot)
        db_session.commit()

        return jsonify({})

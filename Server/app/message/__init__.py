import datetime

import flask

from ..models import Messages, Users, Bots, Files
from flask import abort, request, jsonify
from ..user.tools import is_token_wrong as is_user_token_wrong
from ..bot.tools import is_token_wrong as is_bot_token_wrong
from ..config import *
from .. import db_session, db


class Message:
    @staticmethod
    def process_input_message() -> flask.Response:
        messages_ids = []
        sender_type = request.args.get("sender_type", default="user", type=str)
        receivers_type = request.args.get("receivers_type", default="user", type=str)

        sender_id = request.args.get("sender_id", default=None, type=int)
        receivers_ids = request.args.getlist("receivers_ids")

        is_reply = request.args.get("is_reply", default=False, type=bool)
        reply_on = request.args.get("reply_on", default=0, type=int)

        file_id = request.args.get("file_id", default=0, type=int)

        if type(receivers_ids) is not list:
            abort(400)

        elif type(receivers_ids) is int:

            receivers_ids = [receivers_ids]

        try:
            receivers_ids = map(int, receivers_ids)
        except ValueError as e:
            print(e)

        message_type = request.args.get("message_type", default="text", type=str)

        if message_type == "file":
            if Files.select_where(id=file_id).first() is None:
                abort(403)

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

        json_data = request.get_json()

        for receiver_id in receivers_ids:
            if receivers_type == "user":
                user = Users.select_where(id=receiver_id).first()
                if user is None:
                    continue
            else:

                bot: Bots = Bots.select_where(id=receiver_id).first()
                if bot is None:
                    continue
                if bot.is_private is True:
                    if sender.id != bot.get_parent_id() or sender_type != "user":
                        continue
            message = Messages(
                sender_id,
                receiver_id,
                sender_type,
                receivers_type,
                message_type,
                json_data.get("content"),
                json_data.get("json_content"),
                json_data.get("command"),
                is_reply,
                reply_on,
                file_id
            )

            db_session.add(message)
            db_session.commit()
            messages_ids.append(message.get_id())

        return jsonify(messages_ids)

    @staticmethod
    def get_messages() -> flask.Response:
        receiver_type = request.args.get("receiver_type", default="user", type=str)

        messages = {}

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

        for message in Messages.select_where(receiver_id=receiver.id, receiver_type=receiver_type).all():
            file: Files | None = None
            if message.message_type == "file":
                file: Files = Files.select_where(id=message.file_id).first()

            message: Messages
            messages.update({message.get_id(): {
                "sender_id": message.sender_id,
                "sender_type": message.sender_type,
                "message_type": message.message_type,
                "content": message.content,
                "command": message.command,
                "json_content": message.json_content,
                "sending_datetime": message.sending_datetime,
                "is_reply": message.is_reply,
                "reply_on": message.reply_on if message.is_reply else 0,
                "file_id": message.file_id if message.message_type == "file" else 0,
                "filename": file.filename if file is not None else None,
            }})
            print(messages)

            db_session.delete(message)

        db_session.commit()

        return jsonify(messages)

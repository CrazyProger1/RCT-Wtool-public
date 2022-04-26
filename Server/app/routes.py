from . import app
from flask import request
from .user import User
from .bot import Bot
from .message import Message
from .file import File
from .update import Update
from .module import Module


@app.route("/")
def index():
    return "HELLO!"


# USER
@app.route("/api/users/register", methods=["POST"])
def register_user():
    return User.register()


@app.route("/api/users/login", methods=["POST"])
def login_user():
    return User.login()


@app.route("/api/users/<user_id>", methods=["PUT"])
def edit_user(user_id: int):
    return ""


@app.route("/api/users/<user_id>", methods=["DELETE"])
def delete_user(user_id: int):
    return User.delete(user_id)


@app.route("/api/users/<user_id>/tokens", methods=["PUT"])
def refresh_tokens(user_id: int):
    return User.refresh_tokens(user_id)


# BOT
@app.route("/api/bots/register", methods=["POST"])
def register_bot():
    return Bot.register()


@app.route("/api/bots/login", methods=["POST"])
def login_bot():
    return Bot.login()


@app.route("/api/bots/<bot_id>", methods=["PUT"])
def edit_bot(bot_id: int):
    return ""


@app.route("/api/bots", methods=["GET"])
def get_available_bots():
    return Bot.get_available_bots()


@app.route("/api/bots/<bot_id>", methods=["GET"])
def get_bot(bot_id: int):
    return Bot.get_available_bots(bot_id)


@app.route("/api/bots/<bot_id>", methods=["DELETE"])
def delete_bot(bot_id: int):
    return Bot.delete(bot_id)


# MESSAGES
@app.route("/api/messages", methods=["POST"])
def process_message():
    return Message.process_input_message()


@app.route("/api/messages", methods=["GET"])
def get_messages():
    return Message.get_messages()


# FILES
@app.route("/api/files", methods=["POST"])
def process_file():
    return File.process_input_file()


@app.route("/api/files/<file_id>", methods=["GET"])
def get_file(file_id: int):
    return File.get_file(file_id)


# UPDATES
@app.route("/api/updates", methods=["POST"])
def process_update():
    return Update.process_input_update()


@app.route("/api/updates/<update_name>/<update_version>", methods=["GET"])
def get_update(update_name, update_version):
    return Update.get_update(update_name, update_version)


@app.route("/api/updates", methods=["GET"])
def get_updates():
    return Update.get_updates()


# MODULES
@app.route("/api/modules", methods=["POST"])
def process_module():
    return Module.process_input_module()


@app.route("/api/modules/<module_name>/<module_version>", methods=["GET"])
def get_module(module_name, module_version):
    return Module.get_module(module_name, module_version)


@app.route("/api/modules", methods=["GET"])
def get_modules():
    return Module.get_modules()


# SPREAD PACKS
@app.route("/api/packs/<pack_name>/<pack_version>", methods=["GET"])
def get_pack(pack_name, pack_version):
    return ""

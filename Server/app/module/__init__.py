import os

import flask

from ..user.tools import is_token_wrong as is_user_token_wrong
from ..bot.tools import is_token_wrong as is_bot_token_wrong
from ..models import Users, Bots, Modules
from flask import request, abort, jsonify, send_from_directory
from ..config import *
from .. import db_session, db
import shutil


def save_file(file, name: str, version: str = ""):
    path_to_folder = os.path.join("app" + MODULE_SAVING_PATH, name)
    path_to_file = os.path.join(path_to_folder, version)

    if os.path.exists(path_to_file):
        os.remove(path_to_file)

    if not os.path.exists(path_to_folder):
        os.mkdir(path_to_folder)

    file.save(os.path.join(
        path_to_folder,
        version
    ))


class Module:
    @staticmethod
    def process_input_module() -> flask.Response:
        name = request.args.get("name", default="classic", type=str)
        version = request.args.get("version", default="0.0.0-pre_realize", type=str)
        description = request.args.get("description", default=None, type=str)

        access_token = request.headers.get("access_token")
        if is_user_token_wrong(access_token, ACCESS_TOKEN_SECRET_KEY):
            abort(401)

        module = Modules(name, version, description)
        db_session.add(module)
        db_session.commit()

        save_file(
            file=request.files['file'],
            name=name,
            version=version
        )

        db_session.commit()
        return jsonify([module.id])

    @staticmethod
    def get_module(module_name: str, module_version: str) -> flask.Response:

        module: Modules = Modules.select_where(name=module_name, version=module_version).first()

        if module is None \
                or not os.path.exists(os.path.join("app", MODULE_SAVING_PATH, module_name, module_version)):
            abort(404)

        return send_from_directory(os.path.join(MODULE_SAVING_PATH, module_name), module_version, as_attachment=True)

    @staticmethod
    def get_modules():
        modules = {}

        for module in Modules.get_all():
            module: Modules
            modules.update({module.id: {
                "name": module.name,
                "version": module.version,
                "description": module.description,
            }})

        return jsonify(modules)

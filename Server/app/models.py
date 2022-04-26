import json
from sqlalchemy.orm.attributes import flag_modified
import flask
import datetime
import jwt
import hashlib
from string import ascii_letters
from random import randint
from . import db, db_session
from flask import request, abort, jsonify
from .config import *
from .model_tools import *

Model = db.Model
Column = db.Column
StringType = db.String
IntegerType = db.Integer
JsonType = db.JSON
BooleanType = db.Boolean
DateTimeType = db.DateTime
FloatType = db.Float
LargeBinaryType = db.LargeBinary


class Users(Model):
    __tablename__ = "users"

    id = Column(IntegerType, primary_key=True)
    username = Column(StringType(256), unique=True, nullable=False)
    password = Column(LargeBinaryType(1024))
    salt = Column(LargeBinaryType(10))
    lot = Column(DateTimeType)  # last online time
    api_key = Column(StringType(32))

    access_token = Column(StringType(256))
    refresh_token = Column(StringType(256))

    def __init__(self, username: str, password: str):
        self.username = username
        self.salt = generate_token(10).encode()
        self.password = encrypt_password(password, self.salt, SALT_2)
        self.api_key = generate_token(32)

        self.update_lot()
        self.refresh_tokens()

    def update_lot(self):
        self.lot = datetime.datetime.now()

    def refresh_tokens(self):
        self.access_token = generate_jwt_token(self.username, ACCESS_TOKEN_SECRET_KEY, ACCESS_TOKEN_ALIVE_DAYS)
        self.refresh_token = generate_jwt_token(self.username, REFRESH_TOKEN_SECRET_KEY, REFRESH_TOKEN_ALIVE_DAYS)

    def is_password_right(self, password):
        return encrypt_password(password, self.salt, SALT_2) == self.password

    def get_id(self):
        return self.id

    def get_tokens(self) -> dict:
        return {"access_token": self.access_token, "refresh_token": self.refresh_token}

    def get_api_key(self):
        return self.api_key

    @staticmethod
    def is_username_used(username: str):
        return Users.select_where(username=username).first() is not None

    @staticmethod
    def get_all():
        return Users.query.all()

    @staticmethod
    def select_where(**where):
        return Users.query.filter_by(**where)

    def __repr__(self):
        return f"User<id={self.id}, username={self.username}>"


class Bots(Model):
    __tablename__ = "bots"

    id = Column(IntegerType, primary_key=True)
    system_id = Column(IntegerType, default=None)
    name = Column((StringType(256)))
    token = Column((StringType(32)))

    lot = Column(DateTimeType)  # last online time
    parent_id = Column(IntegerType, db.ForeignKey('users.id'), nullable=True)
    is_private = Column(BooleanType, default=False)

    def __init__(self, name: str, is_private: bool, parent_id: int):
        self.name = name
        self.is_private = is_private
        self.token = generate_token(32)
        self.parent_id = None if parent_id == 0 else parent_id

        self.update_lot()

    def update_lot(self):
        self.lot = datetime.datetime.now()

    def get_id(self):
        return self.id

    def get_token(self):
        return self.token

    def get_parent_id(self):
        return self.parent_id

    def get_name(self):
        return self.name

    def get_system_id(self):
        return self.system_id

    @staticmethod
    def get_all():
        return Bots.query.all()

    @staticmethod
    def select_where(**where):
        return Bots.query.filter_by(**where)

    def __repr__(self):
        return f"Bot<id={self.id}, name={self.name}>"


class Messages(Model):
    __tablename__ = "messages"

    id = Column(IntegerType, primary_key=True)

    sender_id = Column(IntegerType)
    receiver_id = Column(IntegerType)
    sender_type = Column(StringType(10))
    receiver_type = Column(StringType(10))

    message_type = Column(StringType(10))
    content = Column(StringType(8192))
    json_content = Column(JsonType)
    command = Column(StringType(1024))

    file_id = Column(IntegerType, db.ForeignKey('files.id'), nullable=True)

    sending_datetime = Column(DateTimeType)

    is_reply = Column(BooleanType, default=False)
    reply_on = Column(IntegerType, db.ForeignKey('messages.id'), nullable=True)

    def __init__(self,
                 sender_id: int,
                 receiver_id: int,
                 sender_type: str,
                 receiver_type: str,
                 message_type: str,
                 content: str,
                 json_content: str,
                 command: str,
                 is_reply: bool,
                 reply_on: int,
                 file_id: int):
        self.sender_id = sender_id
        self.receiver_id = receiver_id
        self.sender_type = sender_type
        self.receiver_type = receiver_type

        self.message_type = message_type
        self.content = content
        self.json_content = json_content
        self.command = command

        self.file_id = None if file_id == 0 else file_id

        self.sending_datetime = datetime.datetime.now()

        self.is_reply = is_reply

        if Messages.select_where(id=reply_on).first() is not None:
            self.reply_on = None if reply_on == 0 else reply_on

    def get_id(self):
        return self.id

    @staticmethod
    def get_all():
        return Messages.query.all()

    @staticmethod
    def select_where(**where):
        return Messages.query.filter_by(**where)


class Files(Model):
    __tablename__ = "files"

    id = Column(IntegerType, primary_key=True)
    filename = Column(StringType(128))

    def __init__(self, filename: str):
        self.filename = filename

    @staticmethod
    def select_where(**where):
        return Files.query.filter_by(**where)

    @staticmethod
    def get_all():
        return Files.query.all()

    def __repr__(self):
        return f"File<id={self.id}, filename={self.filename}>"


class Updates(Model):
    __tablename__ = "updates"

    id = Column(IntegerType, primary_key=True)
    name = Column(StringType(128))
    version = Column(StringType(16))
    description = Column(StringType(2048))
    data_format = Column(StringType(16))

    def __init__(self, name: str, version: str, description: str = None, data_format: str = "zip"):
        self.name = name
        self.version = version
        self.description = description
        self.data_format = data_format

    @staticmethod
    def select_where(**where):
        return Updates.query.filter_by(**where)

    @staticmethod
    def get_all():
        return Updates.query.all()

    def __repr__(self):
        return f"Update<id={self.id}, name={self.name}, ver={self.version}>"


class Modules(Model):
    __tablename__ = "modules"

    id = Column(IntegerType, primary_key=True)
    name = Column(StringType(128))
    version = Column(StringType(16))
    description = Column(StringType(2048))

    def __init__(self, name: str, version: str, description: str = None):
        self.name = name
        self.version = version
        self.description = description

    @staticmethod
    def select_where(**where):
        return Modules.query.filter_by(**where)

    @staticmethod
    def get_all():
        return Modules.query.all()

    def __repr__(self):
        return f"Module<id={self.id}, name={self.name}, ver={self.version}>"

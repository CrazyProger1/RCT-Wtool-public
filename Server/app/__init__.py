from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from .config import *


def crate_app() -> Flask:
    return Flask(__name__)


def configure_app():
    app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = True
    database_url = os.getenv("DATABASE_URL")
    # database_url = "postgresql://postgres:PHPython@localhost:5432/rctbrutool"
    if "postgres://" in database_url:
        database_url = database_url.replace("postgres://", "postgresql://")
    app.config['SQLALCHEMY_DATABASE_URI'] = database_url  # DATABASE_URI
    app.config['SECRET_KEY'] = SECRET_KEY


def configure_db():
    pass
    # print(os.getenv("DATABASE_URL"))


if __name__ != '__main__':
    app = crate_app()
    configure_app()

    db = SQLAlchemy(app)
    db_session = db.session
    configure_db()

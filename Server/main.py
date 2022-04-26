from app import app, db, db_session
from app.config import *
from app.routes import *
from app.models import *


def run_app():
    app.run(HOST, PORT, DEBUG)


db.create_all()

if __name__ == '__main__':
    # print("created database and tables")
    run_app()

import os

HOST = "localhost"
PORT = "2288"
DEBUG = False

DATABASE_FILENAME = "app.db"
DATABASE_FOLDER = "database"
DATABASE_URI = "sqlite:///" + os.path.join(DATABASE_FOLDER, DATABASE_FILENAME)  # postgresql:// sqlite:///
FILES_SAVING_PATH = "files/temp"
UPDATES_SAVING_PATH = "files/updates"
MODULE_SAVING_PATH = "files/modules"

FAVICONS_DIRECTORY = "./favicons"
FAVICON_FILENAME = "favicon.ico"

ACCESS_TOKEN_ALIVE_DAYS = 1
REFRESH_TOKEN_ALIVE_DAYS = 5

SECRET_KEY = "SA23daAdw3SD>)8*7llL.d/|21|23>2*"
ACCESS_TOKEN_SECRET_KEY = '>d?wL_<,dkY.|/wTg:"qJZz+O='
REFRESH_TOKEN_SECRET_KEY = 'K";d(J&kdU*@l>dn<#eRl,WMq'
SALT_2 = b"IASOdiu87u81jm"

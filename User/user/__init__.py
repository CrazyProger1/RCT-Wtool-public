import os.path

from .config import _Config
from .message import Message
import requests
import json
from .errors import *
from .bot import Bot
import urllib.request


class User:
    def __init__(self, username: str | None = None, password: str | None = None, config_file: str | None = None):
        self._config = _Config(config_file or "config.json")
        self.id: int = 0
        self.username = username or self._config.get_username()
        self.password = password or self._config.get_password()
        self.access_token: str | None = None
        self.refresh_token: str | None = None
        self.api_key: str | None = None

    def get_api_key(self):
        return self.api_key

    def register(self) -> int:
        data = {
            "username": self.username,
            "password": self.password
        }

        response = requests.post(self._config.users_page + "/register", json=data)

        match response.status_code:
            case 200:
                out_data: dict = response.json()
                self.access_token = out_data.get("access_token")
                self.refresh_token = out_data.get("refresh_token")
                self.id = out_data.get("id")
                self.api_key = out_data.get("api_key")
                return self.id

            case 403:
                raise Forbidden("Name was already use")

            case 500:
                raise InternalServerError("Server error")

    def delete(self):
        response = requests.delete(self._config.users_page + f"/{self.id}", headers={"access_token": self.access_token})

        match response.status_code:
            case 401:
                raise Unauthorized("Wrong token")
            case 200:
                pass

    def delete_bot(self, bot_id: int):
        response = requests.delete(self._config.bots_page + f"/{bot_id}", headers={"access_token": self.access_token})

        match response.status_code:
            case 401:
                raise Unauthorized("Wrong token")
            case 404:
                raise NotFound("Wrong bot id")
            case 403:
                raise Forbidden("Access denied")
            case 200:
                pass

    def refresh_tokens(self):
        response = requests.put(self._config.users_page + f"/{self.id}" + f"/tokens",
                                headers={"refresh_token": self.refresh_token})
        match response.status_code:
            case 401:
                raise Unauthorized("Wrong refresh token")
            case 200:
                out_data: dict = response.json()

                self.refresh_token = out_data.get("refresh_token")
                self.access_token = out_data.get("access_token")


    def login(self) -> int:
        data = {
            "username": self.username,
            "password": self.password
        }

        response = requests.post(self._config.users_page + "/login", json=data)

        match response.status_code:
            case 200:
                json_output: dict = response.json()
                self.access_token = json_output.get("access_token")
                self.refresh_token = json_output.get("refresh_token")
                self.api_key = json_output.get("api_key")
                self.id = json_output.get("id")
                return self.id
            case 401:
                raise Unauthorized("Wrong name or password")

    def create_bot(self, name: str, is_private: bool = False):
        data = {
            "name": name,
            "is_private": is_private,

        }

        headers = {"access_token": self.access_token}

        response = requests.post(
            self._config.bots_page + "/register",
            json=data,
            headers=headers
        )

        match response.status_code:
            case 200:
                json_output: dict = response.json()
                token = json_output.get("token")
                return token
            case 401:
                raise Unauthorized("Wrong access token")

    def get_available_bots(self) -> list[Bot]:
        headers = {"access_token": self.access_token}

        response = requests.get(
            self._config.bots_page,
            headers=headers
        )
        match response.status_code:
            case 200:
                bots = []

                bots_info: dict = response.json()

                for bot_id, bot_info in bots_info.items():
                    bots.append(Bot(bot_id, **bot_info))

                return bots
            case 401:
                raise Unauthorized("Wrong access token")

    def get_bot_by_id(self, bot_id: int) -> Bot:
        headers = {"access_token": self.access_token}

        response = requests.get(
            self._config.bots_page + "/" + str(bot_id),
            headers=headers
        )
        match response.status_code:
            case 200:
                bot_info: dict = response.json()
                return Bot(bot_id, **bot_info.get(str(bot_id)))
            case 401:
                raise Unauthorized("Wrong access token")

    def _send_message(self, receiver_id: int,
                      content: str = None,
                      json_content: str = None,
                      command: str = None,
                      receiver_type: str = "bot",
                      for_group: bool = False,
                      receivers_ids: list | tuple = (),
                      is_file: bool = False,
                      file_id: int = 0):
        headers = {"access_token": self.access_token}

        params = {
            "sender_type": "user",
            "sender_id": self.id,
            "receivers_type": receiver_type,
            "receivers_ids": [receiver_id] if not for_group else receivers_ids,
            "message_type": "text" if not is_file else "file",
            "file_id": file_id

        }

        data = {
            "content": content,
            "json_content": json_content,
            "command": command
        }

        response = requests.post(
            self._config.messages_page,
            headers=headers,
            json=data,
            params=params
        )

        match response.status_code:
            case 200:
                return response.json()
            case 401:
                raise Exception("Wrong access token")

    def send_message(self, receiver_id: int,
                     content: str = None,
                     json_content: str = None,
                     command: str = None,
                     receiver_type: str = "bot",
                     for_group: bool = False,
                     receivers_ids: list | tuple = (),
                     ) -> list:
        return self._send_message(receiver_id, content, json_content, command, receiver_type, for_group, receivers_ids)

    def send_file(self,
                  receiver_id: int,
                  filepath: str,
                  end_filename: str,
                  content: str = None,
                  json_content: str = None,
                  command: str = None,
                  receiver_type: str = "bot",
                  for_group: bool = False,
                  receivers_ids: list | tuple = ()):

        headers = {"access_token": self.access_token}
        params = {
            "sender_type": "user",
            "filename": end_filename
        }

        fp = open(filepath, 'rb')

        files = {"file": fp}

        response = requests.post(
            self._config.files_page,
            headers=headers,
            params=params,
            files=files
        )
        fp.close()

        print(response.status_code)
        match response.status_code:
            case 200:
                return self._send_message(receiver_id,
                                          content,
                                          json_content,
                                          command,
                                          receiver_type,
                                          for_group,
                                          receivers_ids,
                                          True,
                                          response.json()[0])
            case 401:
                raise Unauthorized("Wrong access token")

    def save_file(self, msg: Message):
        if msg.message_type == "file":
            opener = urllib.request.build_opener()
            opener.addheaders = [("access_token", self.access_token)]
            urllib.request.install_opener(opener)
            urllib.request.urlretrieve(_Config().files_page + f"/{msg.file_id}?receiver_type=user", msg.filename)

    def get_messages(self) -> list[Message]:
        headers = {"access_token": self.access_token}

        params = {"receiver_type": "user"}

        response = requests.get(
            self._config.messages_page,
            headers=headers,
            params=params
        )

        match response.status_code:
            case 200:
                msgs = []
                messages_dict: dict = response.json()
                for msg_id, msg_data in messages_dict.items():
                    msg = Message(
                        msg_id,
                        msg_data.get("sender_id"),
                        msg_data.get("sender_type"),
                        msg_data.get("message_type"),
                        msg_data.get("content"),
                        msg_data.get("json_content"),
                        msg_data.get("command"),
                        msg_data.get("sending_datetime"),
                        msg_data.get("is_reply"),
                        msg_data.get("reply_on"),
                        msg_data.get("file_id"),
                        msg_data.get("filename")
                    )
                    msgs.append(msg)

                return msgs
            case 401:
                raise Unauthorized("Wrong access token")

    def upload_update(self, filepath: str, name: str, version: str, description: str = None, data_format: str = "zip"):
        fp = open(filepath, 'rb')

        assert os.path.splitext(filepath)[1].lower() in [".zip", ".rar", "." + data_format]

        files = {"file": fp}
        response = requests.post(
            self._config.updates_page,
            headers={"access_token": self.access_token},
            params={"name": name, "version": version, "description": description, "data_format": data_format},
            files=files
        )

        fp.close()

    def upload_module(self, filepath: str, name: str, version: str, description: str = None):
        fp = open(filepath, 'rb')
        files = {"file": fp}

        response = requests.post(
            self._config.modules_page,
            headers={"access_token": self.access_token},
            params={"name": name, "version": version, "description": description},
            files=files
        )

        fp.close()

from .user_api import _UserAPI
from .config import _Config
import requests


class Bot:
    def __init__(self, token: str, config_file: str | None = None):
        self.id: int | None = None
        self.token = token
        self._config = _Config(config_file if config_file is not None else "config.json")
        self.user_api = _UserAPI(None, self._config, self)

    def login(self):
        data = {
            "token": self.token,
        }

        response = requests.post(self._config.bots_page, json=data, params={"auth_type": "login"})

        match response.status_code:
            case 200:
                json_output: dict = response.json()
                self.id = json_output.get("id")
                return self.id
            case 401:
                raise Exception("Wrong token")

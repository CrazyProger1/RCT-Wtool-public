import requests
from .config import _Config


class _UserAPI:
    def __init__(self, api_key: str | None, config: _Config, bot):
        self.api_key = api_key
        self.bot = bot
        self._config = config

    def set_api_key(self, api_key: str):
        self.api_key = api_key

    def create_bot(self, name: str, is_private: bool = False):
        data = {
            "name": name,
            "is_private": is_private,

        }

        params = {"auth_type": "registration", "is_api_requesting": True}
        headers = {"api_key": self.api_key, "token": self.bot.token}

        response = requests.post(
            self._config.bots_page,
            json=data,
            headers=headers,
            params=params
        )

        match response.status_code:
            case 200:
                json_output: dict = response.json()
                token = json_output.get("token")
                return token

            case 401:
                raise Exception("Wrong token")

            case 403:
                raise Exception("Wrong api key")

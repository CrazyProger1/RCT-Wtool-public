import json


class _Config:
    def __init__(self, config_file: str):
        self.config_file = config_file

        self.users_page = ""
        self.bots_page = ""

        self.domain = ""
        self._read_config()

    def _read_config(self):
        with open(self.config_file, "r") as config_file:
            config = json.load(config_file)
            self.domain = config.get("domain")

            self.users_page = f"{self.domain}/users"
            self.bots_page = f"{self.domain}/bots"

import json

_instances = {}


def singleton(cls):
    def wrapper(*args, **kwargs):
        if cls not in _instances.keys():
            _instances.update({cls: cls(*args, **kwargs)})
        return _instances.get(cls)

    return wrapper


@singleton
class _Config:
    def __init__(self, config_file: str):
        self.config_file = config_file

        self.users_page = ""
        self.bots_page = ""
        self.messages_page = ""
        self.files_page = ""
        self.updates_page = ""
        self.modules_page = ""
        self.user = {}

        self.domain = ""
        self._read_config()

    def _read_config(self):
        with open(self.config_file, "r") as config_file:
            config = json.load(config_file)
            self.domain = config.get("domain")

            self.users_page = f"{self.domain}/api/users"
            self.bots_page = f"{self.domain}/api/bots"
            self.messages_page = f"{self.domain}/api/messages"
            self.files_page = f"{self.domain}/api/files"
            self.updates_page = f"{self.domain}/api/updates"
            self.modules_page = f"{self.domain}/api/modules"

            self.user = config.get("user")

    def get_username(self):
        if self.user is not None:
            return self.user.get("username")

    def get_password(self):
        if self.user is not None:
            return self.user.get("password")

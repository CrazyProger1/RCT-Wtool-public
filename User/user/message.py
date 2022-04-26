import datetime
from .config import _Config
import urllib.request


class Message:
    def __init__(self, msg_id: int,
                 sender_id: int,
                 sender_type: str,
                 message_type: str,
                 content: str,
                 json_content: str,
                 command: str,
                 sending_datetime: str,
                 is_reply: bool,
                 reply_on: int,
                 file_id: int,
                 filename: str):
        self.id = msg_id
        self.sender_id = sender_id,
        self.sender_type = sender_type,
        self.message_type = message_type
        self.content = content
        self.json_content = json_content
        self.sending_datetime = sending_datetime
        # self.sending_datetime = datetime.datetime.strptime(sending_datetime, "%a, %d %b ")
        self.is_reply = is_reply
        self.command = command
        self.reply_on = reply_on
        self.file_id = file_id
        self.filename = filename

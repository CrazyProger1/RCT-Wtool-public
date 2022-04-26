from ..models import Bots


def is_token_wrong(token: str):
    return token is None or Bots.select_where(token=token).first() is None

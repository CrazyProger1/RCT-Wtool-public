import jwt
from ..models import Users


def is_password_valid(password: str):
    return password is not None


def is_username_valid(username: str):
    return username is not None


def is_token_wrong(token: str, secret_key: str, token_type: str = 'access'):
    if token is None:
        return True

    try:
        payload = jwt.decode(jwt=token, key=secret_key, algorithms=['HS256'])

        if token_type == 'access':
            user: Users | None = Users.select_where(access_token=token).first()
        else:
            user: Users | None = Users.select_where(refresh_token=token).first()

        if user is None:
            return True

        if user.username != payload.get('name'):
            return True

        return False
    except jwt.exceptions.ExpiredSignatureError:
        return True


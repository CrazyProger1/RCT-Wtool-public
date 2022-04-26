import datetime
import jwt
import hashlib
from string import ascii_letters
from random import randint


def generate_jwt_token(username: str, secret_key: str, alive_days: int) -> str:
    return jwt.encode(
        payload={'name': username, 'exp': datetime.datetime.utcnow() + datetime.timedelta(days=alive_days)},
        # will be corrected
        key=secret_key,
        algorithm='HS256'
    )


def generate_token(length: int, symbols_list: str = ascii_letters) -> str:
    return "".join([
        symbols_list[randint(0, len(symbols_list) - 1)]
        for i in range(length)
    ])


def encrypt_password(password: str | bytes, salt: bytes, salt2: bytes) -> bytes:
    if type(password) is str:
        password = password.encode()

    encrypted_salt = hashlib.sha512(salt)
    encrypted_salt_2 = hashlib.sha256(salt2)

    pwd_hash = hashlib.md5(password)
    pwd_hash = hashlib.sha256(pwd_hash.digest())
    pwd_hash = hashlib.sha224(pwd_hash.digest() + encrypted_salt.digest())
    pwd_hash = hashlib.sha384(pwd_hash.digest())
    pwd_hash = hashlib.sha512(pwd_hash.digest() + encrypted_salt_2.digest())
    return pwd_hash.digest()

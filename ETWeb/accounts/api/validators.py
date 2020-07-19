from django.core.validators import validate_email
from django.core.exceptions import ValidationError
from django.conf import settings
import re


def is_email_valid(email):
    try:
        validate_email(email)
    except ValidationError as ex:
        raise ValidationError({'email': 'Invalid email.'})

    return True


def is_password_valid(password1, password2):
    error_msg = {}

    if password1 != password2:
        error_msg['password'] = 'Passwords do not match, please try again.'

    elif len(password1) < settings.MIN_PASS_LENGTH:
        error_msg['password'] = 'Password is too short'

    elif not re.findall('\d', password1):
        error_msg['password'] = 'The password must contain at least 1 digit from 0-9.'

    elif not re.findall('[A-Z]', password1):
        error_msg['password'] = 'The password must contain at least 1 uppercase letter from A-Z.'

    elif not re.findall('[a-z]', password1):
        error_msg['password'] = 'The password must contain at least 1 lowercase letter from a-z.'

    if error_msg:
        raise ValidationError(error_msg)

    return True


def is_username_valid(username):
    error_msg = {}

    if not re.findall(r'^[\w.@+-]+\Z', username):
        # Display error message, only allow special characters @, ., +, -, and _.
        error_msg['username'] = 'This value may contain only alphanumeric values and @/./+/-/_ characters.'

    if error_msg:
        raise ValidationError(error_msg)

    return True

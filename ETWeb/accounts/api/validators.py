from django.contrib.auth.password_validation import validate_password, get_password_validators
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
    if password1 != password2:
        raise ValidationError({'password': 'Passwords do not match, please try again.'})

    validate_password(
        password1,
        password_validators=get_password_validators(settings.AUTH_PASSWORD_VALIDATORS)
    )

    return True


def is_username_valid(username):
    if not re.findall(r'^[\w.@+-]+\Z', username):
        # Display error message, only allow special characters @, ., +, -, and _.
        raise ValidationError({
            'username': 'This value may contain only alphanumeric values and @/./+/-/_ characters.'
        })

    return True

from django.middleware.csrf import CsrfViewMiddleware
from django.core.signing import Signer, BadSignature
from django.core import signing
from django.conf import settings
from django.contrib.auth import get_user_model
from django.contrib.auth.models import AnonymousUser
from rest_framework.authentication import BaseAuthentication
from rest_framework import exceptions
from rest_framework.authtoken.models import Token
from channels.db import database_sync_to_async
from channels.auth import CookieMiddleware

User = get_user_model()


class CSRFCheck(CsrfViewMiddleware):
    def _reject(self, request, reason):
        # Return the failure reason instead of an HttpResponse
        return reason


class SafeTokenAuthentication(BaseAuthentication):
    """
        Custom token based authentication class for DRF
        https://github.com/encode/django-rest-framework/blob/master/rest_framework/authentication.py
    """

    def authenticate(self, request):
        # print(f'Request cookies {request.COOKIES}')

        auth_header = request.headers.get('Authorization')

        if auth_header:
            # try to authorize by token provided in authorization header
            try:
                # header = 'Token xxxxxxxxxxxxxxxxxxxxxxxx'
                auth_token = auth_header.split(' ')[1]
            except IndexError:
                raise exceptions.AuthenticationFailed('Token prefix missing')
        else:
            # try to authorize by token from httponly cookie set by LoginView
            auth_token = request.get_signed_cookie(settings.AUTH_TOKEN_KEY, default=None,
                                                   salt=settings.SIGNED_COOKIE_SALT)
            # print(auth_token)
            if not auth_token:
                return None

        try:
            user = Token.objects.get(key=auth_token).user
        except Token.DoesNotExist:
            raise exceptions.AuthenticationFailed('Could not authenticate with provided credentials.')

        if not user:
            raise exceptions.AuthenticationFailed('User not found')

        if not user.is_active:
            raise exceptions.AuthenticationFailed('User is inactive')

        self.enforce_csrf(request)
        # print(user)
        return user, None

    def enforce_csrf(self, request):
        """
            Enforce CSRF validation
        """
        check = CSRFCheck()
        # populates request.META['CSRF_COOKIE'], which is used in process_view()
        check.process_request(request)
        reason = check.process_view(request, None, (), {})

        if reason:
            print(reason)
            # CSRF failed, bail with explicit error message
            raise exceptions.PermissionDenied('CSRF Failed: %s' % reason)


class TokenAuthMiddleware:
    """
        Token authorization middleware for Django Channels 2
    """

    def __init__(self, inner):
        self.inner = inner

    def __call__(self, scope):
        headers = dict(scope['headers'])
        # print(scope['cookies'])
        # print('\n'.join(map(str, list(zip(list(headers.keys()), headers.values())))))

        token_key = ''
        if b'authorization' in headers:
            token_name, token_key = headers[b'authorization'].decode().split()
            if token_name.lower() != 'token':
                print('Token name invalid')
                token_key = ''
        else:
            cookies = scope['cookies']
            try:
                cookie_value = cookies[settings.AUTH_TOKEN_KEY]
                salt = settings.AUTH_TOKEN_KEY + settings.SIGNED_COOKIE_SALT

                token_key = signing.get_cookie_signer(salt=salt).unsign(cookie_value)
            except BadSignature:
                print('Token is tampered')
            except KeyError:
                # auth token cookie not set
                pass

        scope['user'] = self.get_user(token_key)

        return self.inner(scope)

    @database_sync_to_async
    def get_user(self, key):
        try:
            print('Retrieving user for token {}'.format(key))
            token = Token.objects.get(key=key)
            return token.user
        except Token.DoesNotExist:
            return AnonymousUser()


# set shorthand for TokenAuthMiddleware, since it requires CookieMiddleware
TokenAuthMiddlewareStack = lambda inner: CookieMiddleware(TokenAuthMiddleware(inner))

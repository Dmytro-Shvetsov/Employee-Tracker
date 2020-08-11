from channels.db import database_sync_to_async
from rest_framework.authtoken.models import Token
from django.contrib.auth.models import AnonymousUser


class TokenAuthMiddlewareStack:
    """
    Token authorization middleware for Django Channels 2
    """

    def __init__(self, inner):
        self.inner = inner

    def __call__(self, scope):
        print(scope.keys())
        print(scope['cookies'])
        headers = dict(scope['headers'])
        print('\n'.join(map(str, list(zip(list(headers.keys()), headers.values())))))

        if b'authorization' in headers:
            # try:
            token_name, token_key = headers[b'authorization'].decode().split()
            if token_name.lower() == 'token':
                scope['user'] = self.get_user(token_key)
            else:
                scope['user'] = AnonymousUser()

        scope['user'] = self.get_user(None)

        return self.inner(scope)

    @database_sync_to_async
    def get_user(self, key):
        try:
            print('retrieving user for token {}'.format(key))
            token = Token.objects.get(key=key)
            return token.user
        except Token.DoesNotExist:
            return AnonymousUser()


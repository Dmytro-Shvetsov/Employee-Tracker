from channels.auth import AuthMiddlewareStack
from channels.db import database_sync_to_async, close_old_connections
from rest_framework.authtoken.models import Token
from django.contrib.auth.models import AnonymousUser


class TokenAuthMiddlewareInstance:
    """
    Token authorization middleware for Django Channels 2
    """

    def __init__(self, middleware, scope):
        self.middleware = middleware
        self.scope = dict(scope)
        self.inner = self.middleware.inner

    @database_sync_to_async
    def get_user(self, key):
        try:
            print('retrieving user for token {}'.format(key))
            token = Token.objects.get(key=key)
            return token.user
        except Token.DoesNotExist:
            return AnonymousUser()

    async def __call__(self, receive, send):
        headers = dict(self.scope['headers'])

        if b'authorization' in headers:
            try:
                token_name, token_key = headers[b'authorization'].decode().split()
                if token_name.lower() == 'token':
                    self.scope['user'] = await self.get_user(token_key)
                    print("Identified user {}".format(self.scope['user'].username))
            except Token.DoesNotExist:
                self.scope['user'] = AnonymousUser()

        return self.inner(self.scope)


class TokenAuthMiddleware:
    def __init__(self, inner):
        self.inner = inner

    def __call__(self, scope):
        return TokenAuthMiddlewareInstance(self, scope)


TokenAuthMiddlewareStack = lambda inner: \
    TokenAuthMiddleware(
        AuthMiddlewareStack(inner)
    )

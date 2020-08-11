import os
from django.urls import path
from channels.routing import ProtocolTypeRouter, URLRouter
from channels.auth import CookieMiddleware
from accounts.token_auth import TokenAuthMiddlewareStack
from .consumers import AsyncClientConnectionsConsumer, AsyncManagerConnectionsConsumer


os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'ETWeb.settings')

channel_routing = ProtocolTypeRouter({
    "websocket": CookieMiddleware(
            TokenAuthMiddlewareStack(
                URLRouter([
                    path("client/", AsyncClientConnectionsConsumer),
                    path("master/", AsyncManagerConnectionsConsumer)
                ])
            )
        )
})

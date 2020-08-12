import os
from django.urls import path
from channels.routing import ProtocolTypeRouter, URLRouter
from accounts.authentication import TokenAuthMiddlewareStack
from .consumers import AsyncClientConnectionsConsumer, AsyncManagerConnectionsConsumer


os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'ETWeb.settings')

channel_routing = ProtocolTypeRouter({
    "websocket": TokenAuthMiddlewareStack(
            URLRouter([
                path("client/", AsyncClientConnectionsConsumer),
                path("master/", AsyncManagerConnectionsConsumer)
            ])
        )
})

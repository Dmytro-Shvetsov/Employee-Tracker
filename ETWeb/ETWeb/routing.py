from django.urls import path
from channels.routing import ProtocolTypeRouter, URLRouter
from accounts.authentication import TokenAuthMiddlewareStack
from .consumers import AsyncClientConnectionsConsumer, AsyncManagerConnectionsConsumer


channel_routing = ProtocolTypeRouter({
    "websocket": TokenAuthMiddlewareStack(
            URLRouter([
                path("client/", AsyncClientConnectionsConsumer),
                path("master/", AsyncManagerConnectionsConsumer)
            ])
        )
})

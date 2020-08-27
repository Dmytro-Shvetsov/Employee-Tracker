from django.urls import path
from channels.routing import ProtocolTypeRouter, URLRouter
from accounts.authentication import TokenAuthMiddlewareStack
from .consumers import AsyncClientConnectionsConsumer, AsyncManagerConnectionsConsumer

# default websocket routing
channel_routing = ProtocolTypeRouter({
    "websocket": TokenAuthMiddlewareStack(
            URLRouter([
                # consumer for employee users
                path("client/", AsyncClientConnectionsConsumer),
                # consumer for manager users
                path("master/", AsyncManagerConnectionsConsumer)
            ])
        )
})

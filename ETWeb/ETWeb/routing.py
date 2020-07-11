import os
from django.urls import path
from channels.routing import ProtocolTypeRouter, URLRouter
from authorization.token_auth import TokenAuthMiddlewareStack
from .consumers import AsyncClientConnectionsConsumer


os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'ETWeb.settings')

channel_routing = ProtocolTypeRouter({
    "websocket": TokenAuthMiddlewareStack(
        URLRouter([
            path("client/", AsyncClientConnectionsConsumer)
        ])
    )
})

# application = ProtocolTypeRouter({
#     # Empty for now (http->django views is added by default)
#
#     "websocket": ([
#             url(r"^client/connect$", AsyncClientConnectionsConsumer),
#         ])
#
#
#     # "websocket": AuthMiddlewareStack(
#     #     URLRouter([
#     #         url(r"^chat/admin/$", AdminChatConsumer),
#     #         url(r"^chat/$", PublicChatConsumer),
#     #     ])
#     # ),
# })
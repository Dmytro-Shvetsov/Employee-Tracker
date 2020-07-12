import asyncio
from channels.generic.websocket import AsyncJsonWebsocketConsumer
import channels.exceptions


class AsyncClientConnectionsConsumer(AsyncJsonWebsocketConsumer):
    groups = ["broadcast",  "clients"]

    async def connect(self):
        print(self.scope["user"])
        if not self.scope['user'].is_authenticated:
            raise channels.exceptions.DenyConnection

        # Accepting the connection call:
        await self.accept()

        # self.channel_layer.group_add("clients", self.channel_name)
        print(self.scope.keys())
        # print(self.scope['headers'])
        print(self.scope['type'])

        await self.send_json({
            "type": "websocket.accept",
            "text": "sup bruv"
        })
    #     await self.close()

    async def receive_json(self, content, **kwargs):
        print(content)
        print(11111111111111111111111111111111111111111111)
        # You can call:
        await self.send_json({
            'text': 'ya good?'
        })
        # Want to force-close the connection? Call:
        error = False
        if error:
            await self.close()
            # Or add a custom WebSocket error code!
            # await self.close(code=4123)

    async def disconnect(self, close_code):
        pass
    #     # Called when the socket closes
    #     self.channel_layer.group_discard("clients", self.channel_name)


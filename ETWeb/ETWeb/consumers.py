from channels.generic.websocket import AsyncWebsocketConsumer
import json


class AsyncClientConnectionsConsumer(AsyncWebsocketConsumer):
    # groups = ["broadcast"]
    #
    # async def websocket_connect(self, event):
    #     print(event)
    #     await self.send({
    #         "type": "websocket.accept"
    #     })
    #
    # async def websocket_receive(self, event):
    #     print(event)
    #     await self.send({
    #         "type": "websocket.send",
    #         "text": event["text"]
    #     })

    groups = ["clients"]

    async def connect(self):
        # Called on connection.
        print(self.scope["user"])
        user_authenticated = True
        if user_authenticated:
            # Accepting the connection call:
            await self.accept()

            self.channel_layer.group_add("clients", self.channel_name)

            response = {
                "type": "websocket.accept",
                "text": "sup bruv"
            }
            await self.send(json.dumps(response))
        else:
            # To reject the connection, call:
            await self.close()

    async def receive(self, text_data=None, bytes_data=None):
        # Called with either text_data or bytes_data for each frame
        data = json.loads(text_data)
        print(data)
        # You can call:
        await self.send(text_data="Hello world!")
        # Or, to send a binary frame:
        await self.send(bytes_data="Hello world!")
        # Want to force-close the connection? Call:
        error = False
        if error:
            await self.close()
            # Or add a custom WebSocket error code!
            # await self.close(code=4123)

    async def disconnect(self, close_code):
        # Called when the socket closes
        self.channel_layer.group_discard("clients", self.channel_name)


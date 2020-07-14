import asyncio
import base64
import channels.exceptions
from channels.generic.websocket import AsyncJsonWebsocketConsumer
from channels.db import database_sync_to_async
from io import BytesIO
# from PIL import Image
from django.core.files.base import ContentFile
from employees.models import Screenshot
from string import ascii_letters
from random import choice


class AsyncClientConnectionsConsumer(AsyncJsonWebsocketConsumer):
    groups = ["broadcast",  "clients"]

    async def connect(self):
        print('New connection')
        self.user = await self.scope['user']

        if not self.user.is_authenticated:
            raise channels.exceptions.DenyConnection

        print('Connection accepted')

        # Accepting the connection call:
        await self.accept()

        # self.channel_layer.group_add("clients", self.channel_name)
        # print(self.scope['headers'])

        await self.send_json({
            "type": "websocket.accept",
            "text": "sup bruv"
        })
    #     await self.close()

    # async def receive(self, text_data=None, bytes_data=None, **kwargs):
    #     print('received default msg');
    #     print(text_data)
    #     print(len(bytes_data))
    #     im = Image.open(io.BytesIO(bytes_data))
    #     im.save(os.path.join(BASE_DIR, 'shotty.jpg'))

    async def receive_json(self, content, **kwargs):
        print('New message')
        print(content.keys())
        # You can call:
        if content['type'] == 'data.screenshot':
            await self.save_screenshot(content['screenshot'].encode())

        await self.send_json({
            'type': 'websocket.message',
            'text': 'message received'
        })
        # Want to force-close the connection? Call:
        error = False
        if error:
            await self.close()
            # Or add a custom WebSocket error code!
            # await self.close(code=4123)

    async def disconnect(self, close_code):
        # Called when the socket closes
        print('Disconnected')
    #     self.channel_layer.group_discard("clients", self.channel_name)

    @staticmethod
    def get_random_string(length):
        return ''.join(choice(ascii_letters) for _ in range(length))

    @database_sync_to_async
    def save_screenshot(self, encodedImage):
        new_screenshot = Screenshot()
        new_screenshot.employee = self.user
        new_screenshot.image.save(f'{self.user}_screenshot_{self.get_random_string(10)}.jpg',
                                  ContentFile(BytesIO(base64.decodebytes(encodedImage)).read()))
        new_screenshot.save()


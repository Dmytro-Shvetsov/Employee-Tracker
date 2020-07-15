import asyncio
import base64
from channels.generic.websocket import AsyncJsonWebsocketConsumer
from channels.db import database_sync_to_async
from io import BytesIO
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
            await self.close(401)
            return

        await self.accept()
        print('Connection accepted')

        # self.channel_layer.group_add("clients", self.channel_name)

        user_info = await database_sync_to_async(self.user.json)()
        await self.send_json({
            "type": "websocket.accept",
            **user_info
        })

    async def receive_json(self, content, **kwargs):
        print('New message')
        print(content.keys())

        if not self.user.is_authenticated:
            await self.close(401)
            return

        if content['type'] == 'data.screenshot':
            await self.save_screenshot(content['screenshot'].encode())

        await self.send_json({
            'type': 'websocket.message',
            'text': 'message received'
        })

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


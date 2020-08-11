import asyncio
import base64
from channels.generic.websocket import AsyncJsonWebsocketConsumer
from channels.db import database_sync_to_async
from asgiref.sync import async_to_sync
from io import BytesIO
from django.core.files.base import ContentFile
from accounts.api.serializers import WebsocketUserSerializer
from employees.models import ScreenshotActivity, NetworkActivity, ActivityInfo
from string import ascii_letters
from random import choice


class AsyncClientConnectionsConsumer(AsyncJsonWebsocketConsumer):
    groups = ['broadcast']
    user = None

    async def connect(self):
        print('New connection')
        self.user = await self.scope['user']
        await self.accept()

        if not self.user or not self.user.is_authenticated:
            print(f'Denied connection from {self.user}')
            await self.send_json({
                'type': 'websocket.close',
                'error': 'User does not exist or account has not been activated.'
            })
            await self.close(3401)
            return

        print('Connection accepted')

        await self.add_user_to_groups()

        print('Added user to project groups')

        serializer = WebsocketUserSerializer(self.user)
        user_info = await database_sync_to_async(serializer.json)()
        await self.send_json({
            "type": "websocket.accept",
            **user_info
        })

    async def receive_json(self, content, **kwargs):
        print('New message')
        print(content.keys())

        if not self.user.is_authenticated:
            await self.close(3401)
            return

        if content['type'] == 'data.screenshot':
            await self.save_screenshot(content['screenshot'].encode())
        elif content['type'] == 'data.network':
            await self.save_network_activities(content)

        await self.send_json({
            'type': 'websocket.message',
            'text': 'message received'
        })

    async def project_employees_status(self, event):
        print(f'Getting status of user {self.user} in project {event["project_id"]}')

    async def disconnect(self, close_code):
        # Called when the socket closes
        try:
            await self.remove_user_from_groups()
        except:
            pass

        print('Removed user from groups\nDisconnected')
        # self.channel_layer.group_discard("clients", self.channel_name)

    @database_sync_to_async
    def add_user_to_groups(self):
        if not self.user:
            raise ValueError('User instance is not set')

        user_id = str(self.user.id)

        # add user to broadcast group
        async_to_sync(self.channel_layer.group_add)('broadcast', user_id)

        # add user to project groups, so he/she could have online status
        for project in self.user.projects:
            async_to_sync(self.channel_layer.group_add)(f'project_{project.id}', user_id)

    @database_sync_to_async
    def remove_user_from_groups(self):
        if not self.user:
            raise ValueError('User instance is not set')

        user_id = str(self.user.id)

        # remove user from broadcast group
        async_to_sync(self.channel_layer.group_discard)('broadcast', user_id)

        # remove user from project groups, so he/she could have offline status
        for project in self.user.projects:
            async_to_sync(self.channel_layer.group_discard)(f'project_{project.id}', user_id)

    @staticmethod
    def get_random_string(length):
        return ''.join(choice(ascii_letters) for _ in range(length))

    @database_sync_to_async
    def save_screenshot(self, encoded_image):
        new_screenshot = ScreenshotActivity()
        new_screenshot.employee = self.user
        new_screenshot.image.save(f'{self.user}_screenshot_{self.get_random_string(10)}.jpg',
                                  ContentFile(BytesIO(base64.decodebytes(encoded_image)).read()))
        new_screenshot.save()

    async def save_network_activities(self, data):
        unknown_hosts = data['other']['hostnames']
        resolved_hosts = []
        for host_count_dict in unknown_hosts:
            host, count = next(iter(host_count_dict.items()))
            resolved_hostname = await NetworkActivity.resolve_ipv4_host(host)
            if not resolved_hostname:
                continue
            resolved_hosts.append({resolved_hostname: count})

        print('Resolved unknown hosts')
        data['other']['hostnames'] = resolved_hosts
        await self._save_network_data(data)

    @database_sync_to_async
    def _save_network_data(self, data):
        def create_network_objs(employee, stats, protocol, res):
            for host_count_dict in stats['hostnames']:
                host, count = next(iter(host_count_dict.items()))
                res.append(
                    NetworkActivity(host_name=host, message_count=count, protocol_type=protocol,
                                    employee=employee)
                )
        network_objs = []
        create_network_objs(self.user, data['http']['request_stats'], NetworkActivity.HTTP, network_objs)
        create_network_objs(self.user, data['ssl']['client_hello_stats'], NetworkActivity.SSL, network_objs)
        print(data['other'].keys())
        create_network_objs(self.user, data['other'], None, network_objs)
        NetworkActivity.objects.bulk_create(network_objs)
        print('Saved network objects')
        print('LEN {}'.format(len(network_objs)))


class AsyncManagerConnectionsConsumer(AsyncJsonWebsocketConsumer):
    user = None

    async def connect(self):
        print('New connection')
        self.user = await self.scope['user']
        await self.accept()

        if not self.user or not self.user.is_authenticated:
            print(f'Denied connection from {self.user}')
            await self.send_json({
                'type': 'websocket.close',
                'error': 'User does not exist or account has not been activated.'
            })
            await self.close(3401)
            return
        elif not self.user.is_staff:
            print(f'Denied connection from {self.user}')
            await self.send_json({
                'type': 'websocket.close',
                'error': 'You don\'t have permission to access this resource.'
            })
            await self.close(3403)
            return

        await self.send_json({
            "type": "websocket.accept",
        })

    async def receive_json(self, content, **kwargs):
        print('New message')
        print(content.keys())

        if not self.user.is_authenticated:
            await self.close(3401)
            return

        await self.send_json({
            'type': 'websocket.message',
            'text': 'message received'
        })

    async def disconnect(self, close_code):
        # Called when the socket closes
        print('Disconnected')
        # self.channel_layer.group_discard("clients", self.channel_name)



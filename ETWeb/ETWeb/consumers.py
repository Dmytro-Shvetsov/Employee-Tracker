import asyncio
import base64
from channels.generic.websocket import AsyncJsonWebsocketConsumer
from channels.db import database_sync_to_async
from asgiref.sync import async_to_sync
from io import BytesIO
from django.core.files.base import ContentFile
from accounts.api.serializers import WebsocketUserSerializer
from employees.models import ScreenshotActivity, NetworkActivity
from string import ascii_letters
from random import choice


class AsyncUserConnectionsConsumer(AsyncJsonWebsocketConsumer):
    groups = ['broadcast']
    user = None

    @database_sync_to_async
    def add_user_to_groups(self):
        raise NotImplementedError('add_user_to_groups() method should be implemented in descendant classes')

    @database_sync_to_async
    def remove_user_from_groups(self):
        raise NotImplementedError('remove_user_from_groups() method should be implemented in descendant classes')

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
            return False

        print('Connection accepted')
        return True

    async def disconnect(self, close_code):
        # Called when the socket closes
        await self.remove_user_from_groups()

        print('Removed user from groups\nDisconnected')


class DataCollectionMixin:
    user = None

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

    @database_sync_to_async
    def save_network_activities(self, data):
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

        NetworkActivity.objects.bulk_create(network_objs)
        print('Saved network objects')
        print('Total length {}'.format(len(network_objs)))


class AsyncClientConnectionsConsumer(AsyncUserConnectionsConsumer, DataCollectionMixin):
    STATUS_VALUES = ['online', 'offline', 'idle']
    status = 'offline'
    status_change_subscribers = set()

    async def user_status_report(self, event):
        print('Reporting status to ', event['report_to'])
        await self.channel_layer.group_send(event['report_to'], {
            'type': 'employee.status',
            'user_id': self.user.id,
            'status': self.status,
        })

    @database_sync_to_async
    def set_status(self, value):
        if value not in self.STATUS_VALUES:
            raise ValueError(f'invalid value for parameter value {repr(value)}')

        self.status = value
        for to in self.status_change_subscribers:
            async_to_sync(self.user_status_report)({'report_to': to})

        # for project in self.user.projects:
        #     async_to_sync(self.user_status_report)({
        #         'user_id': self.user.id,
        #         'status': self.status
        #     })

    @database_sync_to_async
    def set_status_change_subscribers(self):
        if not self.user:
            raise ValueError('User instance is not set')

        for project in self.user.project_set.prefetch_related():
            staff = project.members.filter(is_staff=True)
            self.status_change_subscribers.update([f'client_user_{u.id}' for u in staff])
        print(self.status_change_subscribers)

    @database_sync_to_async
    def add_user_to_groups(self):
        if not self.user:
            raise ValueError('User instance is not set')

        # add user to broadcast group
        async_to_sync(self.channel_layer.group_add)('broadcast', self.channel_name)

        # send message to close all living connections, as the new one is created
        async_to_sync(self.channel_layer.group_send)(f'client_user_{self.user.id}', {'type': 'disconnect'})

        # create user and project groups, so he/she could have online status
        async_to_sync(self.channel_layer.group_add)(f'client_user_{self.user.id}', self.channel_name)

        # for project in self.user.projects:
        #     async_to_sync(self.channel_layer.group_add)(f'project_{project.id}', self.channel_name)

    @database_sync_to_async
    def remove_user_from_groups(self):
        if not self.user:
            raise ValueError('User instance is not set')

        # remove user from broadcast group
        async_to_sync(self.channel_layer.group_discard)('broadcast', self.channel_name)

        # remove user from groups, so he/she could have offline status
        async_to_sync(self.channel_layer.group_discard)(f'client_user_{self.user.id}', self.channel_name)

        # for project in self.user.projects:
        #     async_to_sync(self.channel_layer.group_discard)(f'project_{project.id}', self.channel_name)

    async def connect(self):
        connected = await super(AsyncClientConnectionsConsumer, self).connect()
        print(connected, self.user)
        if not connected:
            return

        if self.user.is_staff:
            print(f'Denied connection from {self.user}')
            await self.send_json({
                'type': 'websocket.close',
                'error': 'Manager accounts cannot connect via client.'
            })
            await self.close(3403)
            return False

        await self.add_user_to_groups()
        print('Added user to project groups')

        await self.set_status_change_subscribers()

        serializer = WebsocketUserSerializer(self.user)
        user_info = await database_sync_to_async(serializer.json)()
        await self.send_json({
            "type": "websocket.accept",
            **user_info
        })

    async def receive_json(self, content, **kwargs):
        print('New message')
        print(content.keys())

        msg_type = content['type']
        if msg_type == 'data.screenshot':
            await self.save_screenshot(content['screenshot'].encode())
        elif msg_type == 'data.network':
            await self.save_network_activities(content)
        elif msg_type == 'user.status.change':
            try:
                await self.set_status(content.get('status', None))
            except ValueError:
                await self.send_json({
                    'type': 'websocket.message',
                    'text': 'invalid status sent'
                })

        await self.send_json({
            'type': 'websocket.message',
            'text': 'message received',
        })

    async def disconnect(self, close_code):
        try:
            await super().disconnect(close_code)
            await self.set_status('offline')
        except:
            pass


class AsyncManagerConnectionsConsumer(AsyncUserConnectionsConsumer):
    status = None

    @database_sync_to_async
    def add_user_to_groups(self):
        if not self.user:
            raise ValueError('User instance is not set')
        # add user to broadcast group
        async_to_sync(self.channel_layer.group_add)('broadcast', self.channel_name)

        # create user group
        async_to_sync(self.channel_layer.group_add)(f'client_user_{self.user.id}', self.channel_name)

    @database_sync_to_async
    def remove_user_from_groups(self):
        if not self.user:
            raise ValueError('User instance is not set')

        # remove user from broadcast group
        async_to_sync(self.channel_layer.group_discard)('broadcast', self.channel_name)

        async_to_sync(self.channel_layer.group_discard)(f'client_user_{self.user.id}', self.channel_name)

    async def connect(self):
        connected = await super().connect()
        if not connected:
            return

        if not self.user.is_staff:
            print(f'Denied connection from {self.user}')
            await self.send_json({
                'type': 'websocket.close',
                'error': 'You don\'t have permission to access this resource.'
            })
            await self.close(3403)
            return

        self.status = 'online'

        await self.add_user_to_groups()

        await self.send_json({
            "type": "websocket.accept",
        })

    async def receive_json(self, content, **kwargs):
        print('New message. ', content['type'])
        print(content.keys())

        msg_type = content['type']
        if msg_type == 'employee.ping':
            await self.ping_employee(content.get('user_id', -1))

        await self.send_json({
            'type': 'websocket.message',
            'text': 'message received'
        })

    async def ping_employee(self, id):
        print(f'Sending message to user {id}')
        await self.channel_layer.group_send(f'client_user_{id}', {
            'type': 'user.status.report',
            'report_to': f'client_user_{self.user.id}',
        })

    async def user_status_report(self, event):
        await self.send_json({
            'type': 'employee.status',
            'status': 'online',
            'user_id': self.user.id
        })

    async def employee_status(self, event):
        """
            Function that handles messages of type employee.status sent from AsyncClientConnectionsConsumer instances.
            Obtained information is sent back to frontend to update the user's current work status.
        """
        print(event)
        await self.send_json(event)

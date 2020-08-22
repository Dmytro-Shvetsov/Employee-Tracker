import base64
from rest_framework import serializers, exceptions
from django.contrib.auth.views import get_user_model
from django.utils.http import urlsafe_base64_decode
from django.utils.encoding import force_text
from django.utils import timezone
from employees.models import ScreenshotActivity, NetworkActivity
from accounts.models import UserProfile
from .tokens import account_activation_token
from . import validators


User = get_user_model()


class WebsocketUserSerializer:
    def __init__(self, user_obj):
        self.user_obj = user_obj

    @staticmethod
    def encode_image_field(img_field, encoding='base64'):
        if encoding == 'base64':
            return base64.encodebytes(img_field.read()).decode()

    def json(self):
        return {
            'username': str(self.user_obj.username),
            'date_joined': str(self.user_obj.date_joined.date()),
            'profile_image': self.encode_image_field(self.user_obj.profile.image)
        }


class HttpUserSerializer(serializers.ModelSerializer):
    class Meta:
        model = User
        fields = ['id', 'username', 'email', 'is_staff', 'date_joined']


class UserProfileSerializer(serializers.ModelSerializer):
    full_name = serializers.SerializerMethodField('get_full_name')
    work_place = serializers.SerializerMethodField('get_work_place')

    @staticmethod
    def get_full_name(profile):
        return profile.full_name

    @staticmethod
    def get_work_place(profile):
        return profile.work_place

    class Meta:
        model = UserProfile
        exclude = ['id', 'user']


class RegisterSerializer(serializers.ModelSerializer):
    password2 = serializers.CharField(
        style={'input_type': 'password'},
        write_only=True
    )

    def validate(self, attrs):
        try:
            # print(attrs)
            username = attrs['username']
            email = attrs['email']
            password = attrs['password']
            password2 = attrs['password2']

            if (validators.is_username_valid(username) and validators.is_email_valid(email) and
                    validators.is_password_valid(password, password2)):
                return attrs
            return attrs
        except KeyError as ex:
            missing_key = str(ex)
            raise serializers.ValidationError({missing_key: 'This field is required'})

    def save(self):
        return User.objects.create_user(
            username=self.validated_data['username'],
            email=self.validated_data['email'],
            password=self.validated_data['password'],
            is_staff=self.validated_data['is_staff'],
            is_active=False
        )

    class Meta:
        model = User
        fields = ['username', 'email', 'password', 'password2', 'is_staff']
        extra_kwargs = {
            'password': {
                'write_only': True
             }
        }


class AccountConfirmationSerializer(serializers.Serializer):
    uidb64 = serializers.CharField(max_length=2083, required=True)
    token = serializers.CharField(max_length=2083, required=True)

    def validate(self, attrs):
        try:
            uid = force_text(urlsafe_base64_decode(attrs['uidb64']))
            user = User.objects.get(pk=uid)
        except(TypeError, ValueError, OverflowError, User.DoesNotExist):
            user = None

        if not user or not account_activation_token.check_token(user, attrs['token']):
            raise serializers.ValidationError('Invalid confirmation data provided.')

        self.context['user'] = user
        return attrs

    def save(self, **kwargs):
        user = self.context['user']
        user.is_active = True
        user.save(force_update=True)
        user.send_account_activated()
        return user


class UserAccountUpdateSerializer(serializers.ModelSerializer):
    def validate(self, attrs):
        email = attrs['email']
        validators.is_email_valid(email)
        return attrs

    def save(self, **kwargs):
        if self.instance:
            self.instance.email = self.validated_data['email']
            self.instance.save()
        return self.instance

    class Meta:
        model = User
        fields = ['email']


class SearchUserSerializer(serializers.ModelSerializer):
    class Meta:
        model = User
        fields = ['id', 'username']


class ScreenshotActivitySerializer(serializers.ModelSerializer):
    class Meta:
        model = ScreenshotActivity
        exclude = ['employee']


class NetworkActivitySerializer(serializers.ModelSerializer):
    HTTP = NetworkActivity.HTTP
    SSL = NetworkActivity.SSL

    class Meta:
        model = NetworkActivity
        fields = ['host_name', 'message_count']


class ActivityLogsSerializer(serializers.Serializer):
    employee_id = serializers.IntegerField(min_value=0, required=True)
    since = serializers.DateTimeField(required=True)

    def validate_employee_id(self, value):
        try:
            self.context['employee'] = User.objects.get(id=value)
        except User.DoesNotExist:
            raise exceptions.ValidationError('Unexpected error occurred when retrieving employee instance.')

        return value

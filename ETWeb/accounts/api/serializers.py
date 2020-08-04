import base64
from rest_framework import serializers
from django.contrib.auth.views import get_user_model
from accounts.models import UserProfile
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
    class Meta:
        model = UserProfile
        fields = '__all__'


class RegisterSerializer(serializers.ModelSerializer):
    password2 = serializers.CharField(
        style={'input_type': 'password'},
        write_only=True
    )

    def validate(self, attrs):
        try:
            print(attrs)
            username = attrs['username']
            email = attrs['email']
            password = attrs['password']
            password2 = attrs['password2']

            # if (validators.is_username_valid(username) and validators.is_email_valid(email) and
            #         validators.is_password_valid(password, password2)):
            #     return attrs
            return attrs
        except KeyError as ex:
            missing_key = str(ex)
            raise serializers.ValidationError({missing_key: 'This field is required'})

    def save(self):
        return User.objects.create_user(
            username=self.validated_data['username'],
            email=self.validated_data['email'],
            password=self.validated_data['password'],
            is_staff=self.validated_data['is_staff']
        )

    class Meta:
        model = User
        fields = ['username', 'email', 'password', 'password2', 'is_staff']
        extra_kwargs = {
            'password': {
                'write_only': True
             }
        }

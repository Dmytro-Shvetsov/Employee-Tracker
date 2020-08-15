import os
from rest_framework.authtoken.views import ObtainAuthToken, APIView
from rest_framework import status, permissions
from rest_framework.response import Response
from rest_framework.authtoken.models import Token
from rest_framework.renderers import JSONRenderer
from django.conf import settings

from django.contrib.sites.shortcuts import get_current_site
from django.utils.encoding import force_bytes, force_text
from django.utils.http import urlsafe_base64_encode, urlsafe_base64_decode

from django.template.loader import render_to_string

from django.core.mail import EmailMessage
from .tokens import account_activation_token
from .serializers import (HttpUserSerializer,
                          UserProfileSerializer,
                          RegisterSerializer,
                          UserAccountUpdateSerializer,
                          AccountConfirmationSerializer)
from django.contrib.auth import get_user_model


User = get_user_model()


def _get_auth_token(user):
    token, _ = Token.objects.get_or_create(user=user)
    return token.key


def _set_signed_cookie(response, *, key, value, httponly=False, max_age=86400):
    if not isinstance(response, Response):
        raise ValueError('response parameter should be an instance of rest_framework.response.Response class')

    response.set_signed_cookie(key=key,
                               value=value,
                               salt=settings.SIGNED_COOKIE_SALT,
                               httponly=httponly,
                               secure=settings.USE_HTTPS, # send this cookie only if request is made with https scheme
                               max_age=max_age,
                               samesite='Strict')  # do not send this cookie when performing cross-origin request


class LoginView(ObtainAuthToken):
    authentication_classes = ()
    permission_classes = (permissions.AllowAny,)

    """
        Try authorize with given credentials
    """
    def post(self, request, *args, **kwargs):
        print(11)
        serializer = self.serializer_class(data=request.data,
                                           context={'request': request})
        serializer.is_valid(raise_exception=False)
        if serializer.errors:
            print(serializer.errors)
            return Response(serializer.errors, status.HTTP_400_BAD_REQUEST)

        user = serializer.validated_data['user']
        token_key = _get_auth_token(user)
        response = {
            'token': token_key,
        }
        if 'include_acc_info' in request.data and request.data['include_acc_info']:
            response.update(
                HttpUserSerializer(user, context={'request': request}).data
            )
        response = Response(response, status.HTTP_202_ACCEPTED)
        _set_signed_cookie(response, key=settings.AUTH_TOKEN_KEY, value=token_key, httponly=True)
        return response


class LogoutView(APIView):
    permission_classes = (permissions.AllowAny,)

    def post(self, request):
        response = Response(status=status.HTTP_200_OK)
        _set_signed_cookie(response, key=settings.AUTH_TOKEN_KEY, value=None, httponly=True, max_age=0)
        return response


class RegisterView(APIView):
    serializer_class = RegisterSerializer
    permission_classes = (permissions.AllowAny, )
    authentication_classes = ()

    def mail_confirmation_link(self, user):
        mail_subject = 'Activate your blog account.'

        link = '{protocol}://{domain}/activate/{uid}/{token}'.format(
            protocol='https' if settings.USE_HTTPS else 'http',
            domain=get_current_site(self.request).domain,
            uid=urlsafe_base64_encode(force_bytes(user.pk)),
            token=account_activation_token.make_token(user)
        )

        message = render_to_string('accounts/account_active_email.html', {
            'user': user,
            'confirm_link': link,
        })
        EmailMessage(mail_subject, message, to=[user.email]).send()
        return Response({
            'detail': 'Please confirm your email address to complete the registration'
        }, status.HTTP_201_CREATED)

    """
        Create new user
    """
    def post(self, request):
        serializer = self.serializer_class(data=request.data,
                                           context={'request': request})

        serializer.is_valid(raise_exception=False)
        user_instance = serializer.validated_data.get('user', None)
        if user_instance:
            return Response({'error': 'You already have an account.'}, status.HTTP_400_BAD_REQUEST)

        if serializer.errors:
            return Response(serializer.errors, status.HTTP_400_BAD_REQUEST)

        user_instance = serializer.save()
        # 'token': _get_auth_token(user_instance)

        return self.mail_confirmation_link(user_instance)


class AccountConfirmationView(APIView):
    permission_classes = (permissions.AllowAny,)
    authentication_classes = ()
    serializer_class = AccountConfirmationSerializer

    def post(self, request):
        serializer = self.serializer_class(data=request.data)
        serializer.is_valid(raise_exception=True)
        serializer.save()

        return Response({
            'detail': 'Thank you for your email confirmation. Now you can login your account.',
        }, status.HTTP_201_CREATED)


class AccountView(APIView):
    permission_classes = (permissions.IsAuthenticated,)

    """
        Retrieve account information
    """
    def post(self, request):
        serializer = HttpUserSerializer(request.user)
        response = serializer.data
        response['token'] = _get_auth_token(request.user)
        return Response(JSONRenderer().render(response),
                        status=status.HTTP_200_OK)

    """
        Update account information
    """
    def put(self, request):
        serializer = UserAccountUpdateSerializer(instance=request.user, data=request.data)
        serializer.is_valid(raise_exception=True)
        serializer.save()
        return Response(JSONRenderer().render(serializer.data),
                        status=status.HTTP_202_ACCEPTED)


class ProfileView(APIView):
    permission_classes = (permissions.IsAuthenticated,)
    serializer_class = UserProfileSerializer

    """
        Retrieve profile information
    """
    def post(self, request):
        serializer = self.serializer_class(request.user.profile)
        return Response(JSONRenderer().render(serializer.data),
                        status=status.HTTP_200_OK)

    """
        Update user profile information
    """
    def put(self, request):
        profile = request.user.profile
        prev_image = profile.image
        serializer = self.serializer_class(instance=profile, data=request.data)
        serializer.is_valid(raise_exception=True)
        if request.data.get('image', None):
            os.remove(os.path.join(settings.MEDIA_ROOT, prev_image.name))

        serializer.save()
        return Response(JSONRenderer().render(serializer.data),
                        status=status.HTTP_200_OK)


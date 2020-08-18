import os
from rest_framework.authtoken.views import ObtainAuthToken, APIView
from rest_framework import status, permissions
from rest_framework.response import Response
from rest_framework.authtoken.models import Token
from rest_framework.renderers import JSONRenderer
from django.conf import settings
from .serializers import (HttpUserSerializer,
                          UserProfileSerializer,
                          RegisterSerializer,
                          UserAccountUpdateSerializer,
                          AccountConfirmationSerializer,
                          SearchUserSerializer)
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
        # if request is sent from the web application, include serialized user data to the response data
        if 'include_acc_info' in request.data and request.data['include_acc_info']:
            response.update(
                HttpUserSerializer(user, context={'request': request}).data
            )

        response = Response(response, status.HTTP_202_ACCEPTED)
        # persist authorization token in the http-only cookie
        remember = request.data.get('remember', False)
        age = settings.EXTENDED_AUTH_TOKEN_COOKIE_AGE if remember else settings.DEFAULT_AUTH_TOKEN_COOKIE_AGE
        _set_signed_cookie(response, key=settings.AUTH_TOKEN_KEY, value=token_key, httponly=True, max_age=age)
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
        pass

    """
        Create new user
    """
    def post(self, request):
        serializer = self.serializer_class(data=request.data,
                                           context={'request': request})

        serializer.is_valid(raise_exception=False)
        user_instance = serializer.validated_data.get('user', None)
        if user_instance and user_instance.is_authenticated:
            return Response({'error': 'You already have an account.'}, status.HTTP_400_BAD_REQUEST)

        if serializer.errors:
            return Response(serializer.errors, status.HTTP_400_BAD_REQUEST)

        serializer.save()
        # 'token': _get_auth_token(user_instance)
        return Response({
            'detail':
                'We have sent you an email to activate your account. '
                'Follow the steps in the email to finish the registration.'
        }, status.HTTP_201_CREATED)


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
        print(request.user.profile)
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


class SearchUsersView(APIView):
    serializer_class = SearchUserSerializer

    """
        Search users by username
    """
    def get(self, request, username, *args, **kwargs):
        # case insensitive search of any usernames that contain the pattern
        matches = User.objects.filter(username__iregex=username)
        serializer = self.serializer_class(matches, many=True)
        return Response(JSONRenderer().render({
            'users': serializer.data,
        }), status=status.HTTP_200_OK)


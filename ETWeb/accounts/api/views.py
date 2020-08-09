import os
from django.conf import settings
from rest_framework.authtoken.views import ObtainAuthToken, APIView
from rest_framework import status, permissions
from rest_framework.generics import RetrieveAPIView
from rest_framework.response import Response
from rest_framework.authtoken.models import Token
from rest_framework.renderers import JSONRenderer
from .serializers import (HttpUserSerializer,
                          UserProfileSerializer,
                          RegisterSerializer,
                          UserAccountUpdateSerializer)


def _get_auth_token(user):
    token, _ = Token.objects.get_or_create(user=user)
    return token.key


class LoginView(ObtainAuthToken):
    """
        Try authorize with given credentials
    """
    def post(self, request, *args, **kwargs):
        serializer = self.serializer_class(data=request.data,
                                           context={'request': request})
        serializer.is_valid(raise_exception=True)

        if serializer.errors:
            return Response(serializer.errors, status.HTTP_400_BAD_REQUEST)

        user = serializer.validated_data['user']
        response = {
            'token': _get_auth_token(user),
        }
        if 'include_acc_info' in request.data and request.data['include_acc_info']:
            response.update(
                HttpUserSerializer(user, context={'request': request}).data
            )
        print(response)
        return Response(response, status.HTTP_202_ACCEPTED)


class RegisterView(APIView):
    serializer_class = RegisterSerializer
    permission_classes = (permissions.AllowAny, )

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

        return Response(JSONRenderer().render({
            'token': _get_auth_token(user_instance)
        }), status=status.HTTP_201_CREATED)


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


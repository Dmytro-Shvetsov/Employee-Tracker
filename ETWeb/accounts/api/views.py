from rest_framework.authtoken.views import ObtainAuthToken, APIView
from rest_framework import status, permissions
from rest_framework.generics import RetrieveAPIView
from rest_framework.response import Response
from rest_framework.authtoken.models import Token
from rest_framework.renderers import JSONRenderer
from .serializers import (HttpUserSerializer, UserProfileSerializer, RegisterSerializer, UserAccountUpdateSerialize)


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
            return Response(serializer.errors, status.HTTP_401_UNAUTHORIZED)

        user = serializer.validated_data['user']
        response = {
            'token': _get_auth_token(user),
        }
        if 'include_acc_info' in request.data and request.data['include_acc_info']:
            response.update(
                HttpUserSerializer(user, context={'request': request}).data
            )
        return Response(response, status.HTTP_202_ACCEPTED)


class RegisterView(APIView):
    serializer_class = RegisterSerializer
    authentication_classes = ()
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
            return Response(serializer.errors, status.HTTP_401_UNAUTHORIZED)

        serializer.save()

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
        serializer = UserAccountUpdateSerialize(instance=request.user, data=request.data)
        serializer.is_valid(raise_exception=True)
        serializer.save()
        return Response(status=status.HTTP_202_ACCEPTED)


class ProfileView(APIView):
    permission_classes = (permissions.IsAuthenticated,)
    serializer_class = UserProfileSerializer

    """
        Retrieve account information
    """
    def post(self, request):
        serializer = self.serializer_class(request.user.profile)
        return Response(JSONRenderer().render(serializer.data),
                        status=status.HTTP_200_OK)

    """
        Update user profile information
    """
    def put(self, request):
        if not request.data:
            serializer = self.serializer_class(request.user.profile)
            return Response(JSONRenderer().render(serializer.data),
                            status=status.HTTP_200_OK)

        return Response(status=status.HTTP_400_BAD_REQUEST)

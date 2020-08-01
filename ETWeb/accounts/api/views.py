from rest_framework.authtoken.views import ObtainAuthToken, APIView
from rest_framework import status, permissions
from rest_framework.response import Response
from rest_framework.authtoken.models import Token
from rest_framework.renderers import JSONRenderer
from .serializers import (HttpUserSerializer, UserProfileSerializer, RegisterSerializer)


def _get_auth_token(user):
    token, _ = Token.objects.get_or_create(user=user)
    return token.key


class LoginView(ObtainAuthToken):
    def post(self, request, *args, **kwargs):
        serializer = self.serializer_class(data=request.data,
                                           context={'request': request})
        serializer.is_valid(raise_exception=False)
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

    def post(self, request):
        serializer = self.serializer_class(data=request.data,
                                           context={'request': request})

        serializer.is_valid(raise_exception=False)
        user_instance = serializer.validated_data.get('user', None)
        if user_instance:
            return Response({'error': 'You already have an account.'}, status.HTTP_400_BAD_REQUEST)

        if serializer.errors:
            return Response(serializer.errors, status.HTTP_401_UNAUTHORIZED)

        user_instance = serializer.save()

        token = Token.objects.get(user=user_instance)
        response = {
            'token': token.key
        }
        # response['user']['profile'] = UserProfileSerializer(user_instance.profile).data

        return Response(JSONRenderer().render(response), status=status.HTTP_201_CREATED)


class AccountView(APIView):
    def post(self, request):
        if not request.data:
            serializer = HttpUserSerializer(request.user,
                                            context={'request': request})
            response = serializer.data
            response['token'] = _get_auth_token(request.user)
            return Response(JSONRenderer().render(response), status=status.HTTP_200_OK)

        # otherwise try updating user info

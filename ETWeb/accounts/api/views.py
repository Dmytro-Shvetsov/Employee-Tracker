from rest_framework.authtoken.views import ObtainAuthToken, APIView
from rest_framework import status
from rest_framework import permissions
from rest_framework.response import Response
from rest_framework.authtoken.models import Token
from rest_framework.renderers import JSONRenderer
from .serializers import HttpUserSerializer, RegisterSerializer, UserProfileSerializer


class ObtainAuthToken(ObtainAuthToken):
    def post(self, request, *args, **kwargs):
        serializer = self.serializer_class(data=request.data,
                                           context={'request': request})
        serializer.is_valid(raise_exception=False)
        if serializer.errors:
            return Response({'errors': ' '.join(serializer.errors['non_field_errors'])})

        user = serializer.validated_data['user']
        token, created = Token.objects.get_or_create(user=user)
        return Response({'token': token.key})


class RegisterView(APIView):
    serializer_class = RegisterSerializer
    authentication_classes = ()
    permission_classes = (permissions.AllowAny, )

    def post(self, request):
        serializer = self.serializer_class(data=request.data)
        serializer.is_valid(raise_exception=False)
        if serializer.errors:
            return Response(serializer.errors, status.HTTP_401_UNAUTHORIZED)

        user_instance = serializer.save()

        response = {
            'user': serializer.data
        }
        response['user']['profile'] = UserProfileSerializer(user_instance.profile).data

        return Response(JSONRenderer().render(response), status=status.HTTP_201_CREATED)

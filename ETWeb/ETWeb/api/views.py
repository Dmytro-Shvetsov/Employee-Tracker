from rest_framework import status, permissions
from rest_framework.views import Response, APIView
from rest_framework.renderers import JSONRenderer
from .serializers import ContactFormSerializer


class JSONUpdateMixin:
    """
    Custom UpdateModelMixin for any model instance. Unlike rest_framework.mixins.UpdateModelMixin, this
    view returns an error if an instance is not found by the passed primary key. Otherwise, the view updates
    the found object and returns it in response.
    """
    serializer_class = None

    def get_queryset(self):
        raise NotImplementedError('Override the method to use it with your queryset.')

    def put(self, request, pk):
        model_name = self.serializer_class.Meta.model.__name__
        try:
            queryset = self.get_queryset()
            instance = queryset.get(id=pk)
            serializer = self.serializer_class(instance=instance, data=request.data,
                                               context={
                                                   'request': request,
                                                   f'{model_name.lower()}s': queryset
                                               })

            serializer.is_valid(raise_exception=True)
            serializer.save()
            return Response(JSONRenderer().render(serializer.data),
                            status=status.HTTP_202_ACCEPTED)
        except self.serializer_class.Meta.model.DoesNotExist:
            return Response({
                'detail': f'{model_name} with provided id was not found.'
            }, status.HTTP_400_BAD_REQUEST)


class ContactView(APIView):
    """
    Contact form api view that sends emails to settings.EMAIL_HOST_USER
    """
    permission_classes = (permissions.AllowAny,)
    authentication_classes = ()
    serializer_class = ContactFormSerializer

    def post(self, request):
        serializer = self.serializer_class(data=request.data)
        serializer.is_valid(raise_exception=True)
        serializer.save()
        return Response({
            'detail':
                'Thank you for your email. If you submitted a question, we will respond as quickly as '
                'possible, usually within 24 hours. If you submitted a suggestion or comment, thanks for '
                'the feedback!'
        })

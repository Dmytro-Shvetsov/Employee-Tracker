from rest_framework import status
from rest_framework.views import Response
from rest_framework.renderers import JSONRenderer


class JSONUpdateMixin:
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
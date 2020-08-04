from rest_framework import generics, permissions, status
from rest_framework.pagination import PageNumberPagination
from rest_framework.views import Response, APIView
from rest_framework.settings import api_settings
from rest_framework.renderers import JSONRenderer
from .serializers import GeneralProjectSerializer, DetailProjectSerializer
from collections import OrderedDict


class ProjectListView(APIView, PageNumberPagination):
    renderer_classes = (JSONRenderer, )

    def get_queryset(self):
        return self.request.user.project_set.prefetch_related().all()

    def get_paginated_response(self, data):
        return Response(OrderedDict([
            ('count', self.page.paginator.count),
            ('page_size', api_settings.PAGE_SIZE),
            ('results', data)
        ]))

    def get(self, request):
        projects = self.get_queryset()

        page = self.paginate_queryset(projects, request, view=self)
        serializer = GeneralProjectSerializer(page,
                                              many=True,
                                              context={'user': request.user})

        return self.get_paginated_response(JSONRenderer().render(serializer.data))

    def post(self, request):
        projects = self.get_queryset()
        serializer = DetailProjectSerializer(data=request.data,
                                             context={
                                                 'projects': projects,
                                                 'request': request
                                             })
        serializer.is_valid(raise_exception=True)
        new_project = serializer.save()
        return Response(JSONRenderer().render({
            'message': f'Project \'{new_project.name}\' was successfully created.',
            'id': new_project.id
        }), status=status.HTTP_201_CREATED)


class ProjectDetail(generics.RetrieveUpdateDestroyAPIView):
    serializer_class = DetailProjectSerializer
    renderer_classes = (JSONRenderer,)

    def get_queryset(self):
        return self.request.user.project_set.prefetch_related().all()

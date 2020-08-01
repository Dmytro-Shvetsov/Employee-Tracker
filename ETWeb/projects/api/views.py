from rest_framework import generics, permissions
from rest_framework.views import Response, APIView
from rest_framework import status
from rest_framework.renderers import JSONRenderer
from .serializers import GeneralProjectSerializer, DetailProjectSerializer
from projects.models import Project


class ProjectListView(APIView):
    renderer_classes = (JSONRenderer, )

    def get_queryset(self):
        return self.request.user.project_set.prefetch_related().all()

    def get(self, request):
        projects = self.get_queryset()
        serializer = GeneralProjectSerializer(projects, many=True)

        # page = self.paginate_queryset(queryset)
        # if page is not None:
        #
        #     return self.get_paginated_response(serializer.data)
        # serializer = self.get_serializer(queryset, many=True)
        return Response(JSONRenderer().render(serializer.data), status=status.HTTP_200_OK)

    def post(self, request):
        projects = self.get_queryset()
        serializer = DetailProjectSerializer(data=request.data,
                                             context={
                                                 'projects': projects,
                                                 'request': request
                                             })
        serializer.is_valid(raise_exception=True)
        serializer.save()
        return Response(status=status.HTTP_201_CREATED)


class ProjectDetail(generics.RetrieveUpdateDestroyAPIView):
    queryset = Project.objects.all()
    serializer_class = DetailProjectSerializer


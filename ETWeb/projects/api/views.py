from rest_framework import generics, permissions, status
from rest_framework.pagination import PageNumberPagination
from rest_framework.views import Response, APIView
from rest_framework.settings import api_settings
from rest_framework.renderers import JSONRenderer
from .serializers import (GeneralProjectSerializer, DetailProjectSerializer,
                          AddMembersSerializer, RemoveMembersSerializer, ProjectInvitationSerializer)
from projects.models import Project
from ETWeb.api.views import JSONUpdateMixin
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

    """
        Retrieve one page of all owned projects
    """
    def get(self, request):
        projects = self.get_queryset()

        page = self.paginate_queryset(projects, request, view=self)
        serializer = GeneralProjectSerializer(page,
                                              many=True,
                                              context={'user': request.user})

        return self.get_paginated_response(JSONRenderer().render(serializer.data))

    """
        Create new project
    """
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


class ProjectDetail(generics.RetrieveDestroyAPIView, JSONUpdateMixin):
    serializer_class = DetailProjectSerializer
    model_name = serializer_class.Meta.model.__name__

    """
    Retrieve a model instance.
    """
    def retrieve(self, request, *args, **kwargs):
        instance = self.get_object()
        serializer = self.get_serializer(instance)
        return Response(JSONRenderer().render(serializer.data))

    def get_queryset(self):
        return self.request.user.project_set.prefetch_related().all()


class ManageMembersView(APIView):
    serializer_class = DetailProjectSerializer

    def get_project(self, pk):
        try:
            return self.request.user.project_set.get(id=pk)
        except Project.DoesNotExist:
            return None

    def put(self, request, pk):
        project = self.get_project(pk)
        if not project:
            return Response(status=status.HTTP_400_BAD_REQUEST)

        serializer = AddMembersSerializer(instance=project, data=request.data,
                                          context={
                                              'request': request,
                                              'projects': request.user.project_set
                                          })
        serializer.is_valid(raise_exception=True)
        serializer.save()
        return Response(JSONRenderer().render({
            'detail': 'Invitations were successfully sent to new members.',
            'project': self.serializer_class(project).data
        }), status=status.HTTP_200_OK)

    def delete(self, request, pk):
        project = self.get_project(pk)
        if not project:
            return Response(status=status.HTTP_400_BAD_REQUEST)

        serializer = RemoveMembersSerializer(instance=project,
                                             data=request.data,
                                             context={
                                                 'projects': request.user.project_set,
                                                 'request': request
                                             })
        serializer.is_valid(raise_exception=True)
        serializer.save()
        return Response(JSONRenderer().render({
            'detail': 'Selected members were successfully deleted from the project.',
            'project': self.serializer_class(project).data
        }), status=status.HTTP_200_OK)


class AcceptProjectInvitationView(APIView):
    permission_classes = (permissions.IsAuthenticated,)
    serializer_class = ProjectInvitationSerializer

    def post(self, request, token):
        print(request.user)
        serializer = self.serializer_class(data={'token': token},
                                           context={'request': request})
        serializer.is_valid(raise_exception=True)
        invitation = serializer.save()

        return Response({
            'detail': f'Congratulations! Now you are a member of project \'{invitation.project.name}\'.'
        }, status.HTTP_200_OK)

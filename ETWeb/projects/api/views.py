from rest_framework import viewsets, permissions
from projects.models import Project
from .serializers import ProjectSerializer


class ProjectViewSet(viewsets.ModelViewSet):
    queryset = Project.objects.all()
    permission_classes = [
        permissions.IsAuthenticated
    ]
    serializer_class = ProjectSerializer

    def get_queryset(self):
        return self.request.user.project_set.all()


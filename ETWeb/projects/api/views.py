from rest_framework import generics, permissions
from .serializers import GeneralProjectSerializer, DetailProjectSerializer
from projects.models import Project


class ProjectListView(generics.ListAPIView):
    serializer_class = GeneralProjectSerializer

    def get_queryset(self):
        user_id = self.request.user.id
        # project_id = self.kwargs['id']
        # queryset  = Project.objects.raw(
        #     'SELECT t2.id, t2.name, t2.budget_usd '
        #     'FROM project_user t1 '
        #     'JOIN projects t2 ON t1.user_id = 3 AND t1.project_id = t2.id;'
        # )
        return self.request.user.project_set.all()


class ProjectDetail(generics.RetrieveUpdateDestroyAPIView):
    queryset = Project.objects.all()
    serializer_class = DetailProjectSerializer


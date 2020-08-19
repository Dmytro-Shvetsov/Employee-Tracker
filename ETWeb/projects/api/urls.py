from django.urls import path, re_path
from .views import ProjectListView, ProjectDetail, ManageMembersView
from rest_framework.urlpatterns import format_suffix_patterns

urlpatterns = [
    path('', ProjectListView.as_view(), name='projects'),
    path(r'<int:pk>/', ProjectDetail.as_view(), name='project_detail'),
    path(r'manage_members/<int:pk>/', ManageMembersView.as_view(), name='project_manage_members'),
]

urlpatterns = format_suffix_patterns(urlpatterns)

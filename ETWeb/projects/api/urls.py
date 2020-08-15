from django.urls import path, re_path
from .views import ProjectListView, ProjectDetail
from rest_framework.urlpatterns import format_suffix_patterns

urlpatterns = [
    path('', ProjectListView.as_view()),
    re_path(r'(?P<pk>\d+)/', ProjectDetail.as_view())
]

urlpatterns = format_suffix_patterns(urlpatterns)

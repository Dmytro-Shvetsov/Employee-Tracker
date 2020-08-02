from rest_framework import generics, permissions, status
from rest_framework.pagination import PageNumberPagination
from rest_framework.views import Response, APIView
# from rest_framework.settings import api_settings
from rest_framework.renderers import JSONRenderer
from .serializers import GeneralProjectSerializer, DetailProjectSerializer
from projects.models import Project


# class PaginationMixin:
#     pagination_class = api_settings.DEFAULT_PAGINATION_CLASS  # cool trick right? :)
#     #  taken from django-rest-framework/rest_framework/generics.py
#     @property
#     def paginator(self):
#         """
#         The paginator instance associated with the view, or `None`.
#         """
#         if not hasattr(self, '_paginator'):
#             if self.pagination_class is None:
#                 self._paginator = None
#         else:
#             self._paginator = self.pagination_class()
#         return self._paginator
#
#     def paginate_queryset(self, queryset):
#         """
#         Return a single page of results, or `None` if pagination is disabled.
#         """
#         if self.paginator is None:
#             return None
#         return self.paginator.paginate_queryset(queryset, self.request, view=self)
#
#     def get_paginated_response(self, data):
#         """
#         Return a paginated style `Response` object for the given output data.
#         """
#         assert self.paginator is not None
#         return self.paginator.get_paginated_response(data)
#

class ProjectListView(APIView, PageNumberPagination):
    renderer_classes = (JSONRenderer, )

    def get_queryset(self):
        return self.request.user.project_set.prefetch_related().all()

    def get(self, request):
        projects = self.get_queryset()
        page = self.paginate_queryset(projects, request, view=self)

        serializer = GeneralProjectSerializer(page, many=True)
        return self.get_paginated_response(JSONRenderer().render(serializer.data))

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


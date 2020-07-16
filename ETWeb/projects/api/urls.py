from rest_framework.routers import DefaultRouter
from django.urls import path
from .views import ProjectViewSet


router = DefaultRouter()
router.register('', ProjectViewSet)

urlpatterns = router.urls

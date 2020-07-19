from rest_framework.routers import DefaultRouter
from .views import ProjectViewSet


router = DefaultRouter()
router.register('', ProjectViewSet)

urlpatterns = router.urls

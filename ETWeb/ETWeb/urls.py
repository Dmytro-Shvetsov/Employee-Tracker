from django.urls import path, re_path, include
from django.conf import settings
from django.conf.urls.static import static
from .api.views import ContactView


urlpatterns = [
    # API
    path('api/projects/', include('projects.api.urls')),
    path('api/accounts/', include('accounts.api.urls')),
    path('api/contact/', ContactView.as_view()),
]

# if settings.DEBUG:
urlpatterns += static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)
urlpatterns += static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)

urlpatterns += [
    # Pages
    re_path(r'.*', include('frontend.urls')),
]

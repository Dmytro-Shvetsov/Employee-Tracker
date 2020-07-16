from django.contrib import admin
from django.urls import path, include
from django.conf import settings
from django.conf.urls.static import static


urlpatterns = [
    path('', include('frontend.urls')),

    path('api/projects/', include('projects.api.urls')),

    path('admin/', admin.site.urls),
    path('auth/', include('accounts.urls'), name='auth'),
]

if settings.DEBUG:
    urlpatterns += static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)
    urlpatterns += static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)

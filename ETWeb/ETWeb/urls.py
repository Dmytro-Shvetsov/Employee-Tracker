from django.contrib import admin
from django.urls import path, re_path, include
from django.conf import settings
from django.conf.urls.static import static


urlpatterns = [
    # API
    path('api/projects/', include('projects.api.urls')),
    path('api/auth/', include('accounts.api.urls')),

    path('admin/', admin.site.urls),
    # path('auth/', include('accounts.urls'), name='auth'),

    # Pages
    re_path(r'.*', include('frontend.urls')),
]

if settings.DEBUG:
    urlpatterns += static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)
    urlpatterns += static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)

from django.contrib import admin
from django.urls import path, include
from django.conf import settings
from django.conf.urls.static import static
from .views import ObtainAuthToken


urlpatterns = [
    path('login/', ObtainAuthToken.as_view(), name='login')
]
from django.urls import path
from .views import ObtainAuthToken, RegisterView


urlpatterns = [
    path('login/', ObtainAuthToken.as_view(), name='login'),
    path('register/', RegisterView.as_view(), name='register')
]

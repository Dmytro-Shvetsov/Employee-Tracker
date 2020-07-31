from django.urls import path
from .views import LoginView, RegisterView, AccountView


urlpatterns = [
    path('login/', LoginView.as_view(), name='login'),
    path('register/', RegisterView.as_view(), name='register'),
    path('account/', AccountView.as_view(), name='account'),
]

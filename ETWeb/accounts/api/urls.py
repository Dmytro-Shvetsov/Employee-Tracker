from django.urls import path
from .views import (
    LoginView,
    LogoutView,
    RegisterView,
    AccountConfirmationView,
    AccountView,
    ProfileView
)


urlpatterns = [
    path('login/', LoginView.as_view(), name='login'),
    path('logout/', LogoutView.as_view(), name='logout'),
    path('register/', RegisterView.as_view(), name='register'),
    path('activate/', AccountConfirmationView.as_view(), name='activate'),
    path('account/', AccountView.as_view(), name='account'),
    path('profile/', ProfileView.as_view(), name='profile'),
]

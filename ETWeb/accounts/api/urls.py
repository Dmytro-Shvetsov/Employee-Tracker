from django.urls import path, include
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
    # Endpoints provided
    # POST reset_password/ - request a reset password token by using the email parameter
    # POST reset_password/confirm/ - using a valid token, the users password is set to the provided password
    # POST reset_password/validate_token/ - will return a 200 if a given token is valid
    path('reset_password/', include('django_rest_passwordreset.urls')),
    path('account/', AccountView.as_view(), name='account'),
    path('profile/', ProfileView.as_view(), name='profile'),
]

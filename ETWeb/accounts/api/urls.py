from django.urls import path, include
from .views import (
    LoginView,
    LogoutView,
    RegisterView,
    AccountConfirmationView,
    AccountView,
    ProfileView,
    SearchUsersView,
    EmployeeActivityLogsView,
)


urlpatterns = [
    # Authentication/authorization endpoints
    path('auth/login/', LoginView.as_view(), name='login'),
    path('auth/logout/', LogoutView.as_view(), name='logout'),
    path('auth/register/', RegisterView.as_view(), name='register'),
    path('auth/activate/', AccountConfirmationView.as_view(), name='activate'),
    # Endpoints provided
    # POST reset_password/ - request a reset password token by using the email parameter
    # POST reset_password/confirm/ - using a valid token, the users password is set to the provided password
    # POST reset_password/validate_token/ - will return a 200 if a given token is valid
    path('auth/reset_password/', include('django_rest_passwordreset.urls')),
]

urlpatterns += [
    path('account/', AccountView.as_view(), name='account'),
    path('profile/', ProfileView.as_view(), name='profile'),
    path('search/<str:username>/', SearchUsersView.as_view(), name='search_user'),
    path('activity_logs/', EmployeeActivityLogsView.as_view(), name='employee_activity_logs')
]

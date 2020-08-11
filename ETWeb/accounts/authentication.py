from rest_framework.authentication import BaseAuthentication
from django.middleware.csrf import CsrfViewMiddleware
from rest_framework.authtoken.models import Token
from rest_framework import exceptions
from django.conf import settings
from django.contrib.auth import get_user_model


User = get_user_model()


class CSRFCheck(CsrfViewMiddleware):
    def _reject(self, request, reason):
        # Return the failure reason instead of an HttpResponse
        return reason


class SafeTokenAuthentication(BaseAuthentication):
    """
        custom authentication class for DRF and JWT
        https://github.com/encode/django-rest-framework/blob/master/rest_framework/authentication.py
    """

    def authenticate(self, request):
        print(f'Request cookies {request.COOKIES}')

        auth_header = request.headers.get('Authorization')

        if auth_header:
            try:
                # header = 'Token xxxxxxxxxxxxxxxxxxxxxxxx'
                auth_token = auth_header.split(' ')[1]
            except IndexError:
                raise exceptions.AuthenticationFailed('Token prefix missing')
        else:
            auth_token = request.get_signed_cookie(settings.AUTH_TOKEN_KEY, default=None,
                                                   salt=settings.SECRET_KEY)
            if not auth_token:
                return None

        try:
            user = Token.objects.get(key=auth_token).user
        except Token.DoesNotExist:
            raise exceptions.AuthenticationFailed('Could not authenticate with provided credentials.')

        if user is None:
            raise exceptions.AuthenticationFailed('User not found')

        if not user.is_active:
            raise exceptions.AuthenticationFailed('User is inactive')

        self.enforce_csrf(request)
        return user, None

    def enforce_csrf(self, request):
        """
        Enforce CSRF validation
        """
        check = CSRFCheck()
        # populates request.META['CSRF_COOKIE'], which is used in process_view()
        check.process_request(request)
        reason = check.process_view(request, None, (), {})
        print(reason)
        if reason:
            # CSRF failed, bail with explicit error message
            raise exceptions.PermissionDenied('CSRF Failed: %s' % reason)

from django.apps import AppConfig


class AuthorizationConfig(AppConfig):
    name = 'accounts'

    def ready(self):
        # connect signals
        import accounts.api.signals

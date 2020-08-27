from django.conf import settings
from django_rest_passwordreset.signals import reset_password_token_created
from django.core.mail import EmailMessage
from django.utils.encoding import force_bytes
from django.utils.http import urlsafe_base64_encode
from django.dispatch import Signal, receiver
from django.db.models.signals import post_save
from django.template.loader import render_to_string
from .tokens import account_activation_token


account_activated = Signal()


@receiver(account_activated, sender=settings.AUTH_USER_MODEL, weak=False)
def user_account_activated(sender, instance, token, userprofile, **kwargs):
    # create authentication token and user profile
    token.objects.get_or_create(user=instance)
    userprofile.objects.get_or_create(user=instance)


@receiver(post_save, sender=settings.AUTH_USER_MODEL)
def user_instance_created(sender, instance, created, raw, **kwargs):
    # if created user is already activated, do not send confirmation email
    if created and not instance.is_active:
        mail_subject = 'Activate your account.'
        link = '{protocol}://{domain}/activate/{uid}/{token}'.format(
            protocol='https' if settings.USE_HTTPS else 'http',
            domain=settings.BASE_URL,
            uid=urlsafe_base64_encode(force_bytes(instance.pk)),
            token=account_activation_token.make_token(instance)
        )

        message = render_to_string('accounts/account_active_email.html', {
            'username': instance.username,
            'confirm_link': link,
        })
        EmailMessage(mail_subject, message, to=[instance.email]).send(fail_silently=True)


@receiver(reset_password_token_created)
def password_reset_token_created(sender, instance, reset_password_token, *args, **kwargs):
    """
    Handles password reset tokens
    When a token is created, an e-mail needs to be sent to the user
    """
    link = '{protocol}://{domain}/reset-password/confirm/{token}'.format(
        protocol='https' if settings.USE_HTTPS else 'http',
        domain=settings.BASE_URL,
        token=reset_password_token.key
    )
    # send an e-mail to the user
    context = {
        'username': reset_password_token.user.username,
        'reset_password_link': link
    }

    email_title = 'Password Reset for {title}'.format(title=settings.BASE_URL)
    email_message = render_to_string('accounts/user_reset_password.html', context)

    EmailMessage(
        email_title,
        email_message,
        from_email=settings.EMAIL_HOST_USER,
        to=[reset_password_token.user.email]
    ).send(fail_silently=True)


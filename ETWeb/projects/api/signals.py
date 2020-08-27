from django.conf import settings
from django.dispatch import Signal, receiver
from django.db.models.signals import post_save
from django.core.mail import EmailMessage
from django.template.loader import render_to_string
from django.utils.http import urlsafe_base64_encode
from django.utils.encoding import force_bytes
from projects.models import ProjectInvitationToken


@receiver(post_save, sender=ProjectInvitationToken)
def attempted_to_add_new_members(sender, instance, created, raw, *args, **kwags):
    """
    Sends email invitation to the new member.
    """
    # instance created exactly as presented
    if raw:
        return
    if created:
        accept_link = '{protocol}://{domain}/projects/confirm-invitation/{token}'.format(
            protocol='https' if settings.USE_HTTPS else 'http',
            domain=settings.BASE_URL,
            token=urlsafe_base64_encode(force_bytes(instance.key))
        )
        email_title = 'Invitation to join a project.'
        email_message = render_to_string(
            'projects/project_join_invitation.html', {
                'invitation': instance,
                'accept_link': accept_link
            })

        EmailMessage(email_title,
                     email_message,
                     from_email=settings.EMAIL_HOST_USER,
                     to=[instance.new_member.email]).send(fail_silently=True)

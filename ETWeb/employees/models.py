from django.db import models
from django.contrib.auth.views import get_user_model
from django.db.models.signals import pre_save
from django.dispatch import receiver

User = get_user_model()


class ActivityInfo(models.Model):
    employee = models.ForeignKey(User, on_delete=models.CASCADE,
                                 verbose_name='employee the activity was made by')
    date = models.DateTimeField(auto_now_add=True, blank=False, null=False,
                                verbose_name='exact time when the data was collected')

    class Meta:
        db_table = 'employees_activity_info'
        ordering = ['date']


class ScreenshotActivity(models.Model):
    activity_info = models.ForeignKey(ActivityInfo, on_delete=models.CASCADE, null=False)
    image = models.ImageField(upload_to='employee_screenshots', blank=False, null=False,
                              verbose_name='screenshot itself')

    class Meta:
        db_table = 'employees_screenshots'


class NetworkActivity(models.Model):
    '''
        NetworkActivity model represents specific amount of internet traffic sent/received by an employee.
    '''

    MAXIMUM_URL_LENGTH = 2083

    HTTP = 'HTTP'
    SSL = 'SSL'
    TLS = 'TLS'
    PROTOCOL_CHOICES = (
        (HTTP, 'Hypertext Transfer Protocol'),
        (SSL, 'Secure Sockets Layer'),
        (TLS, 'Transport Layer Security')
    )

    REQUEST = 'Request'
    RESPONSE = 'Response'
    HTTP_MESSAGE_TYPE_CHOICES = (
        (REQUEST, 'HTTP Request'),
        (RESPONSE, 'HTTP Response')
    )

    CLIENT_HELLO = 'Client hello'
    SERVER_HELLO = 'Client hello'
    SSL_TLS_MESSAGE_TYPE_CHOICES = (
        (CLIENT_HELLO, 'Client hello message'),
        (SERVER_HELLO, 'Server hello message')
    )

    MESSAGE_TYPE_CHOICES = (
        *HTTP_MESSAGE_TYPE_CHOICES,
        *SSL_TLS_MESSAGE_TYPE_CHOICES
    )

    activity_info = models.ForeignKey(ActivityInfo, on_delete=models.CASCADE, null=False)
    host_name = models.CharField(max_length=MAXIMUM_URL_LENGTH, null=False, blank=False)
    message_count = models.IntegerField(default=0)
    protocol_type = models.CharField(max_length=255, choices=PROTOCOL_CHOICES, default=None,
                                     null=True, blank=True)
    message_type = models.CharField(max_length=255, choices=MESSAGE_TYPE_CHOICES, default=None,
                                    null=True, blank=True)

    class Meta:
        db_table = 'employees_network_messages'


@receiver(signal=pre_save, sender=NetworkActivity)
def check_protocol_message_type_compat(sender, instance, raw, using, **kwargs):
    protocol = instance.protocol_type
    msg_type = instance.message_type
    if protocol == sender.HTTP and (msg_type != sender.REQUEST or msg_type != sender.RESPONSE):
        raise ValueError('HTTP protocol may only have Request/Response message type')

    if (protocol == sender.SSL or protocol == sender.TLS) and \
            (msg_type != sender.CLIENT_HELLO or msg_type != sender.SERVER_HELLO):
        raise ValueError('SSL/TLS protocols may only have client/server hello message type')

    instance.save(using=using)

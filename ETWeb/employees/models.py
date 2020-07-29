from django.db import models
from django.contrib.auth.views import get_user_model
from socket import gethostbyaddr, herror
from ipaddress import ip_address

# from django.db.models.signals import pre_save
# from django.dispatch import receiver

User = get_user_model()


class ActivityInfo(models.Model):
    employee = models.ForeignKey(User, on_delete=models.CASCADE,
                                 verbose_name='employee the activity was made by')
    date = models.DateTimeField(auto_now_add=True, blank=False, null=False,
                                verbose_name='exact time when the data was collected')

    class Meta:
        abstract = True
        db_table = 'employees_activity_info'
        ordering = ['date']


class ScreenshotActivity(ActivityInfo):
    # activity_info = models.ForeignKey(ActivityInfo, on_delete=models.CASCADE, null=False)
    image = models.ImageField(upload_to='employee_screenshots', blank=False, null=False,
                              verbose_name='screenshot itself')

    class Meta:
        db_table = 'employees_screenshots'


class NetworkActivity(ActivityInfo):
    '''
        NetworkActivity model represents specific amount of internet traffic sent/received by an employee.
    '''

    MAXIMUM_URL_LENGTH = 2083

    HTTP = 'HTTP'
    SSL = 'SSL/TLS'
    PROTOCOL_CHOICES = (
        (HTTP, 'Hypertext Transfer Protocol'),
        (SSL, 'Secure Sockets Layer / Transport Layer Security'),
    )

    # activity_info = models.ForeignKey(ActivityInfo, on_delete=models.CASCADE, null=False)
    host_name = models.CharField(max_length=MAXIMUM_URL_LENGTH, null=False, blank=False)
    message_count = models.IntegerField(default=0)
    protocol_type = models.CharField(max_length=255, choices=PROTOCOL_CHOICES, default=None,
                                     null=True, blank=True)

    @staticmethod
    async def resolve_ipv4_host(ip):
        if ip_address(ip).is_private:
            return None

        try:
            return gethostbyaddr(ip)[0]
        except herror:
            return None

    class Meta:
        db_table = 'employees_network_messages'


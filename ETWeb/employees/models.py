from django.db import models
from django.contrib.auth.views import get_user_model

User = get_user_model()


class Screenshot(models.Model):
    employee = models.ForeignKey(User, on_delete=models.CASCADE,
                                 verbose_name='employee the screenshot is taken by')
    image = models.ImageField(upload_to='employee_screenshots', blank=False, null=False,
                              verbose_name='screenshot itself')
    date = models.DateTimeField(auto_now_add=True, blank=False, null=False,
                                verbose_name='exact time when the screenshot was taken')

    class Meta:
        db_table = "employees_screenshots"
        ordering = ["date"]
import os
import binascii
from django.db import models
from accounts.models import User


class Project(models.Model):
    DEFAULT_DESCRIPTION = 'No project description.'

    name = models.CharField(max_length=255, null=False, blank=False)
    members = models.ManyToManyField(User, db_table='project_user')
    budget_usd = models.FloatField(default=0.0,
                                   verbose_name='Money currently spent on a project.')
    description = models.CharField(max_length=2048, blank=True, default=DEFAULT_DESCRIPTION)

    def __str__(self):
        return self.name

    class Meta:
        db_table = "projects"


class ProjectInvitationToken(models.Model):
    key = models.CharField(max_length=40, primary_key=True, verbose_name='Invitation identifier.')
    project = models.ForeignKey(Project, on_delete=models.CASCADE)
    manager = models.ForeignKey(User, related_name='manager', on_delete=models.CASCADE,
                                verbose_name='User that invited a new member.')
    new_member = models.ForeignKey(User, related_name='new_member', on_delete=models.CASCADE,
                                   verbose_name='User that was invited.')
    accepted = models.BooleanField(verbose_name='Whether or not the invitation was accepted by a new member',
                                   default=False)
    timestamp = models.DateTimeField(auto_now_add=True)

    class Meta:
        db_table = 'projects_projectinvitationtokens'

    def save(self, *args, **kwargs):
        if not self.key:
            self.key = self.generate_key()
        return super().save(*args, **kwargs)

    @staticmethod
    def generate_key():
        return binascii.hexlify(os.urandom(20)).decode()

    def __str__(self):
        return self.key

from django.db import models
from accounts.models import User


class Project(models.Model):
    name = models.CharField(max_length=255, unique=True, blank=False)
    members = models.ManyToManyField(User, db_table='project_user')
    budget_usd = models.FloatField(default=0.0,
                                   verbose_name='Money currently spent on a project.')

    def __str__(self):
        return self.name

    class Meta:
        db_table = "projects"

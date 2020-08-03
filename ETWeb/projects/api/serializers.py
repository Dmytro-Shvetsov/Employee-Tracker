from rest_framework import serializers
from projects.models import Project
from accounts.api.serializers import HttpUserSerializer
from rest_framework.exceptions import ValidationError
from rest_framework import status


class GeneralProjectSerializer(serializers.ModelSerializer):
    members_count = serializers.SerializerMethodField('get_members_count')

    @staticmethod
    def get_members_count(project):
        # total members count not including the user that makes the request
        return project.members.count() - 1

    class Meta:
        model = Project
        fields = ['id', 'name', 'budget_usd', 'description', 'members_count']


class DetailProjectSerializer(serializers.ModelSerializer):
    members = serializers.SerializerMethodField('get_member_list')

    @staticmethod
    def get_member_list(project):
        serializer = HttpUserSerializer(project.members.all(), many=True)
        return serializer.data

    def save(self):
        try:
            _ = Project.objects.get(name=self.validated_data['name'])
            raise ValidationError('Project with this name already exists.', status.HTTP_400_BAD_REQUEST)
        except Project.DoesNotExist:
            # check if description is present in the serializer's data
            desc = self.validated_data.get('description', Project.DEFAULT_DESCRIPTION)
            # verify if description is not string
            desc = desc if desc else Project.DEFAULT_DESCRIPTION
            new_project = Project.objects.create(
                name=self.validated_data['name'],
                description=desc
            )
            new_project.members.set([self.context['request'].user])
            return new_project

    class Meta:
        model = Project
        fields = ['id', 'name', 'budget_usd', 'description', 'members']


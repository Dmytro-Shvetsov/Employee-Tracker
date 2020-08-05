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

    def validate(self, attrs):
        if not self.instance:
            projects = self.context['projects']
            try:
                _ = projects.get(name__exact=attrs['name'])
                raise ValidationError({'name': 'You already have project with provided name.'})
            except Project.DoesNotExist:
                pass

        desc = attrs.get('description', Project.DEFAULT_DESCRIPTION)

        # verify if description is not an empty string
        attrs['description'] = desc if desc else Project.DEFAULT_DESCRIPTION

        return attrs

    def save(self):
        instance = self.instance
        if instance:
            instance.name = self.validated_data['name']
            instance.description = self.validated_data['description']
        else:
            instance = Project.objects.create(**self.validated_data)
            instance.members.set([self.context['request'].user])

        instance.save()
        return instance

    class Meta:
        model = Project
        fields = ['id', 'name', 'budget_usd', 'description', 'members']


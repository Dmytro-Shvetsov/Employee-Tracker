from rest_framework import serializers
from projects.models import Project
from accounts.api.serializers import HttpUserSerializer


class GeneralProjectSerializer(serializers.ModelSerializer):
    members_count = serializers.SerializerMethodField('get_members_count')

    @staticmethod
    def get_members_count(project):
        return project.members.count()

    class Meta:
        model = Project
        fields = ['id', 'name', 'budget_usd', 'members_count']


class DetailProjectSerializer(serializers.ModelSerializer):
    members = serializers.SerializerMethodField('get_member_list')

    @staticmethod
    def get_member_list(project):
        # print("\n".join(dir(project)))
        serializer = HttpUserSerializer(project.members.all(), many=True)
        return serializer.data

    class Meta:
        model = Project
        fields = ['id', 'name', 'budget_usd', 'members']


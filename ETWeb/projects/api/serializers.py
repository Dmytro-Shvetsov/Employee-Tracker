from django.contrib.auth import get_user_model
from django.db.models import Q
from rest_framework import serializers
from rest_framework.exceptions import ValidationError
from projects.models import Project
from accounts.api.serializers import HttpUserSerializer
from rest_framework import status


User = get_user_model()


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


class AddMembersSerializer(serializers.Serializer):
    # project_id = serializers.IntegerField(required=True)
    new_members = serializers.ListField(child=serializers.IntegerField(min_value=0), required=True)

    def __init__(self, instance, *args, **kwargs):
        self.instance = instance
        super().__init__(*args, **kwargs)
    # def validate_project_id(self, value):
    #     try:
    #         self.instance = self.context['projects'].get(id=value)
    #     except Project.DoesNotExist:
    #         raise ValidationError({'id': 'Project with specified id was not found.'})
    #     else:
    #         return value

    def validate_new_members(self, ids):
        # get user objects associated with passed ids
        associated_id_user_dict = User.objects.in_bulk(ids)
        # check if all passed user ids are valid
        if len(ids) != len(associated_id_user_dict.keys()):
            raise ValidationError('Some of the passed members do not exist.')

        users = associated_id_user_dict.values()
        # check if some of the passed users are already members of the target project
        present_and_new_users_itersect = self.instance.members.filter(id__in=[u.id for u in users])
        # fetching users by len
        cnt = len(present_and_new_users_itersect)
        usernames = [u.username for u in present_and_new_users_itersect]
        if usernames:
            error = (f"User {usernames[0]} is already a member of the project."
                     if cnt == 1 else
                     f"Users {', '.join(usernames)} are already members of the project.")

            raise ValidationError(error)

        # all users exist, and they are not members of the project
        return users

    def save(self, **kwargs):
        self.instance.members.add(*self.validated_data['new_members'])
        return self.instance


class RemoveMembersSerializer(serializers.Serializer):
    # project_id = serializers.IntegerField(required=True)
    delete_members = serializers.ListField(child=serializers.IntegerField(min_value=0), required=True)

    def __init__(self, instance, *args, **kwargs):
        self.instance = instance
        super().__init__(instance=instance, *args, **kwargs)

    def validate_delete_members(self, ids):
        if self.context['request'].user.id in ids:
            raise ValidationError('You cannot remove yourself from your project.')

        return ids

    def save(self, **kwargs):
        self.instance.members.filter(id__in=self.validated_data['delete_members']).delete()
        return self.instance


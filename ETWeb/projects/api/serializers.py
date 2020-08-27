from django.conf import settings
from django.contrib.auth import get_user_model
from rest_framework import serializers
from rest_framework.exceptions import ValidationError
from projects.models import Project, ProjectInvitationToken
from accounts.api.serializers import HttpUserSerializer
from django.utils.http import urlsafe_base64_decode
from django.utils.encoding import force_text, DjangoUnicodeDecodeError
from django.utils import timezone
from rest_framework import status


User = get_user_model()


class GeneralProjectSerializer(serializers.ModelSerializer):
    """
    Serializer class for representing overall information about a project.
    """

    members_count = serializers.SerializerMethodField('get_members_count')

    @staticmethod
    def get_members_count(project):
        # total members count not including the user that makes the request
        return project.members.count() - 1

    class Meta:
        model = Project
        fields = ['id', 'name', 'budget_usd', 'description', 'members_count']


class DetailProjectSerializer(serializers.ModelSerializer):
    """
    Serializer class for representing detailed information about a project.
    """

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
        """
        Creates new or updates existing projects.
        """
        instance = self.instance
        if instance:
            instance.name = self.validated_data['name']
            instance.description = self.validated_data['description']
            instance.save()
        else:
            instance = Project.objects.create(**self.validated_data)
            instance.members.set([self.context['request'].user])

        return instance

    class Meta:
        model = Project
        fields = ['id', 'name', 'budget_usd', 'description', 'members']


class AddMembersSerializer(serializers.Serializer):
    """
    Serializer for adding new members to a project.
    """

    new_members = serializers.ListField(child=serializers.IntegerField(min_value=0), required=True)

    def __init__(self, instance, *args, **kwargs):
        self.instance = instance
        super().__init__(instance=instance, *args, **kwargs)

    def validate_new_members(self, ids):
        if len(ids) == 0:
            raise ValidationError('No members selected.')

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
        """
        Sends invitations to join the project to all the new members.
        """
        manager = self.context['request'].user
        project = self.instance
        for member in self.validated_data['new_members']:
            # if an invitation for the particular user already exists, it's timestamp will be updated
            defaults = {
                'timestamp': timezone.now()
            }
            token, created = ProjectInvitationToken.objects.update_or_create(project=project,
                                                                             manager=manager,
                                                                             new_member=member,
                                                                             defaults=defaults)
            token.save()

        return self.instance


class RemoveMembersSerializer(serializers.Serializer):
    """
    Serializer class for removing members from a project.
    """
    delete_members = serializers.ListField(child=serializers.IntegerField(min_value=0), required=True)

    def __init__(self, instance, *args, **kwargs):
        self.instance = instance
        super().__init__(instance=instance, *args, **kwargs)

    def validate_delete_members(self, ids):
        if len(ids) == 0:
            raise ValidationError('No members selected.')

        if self.context['request'].user.id in ids:
            raise ValidationError('You cannot remove yourself from your own project.')

        return ids

    def save(self, **kwargs):
        self.instance.members.filter(id__in=self.validated_data['delete_members']).delete()
        return self.instance


class ProjectInvitationSerializer(serializers.Serializer):
    """
    Serializer class that represents invitations to join a project.
    """
    token = serializers.CharField(max_length=80, required=True)

    def get_invitation(self, raw_token):
        try:
            token = force_text(urlsafe_base64_decode(raw_token))
            return ProjectInvitationToken.objects.get(key=token, new_member=self.context['request'].user)
        except (ProjectInvitationToken.DoesNotExist, DjangoUnicodeDecodeError):
            raise ValidationError('Invalid token. Make sure you are logged in '
                                  'as the user mentioned in the invitation email.')

    @staticmethod
    def check_invitation_accepted(invitation):
        """
        Verifies the invitation is actual.
        """
        if invitation.accepted:
            raise ValidationError(f'You are already a member of project \'{invitation.project.name}\'',
                                  code=status.HTTP_200_OK)

    @staticmethod
    def check_invitation_expired(invitation):
        """
        Verifies the invitation hasn't expired.
        """
        range_end = timezone.now()
        range_start = range_end - timezone.timedelta(seconds=settings.PROJECT_INVITATION_AGE)
        valid_timestamp = range_start <= invitation.timestamp <= range_end
        if not valid_timestamp:
            raise ValidationError('This invitation has expired.')

    def validate_token(self, raw_token):
        invitation = self.get_invitation(raw_token)
        self.check_invitation_accepted(invitation)
        self.check_invitation_expired(invitation)
        return invitation

    def save(self, **kwargs):
        """
        Add new member to project and make the invitation resolved.
        """
        invitation = self.validated_data['token']

        invitation.project.members.add(invitation.new_member)
        invitation.accepted = True
        invitation.save()
        return invitation

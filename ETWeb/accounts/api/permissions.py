from rest_framework.permissions import BasePermission, IsAdminUser, IsAuthenticated
from rest_framework.exceptions import PermissionDenied
from django.contrib.auth import get_user_model


User = get_user_model()


class CanViewUserActivityLogs(BasePermission):
    def has_permission(self, request, view):
        staff_instance = request.user
        user_id = request.query_params.get('employee_id', view.kwargs.get('pk'))

        try:
            user = User.objects.get(id=user_id)
        except User.DoesNotExist:
            raise PermissionDenied('User does not exist.')

        if user.is_staff:
            raise PermissionDenied('You can only view logs of employee members.')

        # check if at least one of projects that an employee
        # is a member of has a staff member request.user
        if not user.project_set.prefetch_related('members').filter(members__id__exact=staff_instance.id):
            raise PermissionDenied('This user is not a member of any of your projects.')

        return True


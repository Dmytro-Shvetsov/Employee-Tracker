from rest_framework import serializers
from django.core.mail import EmailMessage
from django.conf import settings


class ContactFormSerializer(serializers.Serializer):
    MAX_EMAIL_BODY_LEN = 384000
    # full_name = serializers.EmailField(max_length=255, required=True)
    email = serializers.EmailField(max_length=255, required=True)
    subject = serializers.CharField(max_length=255, required=True)
    message = serializers.CharField(max_length=MAX_EMAIL_BODY_LEN, required=True, min_length=50)

    def save(self):
        EmailMessage(subject=self.validated_data['subject'],
                     body=self.validated_data['message'],
                     from_email=self.validated_data['email'],
                     to=[settings.EMAIL_HOST_USER]).send()

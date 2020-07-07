from django.shortcuts import render


def index(request):
    ctx = {}
    return render(request, "projects/index.html", ctx)


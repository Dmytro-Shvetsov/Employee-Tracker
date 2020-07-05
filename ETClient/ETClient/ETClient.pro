QT += core gui

greaterThan(QT_MAJOR_VERSION, 4) : QT += widgets

TARGET = ETClient
TEMPLATE = app

HEADERS += \
    Authorization/Headers/AuthForm.h

SOURCES += \
    Authorization/Sources/AuthForm.cpp \
    Main.cpp

FORMS += \
    Authorization/AuthForm.ui




QT += core gui

greaterThan(QT_MAJOR_VERSION, 4) : QT += widgets websockets

TARGET = ETClient
TEMPLATE = app

HEADERS += \
    Authorization/Headers/AuthForm.h \
    Authorization/Headers/AuthModel.h \
    Authorization/Headers/AuthPresenter.h \
    definitions.h

SOURCES += \
    Authorization/Sources/AuthForm.cpp \
    Authorization/Sources/AuthModel.cpp \
    Authorization/Sources/AuthPresenter.cpp \
    Main.cpp

FORMS += \
    Authorization/AuthForm.ui

RESOURCES += \
    Resources.qrc

DISTFILES += \
    Resources/auth_lock.jpg



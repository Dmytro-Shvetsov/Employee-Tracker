QT += core gui

greaterThan(QT_MAJOR_VERSION, 4) : QT += widgets websockets

TARGET = ETClient
TEMPLATE = app

HEADERS += \
    Authorization/Headers/AuthForm.h \
    Authorization/Headers/AuthModel.h \
    Authorization/Headers/AuthPresenter.h \
    Authorization/Headers/UserInfo.h \
    MainWindow/Headers/MainWindowForm.h \
    MainWindow/Headers/MainWindowModel.h \
    MainWindow/Headers/MainWindowPresenter.h \
    WebsocketClient.h \
    definitions.h

SOURCES += \
    Authorization/Sources/UserInfo.cpp \
    Authorization/Sources/AuthForm.cpp \
    Authorization/Sources/AuthModel.cpp \
    Authorization/Sources/AuthPresenter.cpp \
    Authorization/Sources/UserInfo.cpp \
    Main.cpp \
    MainWindow/Sources/MainWindowForm.cpp \
    MainWindow/Sources/MainWindowModel.cpp \
    MainWindow/Sources/MainWindowPresenter.cpp \
    WebsocketClient.cpp

FORMS += \
    Authorization/AuthForm.ui \
    MainWindow/MainWindowForm.ui

RESOURCES += \
    Resources.qrc

DISTFILES += \
    Resources/auth_lock.jpg



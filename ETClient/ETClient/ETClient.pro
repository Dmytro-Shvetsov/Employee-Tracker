QT += core gui

greaterThan(QT_MAJOR_VERSION, 4) : QT += widgets websockets concurrent

TARGET = ETClient
TEMPLATE = app

INCLUDEPATH += Dependencies/include \
               Authorization/Headers/ \
               MainWindow/Headers/ \
               Network

BUILD_VERSION = x86
BUILD_MODE = Debug

LIBS += -L"Dependencies/lib/$${BUILD_VERSION}/" \
        -L"Dependencies/lib/$${BUILD_VERSION}/$${BUILD_MODE}"

DESTDIR = bin

message($$LIBS)

HEADERS += \
    Authorization/Headers/AuthForm.h \
    Authorization/Headers/AuthModel.h \
    Authorization/Headers/AuthPresenter.h \
    MainWindow/Headers/MainWindowForm.h \
    MainWindow/Headers/MainWindowModel.h \
    MainWindow/Headers/MainWindowPresenter.h \
    MainWindow/Headers/ScreenshotManager.h \
    MainWindow/Headers/ScreenshotManager.h \
    Network/HttpStatsCollector.h \
    Network/SSLStatsCollector.h \
    WebsocketClient.h \
    definitions.h

SOURCES += \
    Authorization/Sources/AuthForm.cpp \
    Authorization/Sources/AuthModel.cpp \
    Authorization/Sources/AuthPresenter.cpp \
    Main.cpp \
    MainWindow/Sources/MainWindowForm.cpp \
    MainWindow/Sources/MainWindowModel.cpp \
    MainWindow/Sources/MainWindowPresenter.cpp \
    MainWindow/Sources/ScreenshotManager.cpp \
    WebsocketClient.cpp

FORMS += \
    Authorization/AuthForm.ui \
    MainWindow/MainWindowForm.ui

RESOURCES += \
    Resources.qrc

DISTFILES += \
    Resources/auth_lock.jpg





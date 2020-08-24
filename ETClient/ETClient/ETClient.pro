QT += core gui

greaterThan(QT_MAJOR_VERSION, 4) : QT += widgets websockets concurrent

TARGET = ETClient
TEMPLATE = app

INCLUDEPATH += Dependencies/include \
               Authorization/Headers/ \
               MainWindow/Headers/ \
               Network/Headers

DEPENDENCY_DIR = ../ETClient/Dependencies/
BUILD_VERSION = x86
BUILD_MODE = Release


LIBS += -lWs2_32 -lIPHLPAPI -lUser32
LIBS += -L"$${DEPENDENCY_DIR}/lib/$${BUILD_VERSION}/" -lwpcap \
                                                        -lPacket \
                                                        -lpthreadVC2 \
        -L"$${DEPENDENCY_DIR}/lib/$${BUILD_VERSION}/$${BUILD_MODE}" -lCommon++ -lPacket++ -lPcap++

DESTDIR = bin


HEADERS += \
    Authorization/Headers/AuthForm.h \
    Authorization/Headers/AuthModel.h \
    Authorization/Headers/AuthPresenter.h \
    MainWindow/Headers/ConnectionStatusManager.h \
    MainWindow/Headers/MainWindowForm.h \
    MainWindow/Headers/MainWindowModel.h \
    MainWindow/Headers/MainWindowPresenter.h \
    MainWindow/Headers/NetworkManager.h \
    MainWindow/Headers/ScreenshotManager.h \
    MainWindow/Headers/ScreenshotManager.h \
    Network/Headers/BaseStatsCollector.h \
    Network/Headers/HttpStatsCollector.h \
    Network/Headers/SSLStatsCollector.h \
    Network/Headers/WebsocketClient.h \
    definitions.h

SOURCES += \
    Authorization/Sources/AuthForm.cpp \
    Authorization/Sources/AuthModel.cpp \
    Authorization/Sources/AuthPresenter.cpp \
    MainWindow/Sources/ConnectionStatusManager.cpp \
    MainWindow/Sources/MainWindowForm.cpp \
    MainWindow/Sources/MainWindowModel.cpp \
    MainWindow/Sources/MainWindowPresenter.cpp \
    MainWindow/Sources/NetworkManager.cpp \
    MainWindow/Sources/ScreenshotManager.cpp \
    Network/Sources/BaseStatsCollector.cpp \
    Network/Sources/HttpStatsCollector.cpp \
    Network/Sources/SSLStatsCollector.cpp \
    Network/Sources/WebsocketClient.cpp \
    Main.cpp \

FORMS += \
    Authorization/AuthForm.ui \
    MainWindow/MainWindowForm.ui

RESOURCES += \
    Resources.qrc

DISTFILES += \
    Resources/auth_lock.jpg \
    Resources/warning.ico \
    Resources/warning.jpg

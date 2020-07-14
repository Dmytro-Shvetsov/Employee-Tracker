#pragma once

#include <QObject>
#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QTimer>
#include <QFuture>
#include "WebsocketClient.h"
#include "ScreenshotManager.h"


namespace ETClient
{
    class MainWindowModel : public QObject
    {
        Q_OBJECT
    private:
        UserInfo* usrInfo;
        WebsocketClient* socket;
        ScreenshotManager* screenshotManager;
        QWaitCondition waitCond;
    private slots:
        void onScreenshotReady();
        void onWebsocketConnected();
        void onWebsocketDisconnect();
    public:
        explicit MainWindowModel(UserInfo* usrInfo, QObject* parent = nullptr, QWindow* windowObj = nullptr);
        ~MainWindowModel();
        void startDataCollection();
        void stopDataCollection();
        void connectClient();
    signals:
        void websocketConnected();
        void websocketDisconnected();
    };
}


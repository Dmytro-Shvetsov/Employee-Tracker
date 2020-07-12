#pragma once

#include <QObject>
#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QTimer>
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
    private slots:
        void onScreenshotReady();
        void onWebsocketConnected();
    public:
        explicit MainWindowModel(UserInfo* usrInfo, QObject* parent = nullptr, QWindow* windowObj = nullptr);
        ~MainWindowModel();
        void startMakingScreenshots();
        void connectClient();
    signals:
        void websocketConnected();
    };
}


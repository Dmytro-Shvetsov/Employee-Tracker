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
        WebsocketClient* socket = new WebsocketClient(this, true);
        ScreenshotManager* screenshotManager;
        QWaitCondition waitCond;
        bool websocketIsConnected = false;
    private slots:
        void onScreenshotReady();
        void onWebsocketConnected();
        void onWebsocketDisconnect();
        void onTextMessageReceived(const QString& message);
    public:
        explicit MainWindowModel(QObject* parent = nullptr, QWindow* windowObj = nullptr);
        ~MainWindowModel();
        void startDataCollection();
        void stopDataCollection();
        void connectClient(const QString& token);
        void disconnectClient();
        bool clientIsConnected()const;
    signals:
        void websocketConnected();
        void websocketDisconnected();
        void textMessageReceived(const QString&);
    };
}


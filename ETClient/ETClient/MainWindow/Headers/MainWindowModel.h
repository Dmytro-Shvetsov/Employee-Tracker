#pragma once

#include <QObject>
#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QTimer>
#include <QFuture>
#include <QThread>
#include "WebsocketClient.h"
#include "ScreenshotManager.h"
#include "NetworkManager.h"
#include "ConnectionStatusManager.h"


namespace ETClient
{
    class MainWindowModel : public QObject
    {
        Q_OBJECT
    private:
        WebsocketClient* socket = new WebsocketClient(this);
        bool websocketIsConnected = false;

        ScreenshotManager* screenshotManager;
        NetworkManager* networkManager;
        ConnectionStatusManager* conStatusManager;

        QWaitCondition waitCond;
        QList<QFuture<void>> workerStates;
    private slots:
        void onScreenshotReady();
        void onNetworkDataReady();
        void onNetworkInterfaceNotConfigured();
        void onWebsocketConnected();
        void onWebsocketDisconnect();
        void onTextMessageReceived(const QString& message);
        void onStatusChanged(const qint8& newStatus);
    public:
        explicit MainWindowModel(QObject* parent = nullptr, QWindow* windowObj = nullptr);
        ~MainWindowModel();
        void startDataCollection();
        void stopDataCollection();
        void connectClient(const QString& token);
        void disconnectClient();
        bool clientIsConnected()const;
        void sendMessage(const QJsonObject& message);
        void setConnectionStatus(const qint8& newStatus);
    signals:
        void websocketConnected();
        void websocketDisconnected();
        void networkInterfaceNotConfigured();
        void textMessageReceived(const QString&);
        void statusChanged(const qint8& newStatus);
    };
}


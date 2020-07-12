#pragma once

#include <QObject>
#include <QUrl>
#include <QDebug>
#include <QWebSocket>
#include <QNetworkAccessManager>
#include "Authorization/Headers/UserInfo.h"
#include "definitions.h"

namespace ETClient
{
    class WebsocketClient: public QObject
    {
        Q_OBJECT
    private:
        const QUrl* host;
        bool debug;
        QWebSocket ws;
    private slots:
        void onConnected();
        void onDisconnected();
        void onMessageReceived(const QString& message);
    signals:
        void connected();
        void disconnected();
        void messageReceived(const QString&);

    public:
        WebsocketClient(QObject* parent = nullptr, bool debug = false);
        void connectClient(UserInfo* usrInfo);
        ~WebsocketClient();

    };
}

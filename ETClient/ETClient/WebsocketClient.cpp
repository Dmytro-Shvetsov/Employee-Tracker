#include "WebsocketClient.h"

namespace ETClient
{
    WebsocketClient::WebsocketClient(QObject* parent, bool debug):
        QObject(parent),
        host(new QUrl(COMMUNICATION_HOST_URL)),
        debug(debug)
    {
        if (debug)
        {
            qDebug() << "WebSocket server: " << this->host;
        }
        connect(&this->ws,
                SIGNAL(connected()),
                this,
                SLOT(onConnected()));
        connect(&this->ws,
                SIGNAL(disconnected()),
                this,
                SLOT(onDisconnected()));
    }

    void WebsocketClient::connectClient(UserInfo *usrInfo)
    {
        QNetworkRequest wsRequest = this->ws.request();
        wsRequest.setUrl(*this->host);
        qDebug() << "request " << wsRequest.url();
        wsRequest.setRawHeader("authorization",
                               usrInfo->getAuthToken().prepend("token ").toLocal8Bit().data());
        this->ws.open(wsRequest);
    }

    void WebsocketClient::onConnected()
    {
        if (debug)
        {
            qDebug() << "Connected to the server: " << this->host;
        }
        emit this->connected();
    }

    void WebsocketClient::onDisconnected()
    {
        if (debug)
        {
            qDebug() << "Disconnected from the server: " << this->host;
        }
    }

    void WebsocketClient::onMessageReceived(const QString& message)
    {
        qDebug() << "Received message: " << message;
    }

    WebsocketClient::~WebsocketClient()
    {
        delete this->host;
    }

}


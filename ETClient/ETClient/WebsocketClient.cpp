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

        this->ws.open(*this->host);
    }

    void WebsocketClient::onConnected()
    {
        if (debug)
        {
            qDebug() << "Connected to the server: " << this->host;
        }
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
        auto a = message;
    }

    WebsocketClient::~WebsocketClient()
    {
        delete this->host;
    }

}


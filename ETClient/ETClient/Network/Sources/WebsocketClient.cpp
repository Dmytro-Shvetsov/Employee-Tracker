#include "WebsocketClient.h"

namespace ETClient
{
    WebsocketClient::WebsocketClient(QObject* parent):
        QObject(parent)
    {
        connect(&this->ws,
                SIGNAL(connected()),
                this,
                SLOT(onConnected()));
        connect(&this->ws,
                SIGNAL(disconnected()),
                this,
                SLOT(onDisconnected()));
        connect(&this->ws,
                SIGNAL(textMessageReceived(const QString&)),
                this,
                SLOT(onTextMessageReceived(const QString&)));
    }

    void WebsocketClient::connectClient(const QString& token)
    {
        QNetworkRequest wsRequest = this->ws.request();
        wsRequest.setUrl(*this->host);
        qDebug() << "Request " << wsRequest.url() << "with token " << token;

        wsRequest.setRawHeader("authorization", ("token " + token).toLocal8Bit().data());
        this->ws.open(wsRequest);
    }

    void WebsocketClient::disconnectClient()
    {
        this->ws.close();
    }

    void WebsocketClient::sendMessage(const QJsonObject& message)
    {
        if (this->ws.isValid())
        {
            QJsonDocument jsonDoc(message);
            this->ws.sendTextMessage(jsonDoc.toJson(QJsonDocument::Compact));
        }
    }

    void WebsocketClient::sendMessage(const QByteArray &message)
    {
        if (this->ws.isValid())
        {
            this->ws.sendBinaryMessage(message);
        }
    }

    void WebsocketClient::onConnected()
    {
        qDebug() << "Connected to the server: " << *this->host;
        emit this->connected();
    }

    void WebsocketClient::onDisconnected()
    {
        qDebug() << "Disconnected from the server: " << *this->host;
        emit this->disconnected();
    }

    void WebsocketClient::onTextMessageReceived(const QString& message)
    {
        qDebug() << "Received message: ";
        emit this->textMessageReceived(message);
    }

    WebsocketClient::~WebsocketClient()
    {
        qDebug() << "Deleted websocket client ";
        delete this->host;
        this->ws.deleteLater();
    }

}


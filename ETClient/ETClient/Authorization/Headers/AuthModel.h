#pragma once

#include <QObject>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "definitions.h"


namespace ETClient
{
    class AuthModel : public QObject
    {
        Q_OBJECT
    private:
        QUrl* host = new QUrl(AUTH_URL);
        QNetworkAccessManager* reqManager = new QNetworkAccessManager(this);
        QString authToken;

        void setToken(const QString& value);
    private slots:
        void onAuthRequestFinish(QNetworkReply* reply);

    public:
        explicit AuthModel(QObject* parent = nullptr);
        ~AuthModel();

        void authorize(QString username, QString password);
        QString getToken()const;
    signals:
        void authorizationSuccessful();
        void authError(const QString& message);
    };
}

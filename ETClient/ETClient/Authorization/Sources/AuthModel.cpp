#include "Authorization/Headers/AuthModel.h"


namespace ETClient
{

    AuthModel::AuthModel(QObject* parent):
        QObject(parent),
        reqManager(new QNetworkAccessManager(this))
    {
        this->host = new QUrl(COMMUNICATION_HOST_URL + HOST_AUTH_ROUTE);

        connect(this->reqManager,
                SIGNAL(finished(QNetworkReply*)),
                this,
                SLOT(onAuthRequestFinish(QNetworkReply*)));
    }

    AuthModel::~AuthModel()
    {
        qDebug() << "Deleted AuthModel";
        delete this->host;
        delete this->reqManager;
    }

    void AuthModel::authorize(QString username, QString password)
    {
        qDebug() << "Quering " << *this->host;
        qDebug() << "Username: " << username << "\n" << \
                    "Password: " << password;

        QUrlQuery query;
        query.addQueryItem("username", username);
        query.addQueryItem("password", password);

        // validating query in case of special characters
        QString qs = query.query(QUrl::FullyEncoded);
        qs.replace("+", "%2B");

        QByteArray qsba = qs.toUtf8();

        QNetworkRequest request(*this->host);
        request.setHeader(
                    QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded"
                );
        request.setHeader(
                    QNetworkRequest::ContentLengthHeader,
                    qsba.count()
                );

        this->reqManager->post(request, qsba);
    }

    void AuthModel::setToken(const QString& value)
    {
        this->authToken = value;
        emit this->authorizationSuccessful();
    }

    QString AuthModel::getToken() const
    {
        return this->authToken;
    }

    void AuthModel::onAuthRequestFinish(QNetworkReply* reply)
    {
        if (!reply)
        {
            qDebug() << "Unknown error";
            return;
        }

        if (reply->error() == QNetworkReply::NoError && reply->bytesAvailable() > 0)
        {
            QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
            qDebug() << "Token " << response["token"].toString();
            this->setToken(response["token"].toString());
            reply->deleteLater();
        }
        else
        {
            emit invalidCredentials();
        }
    }
}

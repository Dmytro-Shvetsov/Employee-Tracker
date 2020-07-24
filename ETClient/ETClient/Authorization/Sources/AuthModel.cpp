#include "AuthModel.h"


namespace ETClient
{

    AuthModel::AuthModel(QObject* parent):
        QObject(parent)
    {
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
        if (!reply || reply->error() != QNetworkReply::NoError)
        {
            try
            {
                qDebug() << reply->error();
                reply->deleteLater();
            } catch (std::exception&)
            {
                qDebug() << "Unknown error";
            }
            emit this->unhandledError();
            return;
        }

        QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
        QString errors = response["errors"].toString();
        if (errors.isEmpty() || errors.isNull())
        {
            qDebug() << "Token " << response["token"].toString();
            this->setToken(response["token"].toString());
        }
        else
        {
            emit invalidCredentials();
        }
        reply->deleteLater();
    }
}

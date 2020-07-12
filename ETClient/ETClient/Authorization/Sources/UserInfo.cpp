#include "Authorization/Headers/UserInfo.h"


namespace ETClient
{
    UserInfo::UserInfo() {}

    UserInfo::UserInfo(const QString& username, const QString& token,
                       const bool online, QObject* parent):
        QObject(parent), token(token), username(username), online(online)
    {

    }

    UserInfo::~UserInfo()
    {
        qDebug() << "Deleted UserInfo";
    }

    QString UserInfo::getUsername() const
    {
        return this->username;
    }

    QString UserInfo::getAuthToken() const
    {
        return this->token;
    }
    void UserInfo::setActive(bool value)
    {
        this->online = value;
        emit this->onlineStatusChanged();
    }

    QByteArray& UserInfo::serialize()
    {
        QByteArray res;
        QDataStream ser(res);
        ser << this->username << this->token;
        return res;
    }

    void UserInfo::deserialize(const QByteArray& qba)
    {
        QDataStream deser(qba);
        deser >> this->username >> this->token;
    }

//    UserInfo UserInfo::deserialize(const QByteArray& qba)
//    {
//        QDataStream deser(qba);
//        QString username, token;
//        deser >> username >> token;
//        return UserInfo(username, token);
//    }

}


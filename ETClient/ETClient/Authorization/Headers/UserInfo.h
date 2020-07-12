#pragma once

#include <QObject>
#include <QDataStream>
#include <QDebug>


namespace ETClient
{
    class UserInfo : public QObject
    {
        Q_OBJECT
    private:
        QString username;
        QString token;
        bool online;
        // ... image
    public:
        UserInfo();
        explicit UserInfo(const QString& username,
                          const QString& token,
                          const bool online = true,
                          QObject* parent = nullptr);
        ~UserInfo();
        QString getUsername()const;
        QString getAuthToken()const;
        void setActive(bool value);
        QByteArray& serialize();
        void deserialize(const QByteArray& qba);
//        static UserInfo deserialize(const QByteArray& qba);
    signals:
        void onlineStatusChanged();
    };
}

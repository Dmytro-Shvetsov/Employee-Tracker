#pragma once

#include <QDebug>
#include <QString>

namespace ETClient
{
    class AuthModel
    {
    public:
        explicit AuthModel();
        bool authorizeUser(QString username, QString password);
    };
}

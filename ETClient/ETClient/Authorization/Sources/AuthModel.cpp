#include "Authorization/Headers/AuthModel.h"


namespace ETClient
{
    AuthModel::AuthModel()
    {

    }

    bool AuthModel::authorizeUser(QString username, QString password)
    {
        qDebug() << "Username: " << username << "\n" << \
                    "Password: " << password;
        return false;
    }
}

#pragma once

#include <QObject>
#include <QDebug>
#include <QString>
#include "Authorization/Headers/AuthForm.h"
#include "Authorization/Headers/AuthModel.h"

namespace ETClient
{
    class AuthPresenter  : public QObject
    {
        Q_OBJECT
    private:
        IAuthForm* authForm;
        AuthModel* authModel;
    public:
        explicit AuthPresenter(IAuthForm* authForm, QObject* parent = nullptr);
        void show();
        ~AuthPresenter();
    private slots:
        void onFormSubmit();
        void onTextChanged();
        void onAuthorizationSuccessful();
        void onInvalidCredentials();
    };
}



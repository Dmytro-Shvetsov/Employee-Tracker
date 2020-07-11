#pragma once

#include <QObject>
#include <QDebug>
#include <QString>
#include <QSettings>
#include "AuthForm.h"
#include "AuthModel.h"
#include "UserInfo.h"
#include "MainWindow/Headers/MainWindowPresenter.h"

namespace ETClient
{
    class AuthPresenter  : public QObject
    {
        Q_OBJECT
    private:
        QSettings* appSettings;
        IAuthForm* authForm;
        AuthModel* authModel;
        MainWindowPresenter* mvp;
    public:
        explicit AuthPresenter(QObject* parent = nullptr);
        ~AuthPresenter();
    private slots:
        void onFormSubmit();
        void onTextChanged();
        void onAuthorizationSuccessful();
        void onInvalidCredentials();
        void onUnhandledError();
    };
}



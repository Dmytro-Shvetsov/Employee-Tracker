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
        void initUiComponents();
    public:
        explicit AuthPresenter(QObject* parent = nullptr);
        void resetFormInfo();
        ~AuthPresenter();
    private slots:
        void onFormSubmit();
        void onTextChanged();
        void onAuthorizationSuccessful();
        void onInvalidCredentials();
        void onUnhandledError();
        void onLogout();
    };
}



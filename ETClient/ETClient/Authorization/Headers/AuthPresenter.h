#pragma once

#include <QObject>
#include <QDebug>
#include <QString>
#include <QSettings>
#include "AuthForm.h"
#include "AuthModel.h"
#include "MainWindowPresenter.h"

namespace ETClient
{
    class AuthPresenter  : public QObject
    {
        Q_OBJECT
    private:
        QSettings* appSettings = new QSettings(COMPANY_NAME, APPLICATION_TITLE);
        IAuthForm* authForm = new AuthForm;
        AuthModel* authModel = new AuthModel(this);
        MainWindowPresenter* mvp = new MainWindowPresenter(this);

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
        void onLogout(const QString& messaage="");
    };
}



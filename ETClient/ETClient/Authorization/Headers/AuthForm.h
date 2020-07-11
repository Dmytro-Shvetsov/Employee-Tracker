#pragma once

#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QDebug>
#include "ui_AuthForm.h"
#include "definitions.h"


namespace Ui
{
    class AuthForm;
}

namespace ETClient
{
    class IAuthForm
    {
    public:
        virtual void initUiComponents() = 0;
        virtual QString getInputUsername() = 0;
        virtual QString getInputPassword() = 0;
        virtual void setInputUsername(const QString& value) = 0;
        virtual void setInputPassword(const QString& value) = 0;
        virtual void setAlertMessage(const QString& text) = 0;
        virtual void setLoginButtonActive(bool value) = 0;
        virtual bool rememberMeChecked() = 0;
        virtual void setRememberMeChecked(bool value) = 0;
        virtual void showView() = 0;
        virtual void hideView() = 0;
        virtual ~IAuthForm() {}
    public: // signals
        virtual void loginBtnClicked() = 0;
        virtual void textInput() = 0;
    };

    class AuthForm : public QWidget, public IAuthForm
    {
    Q_OBJECT
    private:
        Ui::AuthForm* ui;

    public:
        explicit AuthForm(QWidget* parent = nullptr);
        ~AuthForm();

        void initUiComponents()override;
        QString getInputUsername()override;
        QString getInputPassword()override;
        void setInputUsername(const QString& value)override;
        void setInputPassword(const QString& value)override;
        void setAlertMessage(const QString& text)override;
        void setLoginButtonActive(bool value)override;
        bool rememberMeChecked()override;
        void setRememberMeChecked(bool value)override;
        void showView()override;
        void hideView()override;
    signals:
        void loginBtnClicked()override;
        void textInput()override;
    private slots:
        void onLoginBtnClicked();
        void onTextInput();
    };
}

#include "Authorization/Headers/AuthPresenter.h"


namespace ETClient
{
    AuthPresenter::AuthPresenter(ETClient::IAuthForm* authForm, QObject* parent)
        :QObject(parent),
         authForm(authForm),
         authModel(new AuthModel())
    {
        this->authForm->setAlertMessage("");
        this->authForm->setLoginButtonActive(false);

        QObject* viewObj = dynamic_cast<QObject*>(this->authForm);
        QObject::connect(viewObj,
                         SIGNAL(loginBtnClicked()),
                         this,
                         SLOT(submitForm()));

        QObject::connect(viewObj,
                         SIGNAL(textInput()),
                         this,
                         SLOT(checkChangedText()));
    }

    void AuthPresenter::show()
    {
        this->authForm->showView();
    }
    AuthPresenter::~AuthPresenter()
    {
        qDebug() << "Deleted AuthPresenter";
        delete this->authForm;
    }

    void AuthPresenter::submitForm()
    {
        QString username = this->authForm->getInputUsername();
        QString password = this->authForm->getInputPassword();

        this->authForm->setAlertMessage("Connecting...");
        bool response = this->authModel->authorizeUser(username, password);
        if (!response)
        {
            this->authForm->setAlertMessage("Invalid data.");
        }
        else
        {
            // switch to the main window.
        }

    }

    void AuthPresenter::checkChangedText()
    {
        QString username = this->authForm->getInputUsername();
        QString password = this->authForm->getInputPassword();
        qDebug() << password << username;

        bool inpDataIsEmpty = (username.isNull() || username.isEmpty() ||
                               password.isNull() || password.isEmpty());
        this->authForm->setLoginButtonActive(!inpDataIsEmpty);
    }

}



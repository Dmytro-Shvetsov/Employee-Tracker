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
                         SLOT(onFormSubmit()));
        QObject::connect(viewObj,
                         SIGNAL(textInput()),
                         this,
                         SLOT(onTextChanged()));

        QObject* modelObj = dynamic_cast<QObject*>(this->authModel);
        QObject::connect(modelObj,
                         SIGNAL(authorizationSuccessful()),
                         this,
                         SLOT(onAuthorizationSuccessful()));
        QObject::connect(modelObj,
                         SIGNAL(invalidCredentials()),
                         this,
                         SLOT(onInvalidCredentials()));
    }

    void AuthPresenter::show()
    {
        this->authForm->showView();
    }
    AuthPresenter::~AuthPresenter()
    {
        qDebug() << "Deleted AuthPresenter";
        delete this->authForm;
        delete this->authModel;
    }

    void AuthPresenter::onFormSubmit()
    {
        QString username = this->authForm->getInputUsername();
        QString password = this->authForm->getInputPassword();

        this->authForm->setAlertMessage("Connecting...");
        this->authModel->authorize(username, password);
//        if (!response)
//        {
//            this->authForm->setAlertMessage("Invalid data.");
//        }
//        else
//        {
//            // switch to the main window.
//        }

    }

    void AuthPresenter::onTextChanged()
    {
        QString username = this->authForm->getInputUsername();
        QString password = this->authForm->getInputPassword();
        bool inpDataIsEmpty = (username.isNull() || username.isEmpty() ||
                               password.isNull() || password.isEmpty());
        this->authForm->setLoginButtonActive(!inpDataIsEmpty);
    }

    void AuthPresenter::onAuthorizationSuccessful()
    {
        QString token = this->authModel->getToken();
        // load main window
    }

    void AuthPresenter::onInvalidCredentials()
    {
        this->authForm->setAlertMessage("Invalid data.");
    }

}



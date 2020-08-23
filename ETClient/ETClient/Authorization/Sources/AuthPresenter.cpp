#include "AuthPresenter.h"


namespace ETClient
{
    AuthPresenter::AuthPresenter(QObject* parent)
        :QObject(parent)
    {
        this->initUiComponents();

        // Check if user has already signed in.
        if (this->appSettings->contains("user"))
        {
            qDebug() << this->appSettings->value("user");
            this->mvp->init(this->appSettings->value("user").toString());
        }
        else
        {
            this->authForm->showView();
        }

    }

    void AuthPresenter::resetFormInfo()
    {
        this->authForm->setAlertMessage("");
        this->authForm->setInputUsername("");
        this->authForm->setInputPassword("");
        this->authForm->setLoginButtonActive(false);
        this->authForm->setRememberMeChecked(false);
    }

    void AuthPresenter::initUiComponents()
    {
        this->authForm->initUiComponents();

        this->resetFormInfo();

        QObject* viewObj = dynamic_cast<QObject*>(this->authForm);
        QObject::connect(viewObj,
                         SIGNAL(loginBtnClicked()),
                         this,
                         SLOT(onFormSubmit()));
        QObject::connect(viewObj,
                         SIGNAL(textInput()),
                         this,
                         SLOT(onTextChanged()));

        QObject::connect(this->authModel,
                         SIGNAL(authorizationSuccessful()),
                         this,
                         SLOT(onAuthorizationSuccessful()));
        QObject::connect(this->authModel,
                         SIGNAL(authError(const QString&)),
                         this,
                         SLOT(onAuthError(const QString&)));
    }

    AuthPresenter::~AuthPresenter()
    {
        if (this->mvp != nullptr)
        {
            delete this->mvp;
        }
        delete this->appSettings;
        delete this->authForm;
        this->authModel->deleteLater();
        qDebug() << "Deleted AuthPresenter";
    }

    void AuthPresenter::onFormSubmit()
    {
        QString username = this->authForm->getInputUsername();
        QString password = this->authForm->getInputPassword();

        this->authForm->setAlertMessage("Connecting to the server...");
        this->authModel->authorize(username, password);
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
        if (!this->appSettings->contains("user") && this->authForm->rememberMeChecked())
        {
            this->appSettings->setValue("user", this->authModel->getToken());
            this->appSettings->sync();
        }

        this->authForm->hideView();
        if (!this->mvp)
        {
            this->mvp = new MainWindowPresenter(this);
        }
        this->mvp->init(this->authModel->getToken());
    }

    void AuthPresenter::onAuthError(const QString& message)
    {
        this->authForm->setAlertMessage(
            message != "" ? message : "Couldn't authorize. Make sure you have internet connection, or retry later."
        );
    }

    void AuthPresenter::onLogout(const QString& message)
    {
        qDebug() << "Logout(auth presenter slot) " << message;

        this->mvp->deleteLater();
        this->mvp = nullptr;

        this->appSettings->remove("user");

        this->resetFormInfo();
        this->authForm->setAlertMessage(message);
        this->authForm->showView();
    }
}

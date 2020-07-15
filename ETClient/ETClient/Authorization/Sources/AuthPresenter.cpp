#include "Authorization/Headers/AuthPresenter.h"


namespace ETClient
{
    AuthPresenter::AuthPresenter(QObject* parent)
        :QObject(parent),
         appSettings(new QSettings(COMPANY_NAME, APPLICATION_TITLE)),
         authForm(new AuthForm),
         authModel(new AuthModel(this)),
         mvp(new MainWindowPresenter(this))
    {
        // Check if user has already signed in.
        if (this->appSettings->contains("user"))
        {
            if (!this->mvp)
            {
                this->mvp = new MainWindowPresenter(this);
            }

            qDebug() << this->appSettings->value("user");
            this->authForm->hideView();
            this->mvp->init(this->appSettings->value("user").toString());
            return;
        }

        this->initUiComponents();
        this->authForm->showView();
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

        QObject* modelObj = dynamic_cast<QObject*>(this->authModel);
        QObject::connect(modelObj,
                         SIGNAL(authorizationSuccessful()),
                         this,
                         SLOT(onAuthorizationSuccessful()));
        QObject::connect(modelObj,
                         SIGNAL(invalidCredentials()),
                         this,
                         SLOT(onInvalidCredentials()));
        QObject::connect(modelObj,
                         SIGNAL(unhandledError()),
                         this,
                         SLOT(onUnhandledError()));
    }

    AuthPresenter::~AuthPresenter()
    {
        qDebug() << "Deleted AuthPresenter";
        delete this->mvp;
        delete this->appSettings;
        delete this->authForm;
        delete this->authModel;
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

    void AuthPresenter::onInvalidCredentials()
    {
        this->authForm->setAlertMessage("Wrong username or password.");
    }

    void AuthPresenter::onUnhandledError()
    {
        this->authForm->setAlertMessage("Couldn't authorize. Make sure you have internet connection, or retry later.");
    }

    void AuthPresenter::onLogout()
    {
        qDebug() << "Logout(auth presenter slot)";
        delete this->mvp;
        this->mvp = nullptr;
        this->appSettings->remove("user");
        this->resetFormInfo();
        this->authForm->showView();
    }
}

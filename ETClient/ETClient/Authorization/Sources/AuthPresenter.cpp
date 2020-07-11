#include "Authorization/Headers/AuthPresenter.h"


namespace ETClient
{
    AuthPresenter::AuthPresenter(QObject* parent)
        :QObject(parent),
         appSettings(new QSettings(COMPANY_NAME, APPLICATION_TITLE)),
         authForm(new AuthForm),
         authModel(new AuthModel),
         mvp(nullptr)
    {
        // Check if user has already signed in.
        if (this->appSettings->contains("user"))
        {
            auto* usrInfo = new UserInfo();
            usrInfo->deserialize(this->appSettings->value("user").toByteArray());
            this->mvp = new MainWindowPresenter(usrInfo, this);
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
        delete this->appSettings;
        delete this->authForm;
        delete this->authModel;
        delete this->mvp;
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
        UserInfo* usrInfo = nullptr;
        if (!this->appSettings->contains("user"))
        {
            usrInfo = new UserInfo(this->authForm->getInputUsername(),
                                   this->authModel->getToken());
            if (this->authForm->rememberMeChecked())
            {
                this->appSettings->setValue("user", usrInfo->serialize());
                this->appSettings->sync();
            }
        }
        else
        {
            usrInfo = new UserInfo;
            usrInfo->deserialize(this->appSettings->value("user").toByteArray());
        }

        this->authForm->hideView();
        this->mvp = new MainWindowPresenter(usrInfo, this);
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
        this->appSettings->remove("user");
        delete this->mvp;
        this->resetFormInfo();
        this->authForm->showView();
    }
}



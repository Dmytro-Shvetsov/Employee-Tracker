#include "MainWindow/Headers/MainWindowPresenter.h"


namespace ETClient
{
    MainWindowPresenter::MainWindowPresenter(UserInfo* usrInfo, QObject* parent):
        QObject(parent),
        mwForm(new MainWindowForm),
        mwModel(new MainWindowModel(usrInfo, this, this->mwForm->getWindowObj()))
    {
        connect(this->mwModel,
                SIGNAL(websocketConnected()),
                this,
                SLOT(onWebsocketConnected()));

        this->initUiComponents();
        this->mwForm->showView();
        this->mwModel->connectClient();
    }

    MainWindowPresenter::~MainWindowPresenter()
    {
        qDebug() << "Deleted MainWindowPresenter";
        delete this->mwForm;
        delete this->mwModel;
    }

    void MainWindowPresenter::initUiComponents()
    {
        this->mwForm->initUiComponents();

        QObject* viewObj = dynamic_cast<QObject*>(this->mwForm);
        connect(viewObj,
                SIGNAL(logout()),
                this,
                SLOT(onLogout()));

        QObject::connect(viewObj,
                         SIGNAL(logout()),
                         this->parent(),
                         SLOT(onLogout()));
    }

    void MainWindowPresenter::onLogout()
    {
        emit this->logout();
    }

    void MainWindowPresenter::onWebsocketConnected()
    {
        this->mwModel->startMakingScreenshots();
    }
}

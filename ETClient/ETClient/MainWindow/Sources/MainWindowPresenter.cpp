#include "MainWindow/Headers/MainWindowPresenter.h"


namespace ETClient
{
    MainWindowPresenter::MainWindowPresenter(UserInfo* usrInfo, QObject* parent):
        QObject(parent),
        mwForm(new MainWindowForm),
        mwModel(new MainWindowModel),
        usrInfo(usrInfo)
    {
        qDebug() << usrInfo->getUsername();

        this->initUiComponents();
        this->mwForm->showView();
    }

    MainWindowPresenter::~MainWindowPresenter()
    {
        qDebug() << "Deleted MainWindowPresenter";
        delete this->mwForm;
        delete this->mwModel;
        delete this->usrInfo;
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
}

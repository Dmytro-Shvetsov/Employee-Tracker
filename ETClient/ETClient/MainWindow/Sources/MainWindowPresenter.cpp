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
        this->mwForm->showView();
    }

    MainWindowPresenter::~MainWindowPresenter()
    {
        qDebug() << "Deleted MainWindowPresenter";
        delete this->mwForm;
        delete this->mwModel;
        delete this->usrInfo;
    }
}

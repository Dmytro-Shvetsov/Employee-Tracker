#pragma once

#include <QObject>
#include <QDebug>
#include "Authorization/Headers/UserInfo.h"
#include "MainWindowForm.h"
#include "MainWindowModel.h"

namespace ETClient
{
    class MainWindowPresenter : public QObject
    {
        Q_OBJECT
    private:
        IMainWindowForm* mwForm;
        MainWindowModel* mwModel;
        UserInfo* usrInfo;
        void initUiComponents();
    private slots:
        void onLogout();
    public:
        explicit MainWindowPresenter(UserInfo* usrInfo, QObject* parent = nullptr);
        ~MainWindowPresenter();
    signals:
        void logout();
    };
}

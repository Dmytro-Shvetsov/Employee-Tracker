#pragma once

#include <QObject>
#include <QDebug>
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

        void initUiComponents();
    private slots:
        void onLogout();
        void onWebsocketConnected();
        void onWebsocketDisconnected();
    public:
        explicit MainWindowPresenter(UserInfo* usrInfo, QObject* parent = nullptr);
        ~MainWindowPresenter();
    signals:
        void logout();
    };
}

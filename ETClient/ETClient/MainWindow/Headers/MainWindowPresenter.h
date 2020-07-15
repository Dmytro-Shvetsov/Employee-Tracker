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
        void handleWebsocketAcceptResponse(const QJsonDocument& msg);
    private slots:
        void onWindowClosed(QCloseEvent* event);
        void onLogout();
        void onWebsocketConnected();
        void onWebsocketDisconnected();
        void onTextMessageReceived(const QString& message);
    public slots:
        void destroy();
    public:
        explicit MainWindowPresenter(QObject* parent = nullptr);
        ~MainWindowPresenter();
        void init(const QString& userAuthToken);
    signals:
        void logout();
    };
}

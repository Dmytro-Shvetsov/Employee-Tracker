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
        IMainWindowForm* mwForm = new MainWindowForm;
        MainWindowModel* mwModel = new MainWindowModel(this, this->mwForm->getWindowObj());

        void initUiComponents();
        void handleWebsocketAcceptResponse(const QJsonDocument& response);
        void handleWebsocketCloseResponse(const QJsonDocument& response);
    private slots:
        void onWindowClosed(QCloseEvent* event);
        void onLogout(const QString& message="");
        void onWebsocketConnected();
        void onWebsocketDisconnected();
        void onTextMessageReceived(const QString& message);
        void onStatusChanged(const qint8& newStatus);
    public slots:
        void destroy();
    public:
        explicit MainWindowPresenter(QObject* parent = nullptr);
        ~MainWindowPresenter();
        void init(const QString& userAuthToken);
    signals:
        void logout(const QString&);
    };
}

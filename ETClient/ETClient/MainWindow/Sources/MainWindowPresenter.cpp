#include "MainWindowPresenter.h"


namespace ETClient
{
    MainWindowPresenter::MainWindowPresenter(QObject* parent):
        QObject(parent)
    {
        connect(this,
                SIGNAL(logout(const QString&)),
                this->parent(),
                SLOT(onLogout(const QString&)));

        connect(this->mwModel,
                &MainWindowModel::networkInterfaceNotConfigured,
                this,
                [this]() {
                    this->mwForm->showErrorMessage(
                        "Could not recognize your network device. Please set your active network "
                        "interface name or IP in application's root directory file "
                        "'" + NETWORK_INTERFACE_CONF_FILE + "' and try again."
                    );
                    this->onLogout();
                });
        connect(this->mwModel,
                SIGNAL(websocketConnected()),
                this,
                SLOT(onWebsocketConnected()));
        connect(this->mwModel,
                SIGNAL(websocketDisconnected()),
                this,
                SLOT(onWebsocketDisconnected()));
        connect(this->mwModel,
                SIGNAL(textMessageReceived(const QString&)),
                this,
                SLOT(onTextMessageReceived(const QString&)));
        connect(this->mwModel,
                SIGNAL(statusChanged(const qint8&)),
                this,
                SLOT(onStatusChanged(const qint8&)));
    }

    MainWindowPresenter::~MainWindowPresenter()
    {
        delete this->mwForm;
        this->mwModel->deleteLater();
        qDebug() << "Deleted MainWindowPresenter";
    }

    void MainWindowPresenter::init(const QString& userAuthToken)
    {
        this->initUiComponents();
        this->mwForm->showView();
        this->mwModel->connectClient(userAuthToken);
    }

    void MainWindowPresenter::initUiComponents()
    {
        this->mwForm->initUiComponents();

        QObject* viewObj = dynamic_cast<QObject*>(this->mwForm);
        connect(viewObj,
                SIGNAL(logout(const QString&)),
                this,
                SLOT(onLogout(const QString&)));

        connect(this,
                SIGNAL(logout(const QString&)),
                this->parent(),
                SLOT(onLogout(const QString&)));

        connect(viewObj,
                SIGNAL(windowClosed(QCloseEvent*)),
                this,
                SLOT(onWindowClosed(QCloseEvent*)));

    }

    void MainWindowPresenter::handleWebsocketAcceptResponse(const QJsonDocument& response)
    {
        this->mwForm->setUsernameText(response["username"].toString());

        this->mwForm->setDateJoined(
                    QDate::fromString(response["date_joined"].toString(), "yyyy-MM-dd")
                );

        QByteArray decodedImage = QByteArray::fromBase64(
                    response["profile_image"].toString().toLocal8Bit()
                );
        QPixmap img;
        if (img.loadFromData(decodedImage))
        {
            this->mwForm->setUserImage(img);
        }
        else
        {
            qDebug() << "Couldn't load user image.";
        }
        qDebug() << "Loaded user data";
        this->mwForm->setLoadingState(false);        
        this->mwModel->startDataCollection();
        this->mwModel->setConnectionStatus(STATUSES::ONLINE);
    }

    void MainWindowPresenter::handleWebsocketCloseResponse(const QJsonDocument& response)
    {
        this->mwForm->setLoadingState(false);
        this->onLogout(response["error"].toString());
    }

    void MainWindowPresenter::onWindowClosed(QCloseEvent* event)
    {
        this->mwModel->disconnectClient();
    }

    void MainWindowPresenter::onLogout(const QString& message)
    {
        this->mwModel->disconnectClient();

        const auto wsDisconnectCallback = [this, &message]() {
            this->mwForm->hideView();
            emit this->logout(message);
        };

        if (this->mwModel->clientIsConnected())
        {
            connect(this->mwModel,
                    &MainWindowModel::websocketDisconnected,
                    this,
                    wsDisconnectCallback);
        }
        else
        {
            wsDisconnectCallback();
        }
    }

    void MainWindowPresenter::onWebsocketConnected()
    {
        qDebug() << "Ws connected (presenter slot)";
    }

    void MainWindowPresenter::onWebsocketDisconnected()
    {
        this->mwModel->stopDataCollection();
        this->mwModel->setConnectionStatus(STATUSES::OFFLINE);
        qDebug() << "Ws disconnected (presenter slot)";
    }

    void MainWindowPresenter::onTextMessageReceived(const QString& message)
    {
        QJsonDocument response = QJsonDocument::fromJson(message.toLocal8Bit());
        qDebug() << "Message of type " << response["type"];
        if (response["type"] == "websocket.accept")
        {
            this->handleWebsocketAcceptResponse(response);
        } else if (response["type"] == "websocket.close")
        {
            qDebug() << "Websocket close";
            this->handleWebsocketCloseResponse(response);
        }
    }

    void MainWindowPresenter::onStatusChanged(const qint8& newStatus)
    {
        QString statusTxt = this->mwForm->setStatus(newStatus);

        QJsonObject message;
        message["type"] = "user.status.change";
        message["status"] = statusTxt;
        this->mwModel->sendMessage(message);
    }

    void MainWindowPresenter::destroy()
    {
        qDebug() << "Destroy method called";
        this->deleteLater();
    }

}

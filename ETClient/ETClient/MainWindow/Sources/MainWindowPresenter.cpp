#include "MainWindow/Headers/MainWindowPresenter.h"


namespace ETClient
{
    MainWindowPresenter::MainWindowPresenter(QObject* parent):
        QObject(parent),
        mwForm(new MainWindowForm),
        mwModel(new MainWindowModel(this, this->mwForm->getWindowObj()))
    {
        connect(this,
                SIGNAL(logout()),
                this->parent(),
                SLOT(onLogout()));

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
    }

    MainWindowPresenter::~MainWindowPresenter()
    {
        qDebug() << "Deleted MainWindowPresenter";
        delete this->mwForm;
        delete this->mwModel;
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
                SIGNAL(logout()),
                this,
                SLOT(onLogout()));
        connect(viewObj,
                SIGNAL(windowClosed(QCloseEvent*)),
                this,
                SLOT(onWindowClosed(QCloseEvent*)));
    }

    void MainWindowPresenter::handleWebsocketAcceptResponse(const QJsonDocument& message)
    {
        this->mwForm->setUsernameText(message["username"].toString());

        this->mwForm->setDateJoined(
                    QDate::fromString(message["date_joined"].toString(), "yyyy-MM-dd")
                );

        QByteArray decodedImage = QByteArray::fromBase64(
                    message["profile_image"].toString().toLocal8Bit()
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
        qDebug() << "HANDLED FIRST MESSAGE";
        this->mwModel->startDataCollection();
    }

    void MainWindowPresenter::onWindowClosed(QCloseEvent* event)
    {
        qDebug() << "EXIT BBBBBBBBBB";
        this->mwModel->disconnectClient();
        connect(this->mwModel,
                SIGNAL(websocketDisconnected()),
                this,
                SLOT(destroy()));
    }

    void MainWindowPresenter::onLogout()
    {
        this->mwModel->disconnectClient();
        this->mwForm->hideView();
        emit this->logout();
    }

    void MainWindowPresenter::onWebsocketConnected()
    {
        this->mwForm->setOnlineStatus(true);
        qDebug() << "Ws connected (presenter slot)";
    }

    void MainWindowPresenter::onWebsocketDisconnected()
    {
        this->mwModel->stopDataCollection();
        this->mwForm->setOnlineStatus(false);

        qDebug() << "Ws disconnected (presenter slot)";
    }

    void MainWindowPresenter::onTextMessageReceived(const QString& message)
    {
        QJsonDocument msg = QJsonDocument::fromJson(message.toLocal8Bit());
        qDebug() << "Message of type " << msg["type"];
        if (msg["type"] == "websocket.accept")
        {
            this->handleWebsocketAcceptResponse(msg);
        }
    }

    void MainWindowPresenter::destroy()
    {
        delete this;
    }
}

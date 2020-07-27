#include "MainWindowModel.h"


namespace ETClient
{
    MainWindowModel::MainWindowModel(QObject* parent, QWindow* windowObj):
        QObject(parent),
        screenshotManager(new ScreenshotManager(&this->waitCond, this, windowObj))
    {
        connect(this->socket,
                SIGNAL(connected()),
                this,
                SLOT(onWebsocketConnected()));
        connect(this->socket,
                SIGNAL(disconnected()),
                this,
                SLOT(onWebsocketDisconnect()));
        connect(this->socket,
                SIGNAL(textMessageReceived(const QString&)),
                this,
                SLOT(onTextMessageReceived(const QString&)));


        this->networkManager = new NetworkManager(&this->waitCond, this);

        connect(screenshotManager,
                SIGNAL(screenshotReady()),
                this,
                SLOT(onScreenshotReady()));
    }

    MainWindowModel::~MainWindowModel()
    {
        this->stopDataCollection();
        delete this->socket;
        delete this->screenshotManager;
        delete this->networkManager;
        qDebug() << "Deleted MainWindowModel";
    }

    void MainWindowModel::startDataCollection()
    {
        this->screenshotManager->setRunning(true);
        this->networkManager->setupDevice("192.168.0.104"); // for now it's explicit
        this->networkManager->setRunning(true);

        this->waitCond.wakeAll();
        if (this->workerStates.size() == 0)
        {
            QFuture<void> smFutureObj = QtConcurrent::run(this->screenshotManager, &ScreenshotManager::run);
            this->workerStates.append(smFutureObj);

            QFuture<void> nmFutureObj = QtConcurrent::run(this->networkManager, &NetworkManager::run);
            this->workerStates.append(nmFutureObj);
        }
    }

    void MainWindowModel::stopDataCollection()
    {
        this->screenshotManager->setRunning(false);
        this->networkManager->setRunning(false);
        this->waitCond.wakeAll();

        for(auto& item: this->workerStates)
        {
            item.waitForFinished();
        }
        qDebug() << "Finished data collection work, good to go";
    }

    void MainWindowModel::onScreenshotReady()
    {
        auto screenshotBytes = this->screenshotManager->getScreenshot();
        QJsonObject message;
        message["type"] = "data.screenshot";
        message["screenshot"] = QString::fromStdString(screenshotBytes.toStdString());
        this->socket->sendMessage(message);
        qDebug() << "Retrieved screenshot on thread " << QThread::currentThread();
    }

    void MainWindowModel::onWebsocketConnected()
    {
        this->websocketIsConnected = true;
        emit this->websocketConnected();
    }

    void MainWindowModel::onWebsocketDisconnect()
    {
        this->websocketIsConnected = false;
        emit this->websocketDisconnected();
    }

    void MainWindowModel::onTextMessageReceived(const QString &message)
    {
        emit this->textMessageReceived(message);
    }

    void MainWindowModel::connectClient(const QString& token)
    {
        this->socket->connectClient(token);
    }

    void MainWindowModel::disconnectClient()
    {
        this->socket->disconnectClient();
    }

    bool MainWindowModel::clientIsConnected() const
    {
        return this->websocketIsConnected;
    }
}

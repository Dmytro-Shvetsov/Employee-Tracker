#include "MainWindow/Headers/MainWindowModel.h"


namespace ETClient
{
    MainWindowModel::MainWindowModel(UserInfo* usrInfo, QObject* parent, QWindow* windowObj):
        QObject(parent),
        usrInfo(usrInfo),
        socket(new WebsocketClient(this, true)),
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

        connect(this->screenshotManager,
                SIGNAL(screenshotReady()),
                this,
                SLOT(onScreenshotReady()));
    }

    MainWindowModel::~MainWindowModel()
    {
        qDebug() << "Deleted MainWindowModel";
        delete this->usrInfo;
        delete this->screenshotManager;
    }

    void MainWindowModel::startDataCollection()
    {
        this->screenshotManager->setRunning(true);
        this->waitCond.wakeAll();
        QtConcurrent::run(this->screenshotManager, &ScreenshotManager::run);
    }

    void MainWindowModel::stopDataCollection()
    {
        this->screenshotManager->setRunning(false);
        this->waitCond.wakeAll();
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
        emit this->websocketConnected();
    }

    void MainWindowModel::onWebsocketDisconnect()
    {
        emit this->websocketDisconnected();
    }

    void MainWindowModel::connectClient()
    {
        this->socket->connectClient(this->usrInfo);
    }
}

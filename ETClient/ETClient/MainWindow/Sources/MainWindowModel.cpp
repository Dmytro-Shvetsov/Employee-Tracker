#include "MainWindow/Headers/MainWindowModel.h"


namespace ETClient
{
    MainWindowModel::MainWindowModel(UserInfo* usrInfo, QObject* parent, QWindow* windowObj):
        QObject(parent),
        usrInfo(usrInfo),
        socket(new WebsocketClient(this, true)),
        screenshotManager(new ScreenshotManager(this, windowObj, 5))
    {
        QObject* socketObj = dynamic_cast<QObject*>(this->socket);
        connect(socketObj,
                SIGNAL(connected()),
                this,
                SLOT(onWebsocketConnected()));
    }

    MainWindowModel::~MainWindowModel()
    {
        qDebug() << "Deleted MainWindowModel";
        delete this->usrInfo;
        delete this->screenshotManager;
        this->screenshotManager->setExecutingScreenshotCreationLoop(false);
    }

    void MainWindowModel::onScreenshotReady()
    {
        auto screenshot = this->screenshotManager->getScreenshot();
        qDebug() << "Retrieved screenshot on thread " << QThread::currentThread();
    }

    void MainWindowModel::onWebsocketConnected()
    {
        emit this->websocketConnected();
    }

    void MainWindowModel::startMakingScreenshots()
    {
        connect(this->screenshotManager,
                SIGNAL(screenshotReady()),
                this,
                SLOT(onScreenshotReady()));

        // Start the computation.
        QtConcurrent::run([&]{
            qDebug() << "Starting execution";
            this->screenshotManager->run();
        });
    }

    void MainWindowModel::connectClient()
    {
        this->socket->connectClient(this->usrInfo);
    }
}

#include "MainWindowModel.h"


namespace ETClient
{
    MainWindowModel::MainWindowModel(QObject* parent, QWindow* windowObj):
        QObject(parent),
        screenshotManager(new ScreenshotManager(&this->waitCond, this, windowObj)),
        networkManager(new NetworkManager(&this->waitCond, this)),
        conStatusManager(new ConnectionStatusManager(&this->waitCond, this))
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


        connect(this->screenshotManager,
                SIGNAL(screenshotReady()),
                this,
                SLOT(onScreenshotReady()));

        connect(this->networkManager,
                SIGNAL(dataReadyToExtract()),
                this,
                SLOT(onNetworkDataReady()));

        connect(this->networkManager,
                SIGNAL(networkInterfaceNotConfigured()),
                this,
                SLOT(onNetworkInterfaceNotConfigured()));

        connect(this->conStatusManager,
                SIGNAL(statusChanged(const qint8&)),
                this,
                SLOT(onStatusChanged(const qint8&)));
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
        this->networkManager->setRunning(true);
        this->screenshotManager->setRunning(true);
        this->conStatusManager->setRunning(true);

        this->waitCond.wakeAll();
        if (this->workerStates.size() == 0)
        {
            QFuture<void> smFutureObj = QtConcurrent::run(this->screenshotManager, &ScreenshotManager::run);
            this->workerStates.append(smFutureObj);

            QFuture<void> nmFutureObj = QtConcurrent::run(this->networkManager, &NetworkManager::run);
            this->workerStates.append(nmFutureObj);

            QFuture<void> cmFutureObj = QtConcurrent::run(this->conStatusManager, &ConnectionStatusManager::run);
            this->workerStates.append(cmFutureObj);
        }
    }

    void MainWindowModel::stopDataCollection()
    {
        this->screenshotManager->setRunning(false);
        this->networkManager->setRunning(false);
        this->conStatusManager->setRunning(false);
        this->waitCond.wakeAll();

        for(auto& item: this->workerStates)
        {
            item.waitForFinished();
        }
        this->workerStates.clear();
        this->removeEventFilter(this->conStatusManager);
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

    void MainWindowModel::onNetworkDataReady()
    {
        PacketArrivedData& data = this->networkManager->getData();

        QJsonObject message, httpData, sslData, otherData;
        message["type"] = "data.network";

        QJsonObject reqData, resData;
        QJsonArray reqHosts;
        // render HTTP request stats
        auto httpReqStats = data.httpStatsCollector->getRequestStats();
        reqData["message_count"] = QJsonValue::fromVariant(httpReqStats.numOfMessages);

        for(auto& pair : httpReqStats.hostnameCount)
        {
            QJsonObject item;
            item[pair.first] = QJsonValue::fromVariant(pair.second);
            reqHosts.append(item);
        }
        reqData["hostnames"] = reqHosts;
        httpData["request_stats"] = reqData;

        // render HTTP response stats
        auto* httpResStats = &data.httpStatsCollector->getResponseStats();
        resData["message_count"] = QJsonValue::fromVariant(httpResStats->numOfMessages);
        httpData["response_stats"] = resData;

        // mount HTTP data to the root node
        message["http"] = httpData;

        // render SSL/TLS stats
        QJsonObject clientHelloStats, serverHelloStats;
        QJsonArray helloHosts;

        auto* sslClientHelloStats = &data.sslStatsCollector->getClientHelloStats();
        clientHelloStats["message_count"] = sslClientHelloStats->numOfMessages;
        for(auto& pair : sslClientHelloStats->serverNameCount)
        {
            QJsonObject item;
            item[pair.first] = QJsonValue::fromVariant(pair.second);
            helloHosts.append(item);
        }
        clientHelloStats["hostnames"] = helloHosts;

        sslData["client_hello_stats"] = clientHelloStats;

        serverHelloStats["message_count"] = QJsonValue::fromVariant(
                        data.sslStatsCollector->getServerHelloStats().numOfMessages
                    );
        sslData["server_hello_stats"] = serverHelloStats;
        // mount SSL data to the root node
        message["ssl"] = sslData;
        sslClientHelloStats = nullptr;

        this->socket->sendMessage(message);
        data.clear();

    }

    void MainWindowModel::onNetworkInterfaceNotConfigured()
    {
        emit this->networkInterfaceNotConfigured();
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

    void MainWindowModel::onStatusChanged(const qint8& newStatus)
    {
        emit this->statusChanged(newStatus);
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

    void MainWindowModel::sendMessage(const QJsonObject& message)
    {
        if (!this->clientIsConnected())
        {
            qDebug() << "Attempted to send message when client is not connected. Message ignored";
            return;
        }
        this->socket->sendMessage(message);
    }

    void MainWindowModel::setConnectionStatus(const qint8& newStatus)
    {
        this->conStatusManager->setStatus(newStatus);
    }
}

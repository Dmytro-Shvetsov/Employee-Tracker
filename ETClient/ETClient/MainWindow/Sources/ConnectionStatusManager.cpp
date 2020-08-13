#include "ConnectionStatusManager.h"

namespace ETClient
{    
    ConnectionStatusManager::ConnectionStatusManager(QObject* parent)
        : QObject(parent)
    {
        this->currentStatus = STATUSES::OFFLINE;
        this->idleTimer.setSingleShot(true);
        this->idleTimer.setInterval(this->minMsForIdle);

        connect(&this->idleTimer,
                &QTimer::timeout,
                this,
                [this](){
                    this->setStatus(STATUSES::IDLE);
                });
    }

    ConnectionStatusManager::~ConnectionStatusManager() {}

    void ConnectionStatusManager::restartIdleTimer()
    {
        this->stopwatch.restart();
        this->idleTimer.start(this->minMsForIdle);
    }

    void ConnectionStatusManager::setStatus(qint8 statusCode)
    {
        if (statusCode < 0 || statusCode > STATUSES::STATUS_COUNT - 1)
        {
            throw std::exception("Invalid status code passed to the function.");
        }
        this->currentStatus = statusCode;
        emit this->statusChanged(statusCode);
    }

    bool ConnectionStatusManager::eventFilter(QObject* obj, QEvent* event)
    {
        if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseMove)
        {
            if (this->currentStatus != STATUSES::ONLINE)
            {
                this->setStatus(STATUSES::ONLINE);
            }
            this->restartIdleTimer();
            qDebug() << "Reset idle timer";

//            QKeyEvent* key = dynamic_cast<QKeyEvent*>(event);
            return true;
        }
        else
        {
            return QObject::eventFilter(obj, event);
        }
        return false;
    }
}


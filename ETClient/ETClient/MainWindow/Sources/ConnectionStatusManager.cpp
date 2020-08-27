#include "ConnectionStatusManager.h"

namespace ETClient
{    
    ConnectionStatusManager::ConnectionStatusManager(QWaitCondition* waitCond, QObject* parent, bool running)
        : QObject(parent),
          waitCond(waitCond),
          running(running)
    {
        this->currentStatus = STATUSES::OFFLINE;
    }

    ConnectionStatusManager::~ConnectionStatusManager() {}

    void ConnectionStatusManager::restartIdleTimer()
    {
        if (this->idleTimer == nullptr)
        {
            this->idleTimer = new QTimer();
            this->idleTimer->setSingleShot(true);
            this->idleTimer->setInterval(this->minMsForIdle);
            connect(this->idleTimer,
                    &QTimer::timeout,
                    this,
                    [this](){
                        this->setStatus(STATUSES::IDLE);
                    });
        }
        this->idleTimer->start(this->minMsForIdle);
        this->stopwatch.restart();
        qDebug() << "Restarted idle timer";
    }

    void ConnectionStatusManager::setRunning(bool value)
    {
        this->running = value;
    }

    void ConnectionStatusManager::run()
    {
        this->restartIdleTimer();
        int tempVar;
        BYTE keyboardKeyStates[256];
        while(this->running)
        {
            QCoreApplication::processEvents();
            // fill arr with zeroes
            memset(keyboardKeyStates, 0, 256);
            // GetKeyState has to be called onse to make GetKeyboardState work properly
            GetKeyState(0);
            if(GetKeyboardState(keyboardKeyStates) == true)
            {
                this->mutex.lock();
                // loop through all keys to check whether any key is pressed
                for(size_t i = 0; i < 256; i++)
                {
                    // temp variable can take values from range [0, 255)
                    tempVar = (int)keyboardKeyStates[i];
                    // right shift to get higher-order bit value
                    tempVar >>= 7;
                    // if the high-order bit is 1, the key is down
                    if (tempVar == 1)
                    {
                        if (this->currentStatus != STATUSES::ONLINE)
                        {
                            this->setStatus(STATUSES::ONLINE);
                        }
                        this->restartIdleTimer();
                        break;
                    }
                    // print state of current key
                    // std::cout<<temp;
                }
                // std::cout << std::endl;
                // perform action keystroke checks every 100 ms
                this->waitCond->wait(&this->mutex, 100);
                this->mutex.unlock();
            }
        }
        if (this->idleTimer != nullptr)
        {
            this->idleTimer->stop();
            delete this->idleTimer;
        }
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
}


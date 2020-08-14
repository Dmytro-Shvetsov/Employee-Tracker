#pragma once

#include <iostream>
#include <QMutex>
#include <QWaitCondition>
#include <QWidget>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QCoreApplication>
#include "definitions.h"
#ifdef WIN32
#include <windows.h>
#endif

namespace ETClient
{
    class ConnectionStatusManager : public QObject
    {
        Q_OBJECT
    private:
        bool running;
        QMutex mutex;
        QWaitCondition* waitCond;
        QTimer* idleTimer = nullptr;
        QElapsedTimer stopwatch;
        qint32 minMsForIdle = MIN_IDLE_TIME_SECONDS * 1000;
        qint8 currentStatus;

    public:
        explicit ConnectionStatusManager(QWaitCondition* waitCond, QObject* parent = nullptr, bool running = false);
        ~ConnectionStatusManager();
        void restartIdleTimer();
        void setRunning(bool value);
        void run();
        void setStatus(qint8 statusCode);
    signals:
        void statusChanged(const qint8& newStatus);
    };
}


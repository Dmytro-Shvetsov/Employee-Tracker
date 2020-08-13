#pragma once

#include <iostream>
#include <QWidget>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include "definitions.h"

namespace ETClient
{
    class ConnectionStatusManager : public QObject
    {
        Q_OBJECT
    public:
    private:
        QTimer idleTimer;
        QElapsedTimer stopwatch;
        qint32 minMsForIdle = 5000;
        qint8 currentStatus;
        bool eventFilter(QObject* obj, QEvent* event);
    public:
        explicit ConnectionStatusManager(QObject* parent = nullptr);
        ~ConnectionStatusManager();
        void restartIdleTimer();
        void setStatus(qint8 statusCode);
    signals:
        void statusChanged(const qint8& newStatus);
    };
}


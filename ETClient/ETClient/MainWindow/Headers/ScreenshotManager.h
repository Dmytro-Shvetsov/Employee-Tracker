#pragma once

#include <QWidget>
#include <QWindow>
#include <QDebug>
#include <QScreen>
#include <QThread>
#include <QBuffer>
#include <QApplication>
#include <QMutex>
#include <QWaitCondition>
#include <QPixmap>
#include <QLabel>
#include "definitions.h"


namespace ETClient
{
    class ScreenshotManager : public QObject
    {
        Q_OBJECT
    private:
        bool running;
        QMutex mutex;
        QWaitCondition* waitCond;
        QByteArray screenshotBytes;
        QWindow* windowObj;
        QSize defaultScreenshotSize = DEFAULT_SCREENSHOT_SIZE;
        qint32 screenshotTimedeltaSeconds = SCREENSHOT_TIMEDELTA_SECONDS;
    public:
        explicit ScreenshotManager(QWaitCondition* waitCond, QObject* parent = nullptr,
                                   QWindow* windowObj = nullptr, bool running = false);
        ~ScreenshotManager();
        void newScreenshot();
        QByteArray getScreenshot()const;
        void setRunning(bool value);
        void run();
    signals:
        void noScreenDetected();
        void screenshotReady();
    };
}


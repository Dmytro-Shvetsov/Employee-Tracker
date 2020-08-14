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
        QSize defaultScreenshotSize;
        qint32 screenshotTimedeltaSeconds;
    public:
        explicit ScreenshotManager(QWaitCondition* waitCond, QObject* parent = nullptr,
                                   QWindow* windowObj = nullptr,
                                   QSize defaultScreenshotSize=QSize(1324, 720),
                                   qint32 screenshotTimedeltaSeconds=600, // 10 mins
                                   bool running = false);
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


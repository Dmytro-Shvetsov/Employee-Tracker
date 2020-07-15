#include "MainWindow/Headers/ScreenshotManager.h"

namespace ETClient
{
    ScreenshotManager::ScreenshotManager(QWaitCondition* waitCond, QObject* parent, QWindow* windowObj,
                                         QSize defaultScreenshotSize, qint32 screenshotTimedeltaSeconds,
                                         bool running):
        QObject(parent),
        waitCond(waitCond),
        windowObj(windowObj),
        defaultScreenshotSize(defaultScreenshotSize),
        screenshotTimedeltaSeconds(screenshotTimedeltaSeconds),
        running(running)
    {

    }

    ScreenshotManager::~ScreenshotManager()
    {
        delete this->windowObj;
    }

    void ScreenshotManager::newScreenshot()
    {
        QScreen* screen = QApplication::primaryScreen();
        if (this->windowObj)
        {
            screen = this->windowObj->screen();
        }
        if (!screen)
        {
            qDebug() << "Screenshot Created";
            emit this->noScreenDetected();
            return;
        }

        // Grabbing screenshot from windod with windowId 0.
        QPixmap originalPixmap = screen->grabWindow(0).scaled(this->defaultScreenshotSize,
                                                              Qt::KeepAspectRatio,
                                                              Qt::SmoothTransformation);
//        this->screenshotBytes.clear();
        QByteArray byteArray;
//        QBuffer buffer(&this->screenshotBytes);
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        originalPixmap.save(&buffer, "JPG", 80);

        this->screenshotBytes = byteArray.toBase64();

        emit this->screenshotReady();
    }

    QByteArray ScreenshotManager::getScreenshot()const
    {
        return this->screenshotBytes;
    }

    void ScreenshotManager::setRunning(bool value)
    {
        this->running = value;
    }

    void ScreenshotManager::run()
    {
        qDebug() << "Starting execution";
        while(this->running)
        {
            this->mutex.lock();

            qDebug() << "Started creating screenshot on thread " << QThread::currentThread();

            this->newScreenshot();
            this->waitCond->wait(&this->mutex, this->screenshotTimedeltaSeconds * 1000);
            mutex.unlock();
        }
        QThread::currentThread()->exit();
    }

}


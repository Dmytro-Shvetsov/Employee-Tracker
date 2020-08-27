#include "ScreenshotManager.h"

namespace ETClient
{
    ScreenshotManager::ScreenshotManager(QWaitCondition* waitCond, QObject* parent, QWindow* windowObj, bool running):
        QObject(parent),
        windowObj(windowObj),
        running(running),
        waitCond(waitCond)
    {
    }

    ScreenshotManager::~ScreenshotManager()
    {
        this->windowObj = nullptr;
        this->waitCond = nullptr;
        qDebug() << "Deleted screenshot manager";
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
            qDebug() << "Error detecting screen";
            emit this->noScreenDetected();
            return;
        }

        // Grabbing screenshot from window with windowId 0.
        QPixmap originalPixmap = screen->grabWindow(0).scaled(this->defaultScreenshotSize,
                                                              Qt::KeepAspectRatio,
                                                              Qt::SmoothTransformation);

        QByteArray byteArray;
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

        // initial pause not to make screenshot at once
        this->mutex.lock();
        this->waitCond->wait(&this->mutex, this->screenshotTimedeltaSeconds * 1000);
        this->mutex.unlock();

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


#include "MainWindow/Headers/ScreenshotManager.h"

namespace ETClient
{
    ScreenshotManager::ScreenshotManager(QObject* parent, QWindow* windowObj,
                                         quint32 screenshotTimedeltaSeconds,
                                         QSize defaultScreenshotSize):

        QObject(parent),
        windowObj(windowObj),
        screenshotTimedeltaSeconds(screenshotTimedeltaSeconds),
        defaultScreenshotSize(defaultScreenshotSize)
    {

    }

    ScreenshotManager::~ScreenshotManager()
    {

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
        originalPixmap = screen->grabWindow(0).scaled(this->defaultScreenshotSize);
        emit this->screenshotReady();
    }

    QPixmap& ScreenshotManager::getScreenshot()
    {
        qDebug() << QThread::currentThread();
        return this->originalPixmap;
    }

    void ScreenshotManager::setExecutingScreenshotCreationLoop(bool value)
    {
        this->executingScreenshotCreationLoop = value;
    }

    void ScreenshotManager::run()
    {
        while (this->executingScreenshotCreationLoop)
        {
            qDebug() << "Started creating screenshot on thread "
                     << QThread::currentThread();
            this->newScreenshot();
            QThread::currentThread()->sleep(this->screenshotTimedeltaSeconds);
        }
    }
}


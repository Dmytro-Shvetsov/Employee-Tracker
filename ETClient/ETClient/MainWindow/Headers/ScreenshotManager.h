#pragma once

#include <QWidget>
#include <QWindow>
#include <QDebug>
#include <QScreen>
#include <QThread>
#include <QApplication>
#include <QPixmap>


namespace ETClient
{
    class ScreenshotManager : public QObject
    {
        Q_OBJECT
    private:
        QPixmap originalPixmap;
        quint32 screenshotTimedeltaSeconds;
        QWindow* windowObj;
        bool executingScreenshotCreationLoop;
        QSize defaultScreenshotSize;
    public:
        explicit ScreenshotManager(QObject* parent = nullptr, QWindow* windowObj = nullptr,
                                   quint32 screenshotTimedeltaSeconds=5, QSize defaultScreenshotSize=QSize(600, 800));
        ~ScreenshotManager();
        void newScreenshot();
        QPixmap& getScreenshot();
        void setExecutingScreenshotCreationLoop(bool value);
        void run();
    signals:
        void noScreenDetected();
        void screenshotReady();
    };
}


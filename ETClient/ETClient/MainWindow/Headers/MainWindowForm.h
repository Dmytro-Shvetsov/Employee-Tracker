#pragma once

#include <QMainWindow>
#include <QObject>
#include <QDebug>
#include <QDate>
#include <QMovie>
#include <QLocale>
#include <QCloseEvent>
#include "definitions.h"
#include "ui_MainWindowForm.h"

namespace Ui
{
    class MainWindowForm;
}

namespace ETClient
{
    class IMainWindowForm
    {
    public:
        virtual ~IMainWindowForm() {};
        virtual void initUiComponents() = 0;
        virtual void showView() = 0;
        virtual void hideView() = 0;
        virtual QWindow* getWindowObj() = 0;
        virtual void setUsernameText(const QString& username) = 0;
        virtual void setDateJoined(const QDate& date) = 0;
        virtual void setStatus(const qint8&) = 0;
        virtual void setUserImage(const QPixmap& img) = 0;
        virtual void setLoadingState(bool value) = 0;
    public: // signals
        virtual void logout(const QString& message = "") = 0;
        virtual void windowClosed(QCloseEvent* event) = 0;
    };

    class MainWindowForm : public QMainWindow, public IMainWindowForm
    {
        Q_OBJECT
    private:
        Ui::MainWindowForm* ui;
        QLabel* loadingView;
        QMovie* loadingMovie;
        QLocale locale = QLocale::English;

        void closeEvent(QCloseEvent* event)override;
        void hideEvent(QHideEvent* event)override;
    private slots:
        void onLogoutClick();
//        void onWindowClosed();
    public:
        explicit MainWindowForm(QWidget* parent = nullptr);
        virtual ~MainWindowForm();
        void initUiComponents()override;
        void showView()override;
        void hideView()override;
        QWindow* getWindowObj()override;
        void setUsernameText(const QString& username)override;
        void setDateJoined(const QDate& date)override;
        void setStatus(const qint8&)override;
        void setUserImage(const QPixmap& img)override;
        void setLoadingState(bool value)override;

    signals:
        void logout(const QString& message="")override;
        void windowClosed(QCloseEvent* event)override;
    };
}


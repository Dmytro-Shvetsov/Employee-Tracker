#pragma once

#include <QMainWindow>
#include <QObject>
#include <QDebug>
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
        virtual void initUiComponents() = 0;
        virtual void showView() = 0;
        virtual void hideView() = 0;
        virtual QWindow* getWindowObj() = 0;
        virtual ~IMainWindowForm() {};
    };

    class MainWindowForm : public QMainWindow, public IMainWindowForm
    {
        Q_OBJECT
    private:
        Ui::MainWindowForm* ui;
    private slots:
        void onLogoutClick();
    public:
        explicit MainWindowForm(QWidget* parent = nullptr);
        virtual ~MainWindowForm();
        void initUiComponents()override;
        void showView()override;
        void hideView()override;
        QWindow* getWindowObj() override;
    signals:
        void logout();
    };
}


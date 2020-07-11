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
        virtual void showView() = 0;
        virtual void hideView() = 0;
        virtual ~IMainWindowForm() {};

    };

    class MainWindowForm : public QMainWindow, public IMainWindowForm
    {
        Q_OBJECT
    private:
        Ui::MainWindowForm* ui;

    public:
        explicit MainWindowForm(QWidget* parent = nullptr);
        void showView()override;
        void hideView()override;
        ~MainWindowForm();
    };
}


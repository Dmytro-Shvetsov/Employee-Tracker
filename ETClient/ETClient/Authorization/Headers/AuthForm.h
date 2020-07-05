#pragma once

#include <QWidget>
#include "ui_AuthForm.h"


namespace Ui
{
    class AuthForm;
}

namespace ETClient
{
    class IAuthForm
    {
    public: // signals

    };

    class AuthForm : public QWidget, public IAuthForm
    {
    Q_OBJECT
    private:
        Ui::AuthForm* ui;
    public:
        explicit AuthForm(QWidget* parent = nullptr);
        ~AuthForm();
    };
}

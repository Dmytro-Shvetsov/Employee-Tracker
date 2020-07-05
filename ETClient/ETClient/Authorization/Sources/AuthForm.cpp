#include "Authorization/Headers/AuthForm.h"

namespace ETClient
{
    AuthForm::AuthForm(QWidget* parent) :
        QWidget(parent)
    {
        this->ui = new Ui::AuthForm();
        this->ui->setupUi(this);
    }
    AuthForm::~AuthForm()
    {
        delete this->ui;
    }
}




#include "Authorization/Headers/AuthForm.h"

namespace ETClient
{
    AuthForm::AuthForm(QWidget* parent) :
        QWidget(parent)
    {
        this->ui = new Ui::AuthForm();
        this->ui->setupUi(this);

        QWidget::setWindowTitle(APPLICATION_TITLE);
        this->setWindowIcon(QIcon(":/Resources/icon.ico"));
        this->setStyleSheet(
                    "background-color: #333;" \
                    "color:white;"
                    );

        this->setFixedSize(this->size());

        QPixmap lockImg(":/Resources/auth_lock.jpg");
        this->ui->imageLabel->setPixmap(
                    lockImg.scaled(this->ui->imageLabel->size(),
                                   Qt::KeepAspectRatio)
                    );

        this->ui->alertMessage->setStyleSheet("color:red;");

        this->ui->loginButton->setStyleSheet(
                    "background-color: rgba(219, 219, 219, 0);" \
                    "color: black;");

        connect(this->ui->usernameLineEdit,
                SIGNAL(textEdited(const QString&)),
                this,
                SLOT(onTextInput()));

        connect(this->ui->passwordLIneEdit,
                SIGNAL(textEdited(const QString&)),
                this,
                SLOT(onTextInput()));

        connect(this->ui->loginButton,
                SIGNAL(clicked()),
                this,
                SLOT(onLoginBtnClicked()));
    }
    AuthForm::~AuthForm()
    {
        qDebug() << "Deleted AuthForm";
        delete this->ui;
    }

    QString AuthForm::getInputUsername()
    {
        return this->ui->usernameLineEdit->text();
    }

    QString AuthForm::getInputPassword()
    {
        return this->ui->passwordLIneEdit->text();
    }

    void AuthForm::showView()
    {
        this->show();
    }

    void AuthForm::onLoginBtnClicked()
    {
        emit this->loginBtnClicked();
    }

    void AuthForm::onTextInput()
    {
        qDebug() << "TextChanged";
        emit this->textInput();
    }

    void AuthForm::setAlertMessage(const QString& text)
    {
        this->ui->alertMessage->setText(text);
    }

    void AuthForm::setLoginButtonActive(bool value)
    {
        this->ui->loginButton->setEnabled(value);
        if (value)
        {
            this->ui->loginButton->setStyleSheet(
                        "color: black;" \
                        "background-color: rgba(242, 242, 242, 1.0);");
        }
        else
        {
            this->ui->loginButton->setStyleSheet(
                        "color: gray;" \
                        "background-color: rgba(219, 219, 219, 0.3);");
        }
    }
}




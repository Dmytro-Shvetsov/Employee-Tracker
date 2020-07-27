#include "AuthForm.h"


namespace ETClient
{
    AuthForm::AuthForm(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::AuthForm())
    {
    }
    AuthForm::~AuthForm()
    {
        delete this->ui;
        qDebug() << "Deleted AuthForm";
    }

    void AuthForm::initUiComponents()
    {
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

        this->ui->alertFrame->setStyleSheet("color:red;");
        this->ui->alertMessage->setAlignment(Qt::AlignCenter);

        this->ui->loginButton->setStyleSheet(
                    "background-color: rgba(219, 219, 219, 0);" \
                    "color: black;");

        connect(this->ui->usernameLineEdit,
                SIGNAL(textEdited(const QString&)),
                this,
                SLOT(onTextInput()));

        connect(this->ui->passwordLineEdit,
                SIGNAL(textEdited(const QString&)),
                this,
                SLOT(onTextInput()));

        connect(this->ui->loginButton,
                SIGNAL(clicked()),
                this,
                SLOT(onLoginBtnClicked()));
    }

    QString AuthForm::getInputUsername()
    {
        return this->ui->usernameLineEdit->text();
    }

    QString AuthForm::getInputPassword()
    {
        return this->ui->passwordLineEdit->text();
    }

    void AuthForm::setInputUsername(const QString& value)
    {
        this->ui->usernameLineEdit->setText(value);
    }

    void AuthForm::setInputPassword(const QString &value)
    {
        this->ui->passwordLineEdit->setText(value);
    }

    void AuthForm::showView()
    {
        this->show();
    }

    void AuthForm::hideView()
    {
        this->hide();
    }

    void AuthForm::onLoginBtnClicked()
    {
        emit this->loginBtnClicked();
    }

    void AuthForm::onTextInput()
    {
        emit this->textInput();
    }

    void AuthForm::setAlertMessage(const QString& text)
    {
        this->ui->alertFrame->setVisible(text != "");
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

    bool AuthForm::rememberMeChecked()
    {
        return this->ui->rememberMeChbox->isChecked();
    }

    void AuthForm::setRememberMeChecked(bool value)
    {
        this->ui->rememberMeChbox->setChecked(value);
    }
}




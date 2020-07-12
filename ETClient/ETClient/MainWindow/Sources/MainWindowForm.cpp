#include "MainWindow/Headers/MainWindowForm.h"

namespace ETClient
{
    MainWindowForm::MainWindowForm(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindowForm)
    {
    }

    MainWindowForm::~MainWindowForm()
    {
        qDebug() << "Deleted MainWindowForm";
        delete this->ui;
    }

    void MainWindowForm::initUiComponents()
    {
        this->ui->setupUi(this);
        this->setCentralWidget(this->ui->contentFrame);
        this->setFixedSize(this->size());

        QWidget::setWindowTitle(APPLICATION_TITLE);
        this->setWindowIcon(QIcon(":/Resources/icon.ico"));
        this->setStyleSheet(
                    "background-color: #333;" \
                    "color:white;"
                    );
        this->ui->greetingLabel->setText("a");
        this->ui->logoutButton->setStyleSheet(
                    "color: black;" \
                    "background-color: rgba(242, 242, 242, 1.0);");

        connect(this->ui->logoutButton,
                SIGNAL(clicked()),
                this,
                SLOT(onLogoutClick()));
    }

    void MainWindowForm::showView()
    {
        this->show();
    }

    void MainWindowForm::hideView()
    {
        this->hide();
    }

    QWindow *MainWindowForm::getWindowObj()
    {
        return this->windowHandle();
    }

    void MainWindowForm::onLogoutClick()
    {
        emit this->logout();
    }
}

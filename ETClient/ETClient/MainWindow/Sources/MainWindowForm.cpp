#include "MainWindowForm.h"

namespace ETClient
{
    MainWindowForm::MainWindowForm(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindowForm),
        loadingView(nullptr),
        loadingMovie(new QMovie(":/Resources/loading.gif"))
    {
    }

    MainWindowForm::~MainWindowForm()
    {
        if (this->loadingView != nullptr)
        {
            delete this->loadingView;
        }

        delete this->loadingMovie;
        delete this->ui;
        qDebug() << "Deleted MainWindowForm";
    }

    void MainWindowForm::initUiComponents()
    {
        this->ui->setupUi(this);
        this->setCentralWidget(this->ui->contentFrame);
        this->setFixedSize(this->size());

        this->setLoadingState(true);

        QWidget::setWindowTitle(APPLICATION_TITLE);
        this->setWindowIcon(QIcon(":/Resources/icon.ico"));
        this->setStyleSheet(
                    "background-color: #333;" \
                    "color:white;"
                    );
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
        this->setEnabled(true);
    }

    QWindow* MainWindowForm::getWindowObj()
    {
        return this->windowHandle();
    }

    void MainWindowForm::setUsernameText(const QString& username)
    {
        this->ui->greetingLabel->setText("Hi, " + username);
    }

    void MainWindowForm::setDateJoined(const QDate& date)
    {
        this->ui->memberSinceLabel->setText(
                    "Member since " + this->locale.toString(date, "dd MMMM yyyy")
                );
    }

    void MainWindowForm::setStatus(const qint8& newStatus)
    {
        QLabel* status = this->ui->statusValue;
        switch (newStatus)
        {
        case STATUSES::ONLINE:
        {
            status->setText("online");
            status->setStyleSheet("color:#1eff00;");
            break;
        }
        case STATUSES::OFFLINE:
        {
            status->setText("offline");
            status->setStyleSheet("color:red");
            break;
        }
        case STATUSES::IDLE:
        {
            status->setText("idle");
            status->setStyleSheet("color:#fffb00;");
            break;
        }
        default:
        {
            qDebug() << "Warning! unknown status code " << newStatus <<
                        " passed to MainWindowForm::setStatus. No changes applied";
        }
        }
    }

    void MainWindowForm::setUserImage(const QPixmap& img)
    {
        qDebug() << "size " << this->ui->userImageLabel->size();
        this->ui->userImageLabel->setPixmap(
                    img.scaled(this->ui->userImageLabel->size(), Qt::KeepAspectRatio)
                    );
    }

    void MainWindowForm::setLoadingState(bool value)
    {
        this->ui->contentFrame->setEnabled(!value);
        if (value)
        {
            this->loadingView = new QLabel(this);
            this->loadingView->setStyleSheet("background-color: white;");
            this->loadingView->resize(this->size());
            this->loadingView->setAlignment(Qt::AlignCenter);
            this->loadingView->setMovie(this->loadingMovie);
            this->loadingMovie->start();
        }
        else
        {
            delete this->loadingView;
            this->loadingView = nullptr;
        }
        qDebug() << "Loading: " << value;
    }

    void MainWindowForm::closeEvent(QCloseEvent* event)
    {
        emit this->windowClosed(event);
    }

    void MainWindowForm::hideEvent(QHideEvent *event)
    {
        event->accept();
        this->setEnabled(true);

    }

    void MainWindowForm::onLogoutClick()
    {
        emit this->logout();
    }
}

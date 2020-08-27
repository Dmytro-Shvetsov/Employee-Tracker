#include "MainWindowForm.h"

namespace ETClient
{
    MainWindowForm::MainWindowForm(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindowForm),
        loadingView(nullptr),
        loadingMovie(new QMovie(":/Resources/loading.gif"))
    {
        this->trayIcon = new QSystemTrayIcon(this);
        this->trayIcon->setIcon(this->style()->standardIcon(QStyle::SP_ComputerIcon));
        this->trayIcon->setToolTip("Employee Tracker");
        // Create a context menu for the tray icon
        QMenu* menu = new QMenu(this);
        QAction* quitAction = new QAction("Exit", this);

        connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

        menu->addAction(quitAction);

        this->trayIcon->setContextMenu(menu);

        // connect clicking on the icon to the signal processor of this press
        connect(this->trayIcon,
                SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this,
                SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));
    }

    MainWindowForm::~MainWindowForm()
    {
        if (this->loadingView != nullptr)
        {
            delete this->loadingView;
        }

        delete this->loadingMovie;
        if (this->idleAlert != nullptr)
        {
            delete this->idleAlert;
        }
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

        this->idleAlert = new QMessageBox(QMessageBox::Icon::Warning,
                                          "Are you still there?",
                                          "You have gone idle.\nPress OK to continue.",
                                          QMessageBox::Ok);

        QPixmap warningIcon(":/Resources/icon.ico");
        idleAlert->setWindowIcon(warningIcon);
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

    QString MainWindowForm::setStatus(const qint8& newStatus)
    {
        QLabel* status = this->ui->statusValue;
        QString statusTxt;
        switch (newStatus)
        {
        case STATUSES::ONLINE:
        {
            statusTxt = "online";
            status->setText(statusTxt);
            status->setStyleSheet("color:#1eff00;");

            QIcon icon(":/Resources/user_online.ico");
            this->trayIcon->setIcon(icon);
            break;
        }
        case STATUSES::OFFLINE:
        {
            statusTxt = "offline";
            status->setText(statusTxt);
            status->setStyleSheet("color:red");

            QIcon icon(":/Resources/user_offline.ico");
            this->trayIcon->setIcon(icon);
            break;
        }
        case STATUSES::IDLE:
        {
            statusTxt = "idle";
            status->setStyleSheet("color:#fffb00;");
            status->setText(statusTxt);

            QIcon icon(":/Resources/user_idle.ico");
            this->trayIcon->setIcon(icon);

            if (this->idleAlert->isVisible() == false)
            {
                QTimer::singleShot(1000, this, [this](){
                    if (this->trayIcon->isVisible())
                    {
                        this->showView();
                    }
                    this->idleAlert->exec();
                });
            }
            break;
        }
        default:
        {
            qDebug() << "Warning! unknown status code " << newStatus <<
                        " passed to MainWindowForm::setStatus. No changes applied";
        }
        }
        return statusTxt;
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

    int MainWindowForm::showErrorMessage(const QString& message)
    {
        return QMessageBox::critical(this, "An error occured", message, QMessageBox::Ok);
    }

    void MainWindowForm::closeEvent(QCloseEvent* event)
    {
        emit this->windowClosed(event);
    }

    void MainWindowForm::hideEvent(QHideEvent* event)
    {
        if (this->isVisible() == false)
        {
            return;
        }
        this->hideView();
        this->trayIcon->show();
        this->trayIcon->showMessage("Employee Tracker",
                                    "Program was minimized to system tray.");
    }

    void MainWindowForm::onLogoutClick()
    {
        emit this->logout();
    }

    void MainWindowForm::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
    {
        switch (reason)
        {
        case QSystemTrayIcon::ActivationReason::DoubleClick:
        {
            this->showView();
            this->raise();
            this->setFocus();
            this->trayIcon->hide();
            break;
        }
        default:
        {
            break;
        }
        }
    }
}

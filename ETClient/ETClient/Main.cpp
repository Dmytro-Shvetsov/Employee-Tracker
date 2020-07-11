#include <QApplication>
#include <QLabel>
#include <QtDebug>
#include <QWidget>
#include "Authorization/Headers/AuthPresenter.h"

using namespace ETClient;

int main(int argc, char* argv[])
{
    // for memory leak checking
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//    QSettings q("Content Innovations", "Employee Tracker");
//    q.setValue("a", "1");
//    q.sync();
//    qDebug() << q.value("a").toString();
//    q.clear();

    QApplication app(argc, argv);

    qDebug("Hello console");
    AuthPresenter authPresenter;
    return app.exec();
}



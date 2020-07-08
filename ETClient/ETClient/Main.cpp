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

    QApplication app(argc, argv);
//    qDebug() << "App path : " << qApp->applicationDirPath();
    qDebug("Hello console");
    AuthForm* authView = new AuthForm;
    AuthPresenter authPresenter(authView);
    authPresenter.show();

    return app.exec();
}



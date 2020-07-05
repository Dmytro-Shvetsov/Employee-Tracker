#include <QApplication>
#include <QLabel>
#include <QtDebug>
#include <QWidget>
#include "Authorization/Headers/AuthForm.h"

using namespace ETClient;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    qDebug("Hello console");
    AuthForm auth;
    auth.show();
    return app.exec();
}



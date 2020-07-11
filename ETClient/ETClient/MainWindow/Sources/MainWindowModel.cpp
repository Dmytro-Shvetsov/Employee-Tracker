#include "MainWindow/Headers/MainWindowModel.h"


namespace ETClient
{
    MainWindowModel::MainWindowModel(QObject* parent):
        QObject(parent)
    {

    }

    MainWindowModel::~MainWindowModel()
    {
        qDebug() << "Deleted MainWindowModel";
    }
}

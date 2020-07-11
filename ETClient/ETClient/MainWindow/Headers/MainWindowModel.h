#pragma once

#include <QObject>
#include <QDebug>


namespace ETClient
{
    class MainWindowModel : public QObject
    {
        Q_OBJECT
    public:
        explicit MainWindowModel(QObject* parent = nullptr);
        ~MainWindowModel();
    };
}


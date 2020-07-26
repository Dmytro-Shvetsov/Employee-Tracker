#include <QApplication>
#include <QLabel>
#include <QtDebug>
#include <QWidget>
#include "AuthPresenter.h"
#include "NetworkManager.h"

using namespace ETClient;

int main(int argc, char* argv[])
{
    // for memory leak checking
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

//    QApplication app(argc, argv);

    std::string interfaceNameOrIP = "192.168.0.106";
    NetworkManager nm(interfaceNameOrIP);
    nm.run();

//    AuthPresenter authPresenter;
//    qDebug() << "Starting program";
//    return app.exec();
}
/**
 * SSLAnalyzer application
 * ========================
 * This application analyzes SSL/TLS traffic and presents detailed and diverse information about it. It can operate in live traffic
 * mode where this information is collected on live packets or in file mode where packets are being read from a pcap/pcapng file. The
 * information collected by this application includes:
 * - general data: number of packets, packet rate, amount of traffic, bandwidth
 * - flow data: number of flow, flow rate, average packets per flow, average data per flow
 * - SSL/TLS data: number of client-hello and server-hello messages, number of flows ended with successful handshake,
 *   number of flows ended with SSL alert
 * - hostname map (which hostnames were used and how much. Taken from the server-name-indication extension in the
 *   client-hello message)
 * - cipher-suite map (which cipher-suites were used and how much)
 * - SSL/TLS versions map (which SSL/TLS versions were used and how much)
 * - SSL/TLS ports map (which SSL/TLS TCP ports were used and how much)
 *
 * For more details about modes of operation and parameters run SSLAnalyzer -h
 */
#include <QCoreApplication>
#include <iostream>



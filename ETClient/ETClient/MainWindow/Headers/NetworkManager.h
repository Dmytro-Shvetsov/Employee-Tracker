#pragma once

#include "definitions.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <QDebug>
#include <QFile>

#if !defined(WIN32) && !defined(WINx64) && !defined(PCAPPP_MINGW_ENV)  // for using ntohl, ntohs, etc.
#include <in.h>
#endif
#include <PcapLiveDeviceList.h>
#include <PcapFilter.h>
#include <TablePrinter.h>
#include <PlatformSpecificUtils.h>
#include <NetworkUtils.h>
#include <PcapPlusPlusVersion.h>

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QVector>
#include "HttpStatsCollector.h"
#include "SSLStatsCollector.h"


#define PRINT_STAT_HEADLINE(description) \
        qDebug("\n %s \n--------------------\n", description)

#define DEFAULT_CALC_RATES_PERIOD_SEC 2


using namespace pcpp;

namespace ETClient
{
    /**
     * The struct that is passed as a cookie into NetworkManager::packetArrive() callback when capturing packets.
     */
    struct PacketArrivedData
    {
        SSLStatsCollector* sslStatsCollector;
        HttpStatsCollector* httpStatsCollector;
        std::string interfaceInUseIP;

        PacketArrivedData()
        {
            this->httpStatsCollector = new HttpStatsCollector(80);
            this->sslStatsCollector = new SSLStatsCollector;
        }
        ~PacketArrivedData()
        {
            delete this->sslStatsCollector;
            delete this->httpStatsCollector;
        }

        void clear()
        {
            this->sslStatsCollector->clear();
            this->httpStatsCollector->clear();
        }

        void tryCollectStats(Packet* parsedPacket)
        {
            // if the stats collector doesn't parse a packet, it returns false
            bool collectStatus = (this->sslStatsCollector->tryCollectStats(parsedPacket)
                || this->httpStatsCollector->tryCollectStats(parsedPacket));
        }
    };
    class NetworkManager : public QObject
    {
        Q_OBJECT
    private:
        QMutex mutex;
        QWaitCondition* waitCond;
        bool running = false;

        const std::vector<PcapLiveDevice*>& devList = PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
        pcpp::PcapLiveDevice* dev = nullptr;

        // set HTTP and SSL/TLS ports filter on the live device to capture only HTTP and SSL/TLS packets
        // currently SSLLayer support only listed ones.
        std::vector<uint16_t> allowedPorts = { 0, 80, 261, 443, 448, 465, 563, 614, 636, 989, 990, 992, 993, 994, 995 };

        std::vector<GeneralFilter*> portFilterRules;
        OrFilter* orFilter = nullptr;

        PacketArrivedData data;

        /**
         * An auxiliary method for sorting the string count map. Used in printServerNames() and in printCipherSuites()
         */
        static bool stringCountComparer(std::pair<QString, quint32> pair1, std::pair<QString, quint32> pair2);


        /**
         * An auxiliary method for GeneralFilter object from numeric port. Used in run()
         */
        static GeneralFilter* portToPortFilter(uint16_t p);


        /**
         * packet capture callback - called whenever a packet arrives
         */
        static void packetArrive(RawPacket* packet, PcapLiveDevice* dev, void* cookie);

        std::string readInterfaceNameOrIp()const;
    public:
        NetworkManager(QWaitCondition* waitCond, QObject* parent=nullptr);
        ~NetworkManager();
        /**
         * An auxiliary method for sorting the pairs of a map. Used in printxxx() methods
         */
        static std::vector<std::pair<QString, quint32>> sortHostnamesByFreq(std::map<QString, quint32>& map);

        bool setupDevice();
        void setRunning(bool value);
        PacketArrivedData& getData();
        /*
         * Start analyzing packets in live traffic mode
         */
        void run();

        /**
         * Go over all interfaces and output their names
         */
        void listInterfaces();

        /**
         * Print the method count table
         */
        static void printMethods(HttpRequestStats& reqStatscollector);

        /**
         * Print the hostname count map to a table sorted by popularity (most popular hostnames will be first)
         */
        static void printHostnames(HttpRequestStats& reqStatscollector);

        /**
         * Print the server-name count map to a table sorted by popularity (most popular names will be first)
         */
        static void printServerNames(ClientHelloStats& clientHelloStatsCollector);
        static void printUnknownHostNames(std::map<QString, quint32>& hostCount);

        /**
         * Print a summary of all statistics collected by the HttpStatsCollector. Should be called when traffic capture was finished
         */
        static void printStatsSummary(HttpStatsCollector& collector);

        /**
         * Print a summary of all statistics collected by the SSLStatsCollector. Should be called when traffic capture was finished
         */
        static void printStatsSummary(SSLStatsCollector& collector);
    signals:
        void dataReadyToExtract();
        void networkInterfaceNotConfigured();
    };
}

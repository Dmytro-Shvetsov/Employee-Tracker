#include "NetworkManager.h"

namespace ETClient
{
    NetworkManager::NetworkManager(QWaitCondition* waitCond, QObject* parent):
        QObject(parent),
        waitCond(waitCond)
    {
//        this->listInterfaces();
    }

    NetworkManager::~NetworkManager()
    {
        // waitCond should be deleted in parent class
        this->waitCond = nullptr;
        for(GeneralFilter* item: this->portFilterRules)
        {
            delete item;
        }
        delete this->orFilter;
        qDebug() << "Deleted network manager";
    }

    bool NetworkManager::setupDevice()
    {
        if (this->dev == nullptr)
        {
            const std::string interfaceNameOrIP = this->readInterfaceNameOrIp();
            // extract pcap live device by interface name or IP address
            IPv4Address interfaceIP(interfaceNameOrIP);
            if (interfaceIP.isValid())
            {
                this->dev = PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIP);
                if (this->dev == nullptr)
                {
                    qDebug() << "Couldn't find interface by provided IP";
                }
                else
                {
                    this->data.interfaceInUseIP = interfaceNameOrIP;
                }
            }
            else
            {
                this->dev = PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(interfaceNameOrIP);
                if (this->dev == nullptr)
                {
                    qDebug() << "Couldn't find interface by provided name";
                }
            }

            if (this->dev == nullptr)
            {
                return false;
            }

            std::transform(this->allowedPorts.begin(),
            this->allowedPorts.end(),
            std::back_inserter(this->portFilterRules), this->portToPortFilter);

            this->orFilter = new OrFilter(this->portFilterRules);

            // set the filter for the device
            if (!this->dev->setFilter(*this->orFilter))
            {
                std::string filterAsString;
                this->orFilter->parseToString(filterAsString);
                qDebug("Couldn't set the filter '%s' for the device", filterAsString.c_str());
            }
        }
        return true;
    }

    void NetworkManager::setRunning(bool value)
    {
        this->running = value;
    }

    PacketArrivedData& NetworkManager::getData()
    {
        return this->data;
    }

    void NetworkManager::run()
    {
        if (!this->setupDevice() || !this->dev->open())
        {
            emit this->networkInterfaceNotConfigured();
            return;
        }

        // start capturing and analyzing traffic
        this->dev->startCapture(this->packetArrive, &data);
        while (this->running)
        {
            this->mutex.lock();

            this->waitCond->wait(&this->mutex, 10000);
            // pause packet capturing
            this->dev->stopCapture();

            emit this->dataReadyToExtract();

            this->dev->startCapture(this->packetArrive, &data);
            this->mutex.unlock();
        }

        // stop capturing and close the live device
        this->dev->stopCapture();
        this->dev->close();

//        qDebug("\n\nSTATS SUMMARY\n");
//        qDebug("=============\n");
//        this->printStatsSummary(*this->data.httpStatsCollector);
//        this->printStatsSummary(*this->data.sslStatsCollector);
//        this->printUnknownHostNames(this->data.unknownHostCount);

        QThread::currentThread()->exit();
    }

    void NetworkManager::listInterfaces()
    {
        qDebug("Network interfaces:");
        for (std::vector<PcapLiveDevice*>::const_iterator iter = devList.begin(); iter != devList.end(); iter++)
        {
            qDebug("    -> Name: '%s'   IP address: %s\n", (*iter)->getName(), (*iter)->getIPv4Address().toString().c_str());
        }
    }

    void NetworkManager::printMethods(HttpRequestStats &reqStatscollector)
    {
        // create the table
        std::vector<std::string> columnNames;
        columnNames.push_back("Method");
        columnNames.push_back("Count");
        std::vector<int> columnsWidths;
        columnsWidths.push_back(9);
        columnsWidths.push_back(5);
        TablePrinter printer(columnNames, columnsWidths);

        // go over the method count table and print each method and count
        for (auto iter = reqStatscollector.methodCount.cbegin();
            iter != reqStatscollector.methodCount.cend();
            iter++)
        {
            std::stringstream values;

            switch (iter.key())
            {
            case HttpRequestLayer::HttpGET:
                values << "GET" << "|" << reqStatscollector.methodCount[HttpRequestLayer::HttpGET];
                printer.printRow(values.str(), '|');
                break;
            case HttpRequestLayer::HttpPOST:
                values << "POST" << "|" << reqStatscollector.methodCount[HttpRequestLayer::HttpPOST];
                printer.printRow(values.str(), '|');
                break;
            case HttpRequestLayer::HttpCONNECT:
                values << "CONNECT" << "|" << reqStatscollector.methodCount[HttpRequestLayer::HttpCONNECT];
                printer.printRow(values.str(), '|');
                break;
            case HttpRequestLayer::HttpDELETE:
                values << "DELETE" << "|" << reqStatscollector.methodCount[HttpRequestLayer::HttpDELETE];
                printer.printRow(values.str(), '|');
                break;
            case HttpRequestLayer::HttpHEAD:
                values << "HEAD" << "|" << reqStatscollector.methodCount[HttpRequestLayer::HttpHEAD];
                printer.printRow(values.str(), '|');
                break;
            case HttpRequestLayer::HttpOPTIONS:
                values << "OPTIONS" << "|" << reqStatscollector.methodCount[HttpRequestLayer::HttpOPTIONS];
                printer.printRow(values.str(), '|');
                break;
            case HttpRequestLayer::HttpPATCH:
                values << "PATCH" << "|" << reqStatscollector.methodCount[HttpRequestLayer::HttpPATCH];
                printer.printRow(values.str(), '|');
                break;
            case HttpRequestLayer::HttpPUT:
                values << "PUT" << "|" << reqStatscollector.methodCount[HttpRequestLayer::HttpPUT];
                printer.printRow(values.str(), '|');
                break;
            case HttpRequestLayer::HttpTRACE:
                values << "TRACE" << "|" << reqStatscollector.methodCount[HttpRequestLayer::HttpTRACE];
                printer.printRow(values.str(), '|');
                break;
            default:
                break;
            }
        }
    }

    bool NetworkManager::stringCountComparer(std::pair<QString, quint32> pair1, std::pair<QString, quint32> pair2)
    {
        return pair1.second > pair2.second;
    }

    std::vector<std::pair<QString, quint32>> NetworkManager::sortHostnamesByFreq(std::map<QString, quint32>& map)
    {
        std::vector<std::pair<QString, quint32>> map2vec(map.begin(), map.end());

        std::sort(map2vec.begin(), map2vec.end(), &NetworkManager::stringCountComparer);
        return map2vec;
    }

    GeneralFilter* NetworkManager::portToPortFilter(uint16_t p)
    {
        return new PortFilter(p, pcpp::SRC_OR_DST);
    };

    void NetworkManager::printHostnames(HttpRequestStats& reqStatscollector)
    {
        // create the table
        std::vector<std::string> columnNames;
        columnNames.push_back("Hostname");
        columnNames.push_back("Count");
        std::vector<int> columnsWidths;
        columnsWidths.push_back(40);
        columnsWidths.push_back(5);
        TablePrinter printer(columnNames, columnsWidths);

        // sort the hostname count map so the most popular hostnames will be first
        // since it's not possible to sort a std::map you must copy it to a std::vector and sort it then
        auto map2vec = sortHostnamesByFreq(reqStatscollector.hostnameCount);

        // go over all items (hostname + count) in the sorted vector and print them
        for (auto iter = map2vec.begin(); iter != map2vec.end(); iter++)
        {
            std::stringstream values;
            values << iter->first.toStdString() << "|" << iter->second;
            printer.printRow(values.str(), '|');
        }
    }

    void NetworkManager::printServerNames(ClientHelloStats& clientHelloStatsCollector)
    {
        // create the table
        std::vector<std::string> columnNames;
        columnNames.push_back("Hostname");
        columnNames.push_back("Count");
        std::vector<int> columnsWidths;
        columnsWidths.push_back(40);
        columnsWidths.push_back(5);
        TablePrinter printer(columnNames, columnsWidths);

        // sort the server-name count map so the most popular names will be first
        // since it's not possible to sort a std::map you must copy it to a std::vector and sort it then
        auto map2vec = sortHostnamesByFreq(clientHelloStatsCollector.serverNameCount);

        // go over all items (names + count) in the sorted vector and print them
        for (auto iter = map2vec.begin(); iter != map2vec.end(); iter++)
        {
            std::stringstream values;
            values << iter->first.toStdString() << "|" << iter->second;
            printer.printRow(values.str(), '|');
        }
    }

    void NetworkManager::printStatsSummary(HttpStatsCollector& collector)
    {
        PRINT_STAT_HEADLINE("HTTP request stats");
        qDebug("Number of HTTP requests %d",                collector.getRequestStats().numOfMessages);

        PRINT_STAT_HEADLINE("HTTP response stats");
        qDebug("Number of HTTP responses %d",                   collector.getResponseStats().numOfMessages);

        PRINT_STAT_HEADLINE("HTTP request methods");
        printMethods(collector.getRequestStats());

        PRINT_STAT_HEADLINE("Hostnames count");
        printHostnames(collector.getRequestStats());
    }

    void NetworkManager::printStatsSummary(SSLStatsCollector& collector)
    {
        qDebug("Number of client-hello messages %d", collector.getClientHelloStats().numOfMessages);
        qDebug("Number of server-hello messages %d", collector.getServerHelloStats().numOfMessages);

        PRINT_STAT_HEADLINE("Server-name count");
        printServerNames(collector.getClientHelloStats());
    }

    void NetworkManager::packetArrive(RawPacket *packet, PcapLiveDevice* dev, void *cookie)
    {
        PacketArrivedData* data = (PacketArrivedData*)cookie;

        // parse the packet
        Packet parsedPacket(packet);

        // give the packet to the collectors
        data->tryCollectStats(&parsedPacket);
    }

    std::string NetworkManager::readInterfaceNameOrIp()const
    {
        QFile file(NETWORK_INTERFACE_CONF_FILE);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return "";
        }

        QString content = file.readAll();
        file.close();
        return content.toStdString();
    }
}


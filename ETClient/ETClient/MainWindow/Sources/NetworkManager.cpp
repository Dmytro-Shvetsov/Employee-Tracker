#include "NetworkManager.h"

namespace ETClient
{
    NetworkManager::NetworkManager(std::string interfaceNameOrIP)
    {
    //        std::string interfaceNameOrIP = "192.168.0.106";
    //        std::string port = "80";
        // extract pcap live device by interface name or IP address

        IPv4Address interfaceIP(interfaceNameOrIP);
        if (interfaceIP.isValid())
        {
            this->dev = PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIP);
            if (this->dev == NULL)
            {
                qDebug() << "Couldn't find interface by provided IP";
            }
        }
        else
        {
            this->dev = PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(interfaceNameOrIP);
            if (this->dev == NULL)
            {
                qDebug() << "Couldn't find interface by provided name";
            }
        }
    }

    void NetworkManager::run()
    {
        if (!this->dev->open())
        {
            EXIT_WITH_ERROR("Could not open the device");
            throw std::exception("Network device was not initialized properly.");
        }

        std::vector<GeneralFilter*> portFilterVec;
        std::transform(this->sslPorts.begin(),
                       this->sslPorts.end(),
                       std::back_inserter(portFilterVec), portToPortFilter);

        OrFilter orFilter(portFilterVec);
        std::string f;
        orFilter.parseToString(f);
        std::cout << f;
         //set the filter for the device
        if (!dev->setFilter(orFilter))
        {
            std::string filterAsString;
            orFilter.parseToString(filterAsString);
            EXIT_WITH_ERROR("Couldn't set the filter '%s' for the device", filterAsString.c_str());
        }

        // start capturing packets and collecting stats
        PacketArrivedData data;
        data.httpStatsCollector = new HttpStatsCollector(80);
        data.sslStatsCollector = new SSLStatsCollector;

        // start capturing and analyzing traffic
        dev->startCapture(this->packetArrive, &data);


        for(int i = 0; i < 30; i++)
        {
            PCAP_SLEEP(DEFAULT_CALC_RATES_PERIOD_SEC);
            data.calcRates();
        }
        // stop capturing and close the live device
        dev->stopCapture();
        dev->close();

        // calculate final rates
        data.calcRates();
        system("cls");
        qDebug("\n\nSTATS SUMMARY\n");
        qDebug("=============\n");
        printStatsSummary(*data.httpStatsCollector);
        printStatsSummary(*data.sslStatsCollector);

        size_t nUnknownHosts = data.unknownHostCount.size();
        qDebug("Unknown host size: %d", nUnknownHosts);


        std::vector<std::pair<std::string, int>> map2vec(data.unknownHostCount.begin(), data.unknownHostCount.end());
        std::sort(map2vec.begin(), map2vec.end(), &hostnameComparer);

        std::vector<std::pair<std::string, int>>::iterator itBegin, itEnd;
        itBegin = itEnd = map2vec.begin();

        itEnd += MAX_UKNOWN_HOSTS_RESOLVE > nUnknownHosts ? nUnknownHosts : MAX_UKNOWN_HOSTS_RESOLVE;
        std::cout << "################ Unknown host names: ###################" << std::endl;
        for (; itBegin != itEnd; itBegin++)
        {
            auto pair = *itBegin;
            const char* resolvedIP = PacketArrivedData::resolveIPv4(pair.first.c_str());
            qDebug("%s -> %d",
                   (resolvedIP == nullptr ? pair.first.c_str() : resolvedIP),
                   pair.second);
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
        for (std::map<HttpRequestLayer::HttpMethod, int>::iterator iter = reqStatscollector.methodCount.begin();
            iter != reqStatscollector.methodCount.end();
            iter++)
        {
            std::stringstream values;

            switch (iter->first)
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

    bool NetworkManager::hostnameComparer(std::pair<std::string, int> first, std::pair<std::string, int> second)
    {
        return first.second > second.second;
    }

    bool NetworkManager::stringCountComparer(std::pair<std::string, int> first, std::pair<std::string, int> second)
    {
        return first.second > second.second;
    }

    bool NetworkManager::uint16CountComparer(std::pair<uint16_t, int> first, std::pair<uint16_t, int> second)
    {
        return first.second > second.second;
    }

    GeneralFilter* NetworkManager::portToPortFilter(uint16_t p)
    {
        return new PortFilter(p, pcpp::SRC_OR_DST);
    };

    void NetworkManager::printHostnames(HttpRequestStats &reqStatscollector)
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
        std::vector<std::pair<std::string, int> > map2vec(reqStatscollector.hostnameCount.begin(), reqStatscollector.hostnameCount.end());
        std::sort(map2vec.begin(), map2vec.end(), &this->hostnameComparer);

        // go over all items (hostname + count) in the sorted vector and print them
        for (std::vector<std::pair<std::string, int> >::iterator iter = map2vec.begin();
            iter != map2vec.end();
            iter++)
        {
            std::stringstream values;
            values << iter->first << "|" << iter->second;
            printer.printRow(values.str(), '|');
        }
    }

    void NetworkManager::printServerNames(ClientHelloStats &clientHelloStatsCollector)
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
        std::vector<std::pair<std::string, int> > map2vec(clientHelloStatsCollector.serverNameCount.begin(), clientHelloStatsCollector.serverNameCount.end());
        std::sort(map2vec.begin(), map2vec.end(), &stringCountComparer);

        // go over all items (names + count) in the sorted vector and print them
        for (std::vector<std::pair<std::string, int> >::iterator iter = map2vec.begin();
            iter != map2vec.end();
            iter++)
        {
            std::stringstream values;
            values << iter->first << "|" << iter->second;
            printer.printRow(values.str(), '|');
        }
    }

    void NetworkManager::printVersions(std::map<SSLVersion, int> &versionMap, std::string headline)
    {
        // create the table
        std::vector<std::string> columnNames;
        columnNames.push_back(headline);
        columnNames.push_back("Count");
        std::vector<int> columnsWidths;
        columnsWidths.push_back(28);
        columnsWidths.push_back(5);
        TablePrinter printer(columnNames, columnsWidths);

        // go over the status code map and print each item
        for (std::map<SSLVersion, int>::iterator iter = versionMap.begin();
            iter != versionMap.end();
            iter++)
        {
            std::stringstream values;
            values << iter->first << "|" << iter->second;
            printer.printRow(values.str(), '|');
        }
    }

    void NetworkManager::printStatsSummary(HttpStatsCollector &collector)
    {
        PRINT_STAT_HEADLINE("HTTP request stats");
        std::cout << "Number of HTTP requests " << collector.getRequestStats().numOfMessages << std::endl;
        std::cout << "Rate of HTTP requests(Requests/sec) " << collector.getRequestStats().messageRate.totalRate;
        std::cout << "Total data in headers(Bytes) " << collector.getRequestStats().totalMessageHeaderSize;
        std::cout << "Average header size(Bytes) " << collector.getRequestStats().averageMessageHeaderSize;

        PRINT_STAT_HEADLINE("HTTP response stats");
        std::cout << "Number of HTTP responses" << collector.getResponseStats().numOfMessages << std::endl;
        std::cout << "Rate of HTTP responses (Requests/sec)" << collector.getResponseStats().messageRate.totalRate << std::endl;
        std::cout << "Total data in headers(Bytes) " << collector.getResponseStats().totalMessageHeaderSize << std::endl;
        std::cout << "Total body size (may be compressed)(Bytes) " << collector.getResponseStats().totalConentLengthSize << std::endl;
        std::cout << "Average body size(Bytes) " << collector.getResponseStats().averageContentLengthSize << std::endl;

        PRINT_STAT_HEADLINE("HTTP request methods");
        printMethods(collector.getRequestStats());

        PRINT_STAT_HEADLINE("Hostnames count");
        printHostnames(collector.getRequestStats());

        /*PRINT_STAT_HEADLINE("Status code count");
        printStatusCodes(collector.getResponseStats());

        PRINT_STAT_HEADLINE("Content-type count");
        printContentTypes(collector.getResponseStats());*/
    }

    void NetworkManager::printStatsSummary(SSLStatsCollector &collector)
    {
        std::cout << "Client-hello message" << collector.getClientHelloStats().numOfMessages << std::endl;
        std::cout << "Server-hello message" << collector.getServerHelloStats().numOfMessages << std::endl;

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

    void NetworkManager::printCurrentRates(HttpStatsCollector &collector)
    {
        PRINT_STAT_HEADLINE("Current HTTP rates");
        std::cout << "Rate of HTTP requests(Requests/sec) "  << collector.getRequestStats().messageRate.currentRate << std::endl;
        std::cout << "Rate of HTTP responses(Requests/sec) " << collector.getResponseStats().messageRate.currentRate << std::endl;
    }

    void NetworkManager::printCurrentRates(SSLStatsCollector &collector)
    {
        PRINT_STAT_HEADLINE("Current SSL rates");
        std::cout << "Rate of SSL requests(Requests/sec) " << collector.getClientHelloStats().messageRate.currentRate << std::endl;
        std::cout << "Rate of SSL responses(Requests/sec) " << collector.getServerHelloStats().messageRate.currentRate << std::endl;
    }
}


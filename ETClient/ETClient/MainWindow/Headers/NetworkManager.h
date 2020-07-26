#include "definitions.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <QDebug>

#if !defined(WIN32) && !defined(WINx64) && !defined(PCAPPP_MINGW_ENV)  // for using ntohl, ntohs, etc.
#include <in.h>
#endif
#include <PcapLiveDeviceList.h>
#include <PcapFilter.h>
#include <TablePrinter.h>
#include <PlatformSpecificUtils.h>
#include <NetworkUtils.h>
#include <PcapPlusPlusVersion.h>

#include "HttpStatsCollector.h"
#include "SSLStatsCollector.h"


#define EXIT_WITH_ERROR(reason, ...) do { \
    printf("\nError: " reason "\n\n", ## __VA_ARGS__); \
    system("pause"); \
    } while(0)

#define PRINT_STAT_HEADLINE(description) \
        printf("\n" description "\n--------------------\n\n")

#define DEFAULT_CALC_RATES_PERIOD_SEC 2


using namespace pcpp;

namespace ETClient
{
    class NetworkManager
    {
    private:
        pcpp::PcapLiveDevice* dev = NULL;
        // set HTTP and SSL/TLS ports filter on the live device to capture only HTTP and SSL/TLS packets
        // currently SSLLayer support only listed ones.
        std::vector<uint16_t> sslPorts = { 0, 80, 261, 443, 448, 465, 563, 614, 636, 989, 990, 992, 993, 994, 995 };

        /**
         * The struct that is passed as a cookie into packetArrive() callback when capturing packets.
         */
        struct PacketArrivedData
        {
            SSLStatsCollector* sslStatsCollector;
            HttpStatsCollector* httpStatsCollector;
            std::string interfaceInUseIP;
            std::map<std::string, size_t> unknownHostCount; // a map for counting the hostnames seen in traffic

            ~PacketArrivedData()
            {
                delete this->sslStatsCollector;
                delete this->httpStatsCollector;
            }

            void calcRates()
            {
                this->sslStatsCollector->calcRates();
                this->httpStatsCollector->calcRates();
            }

            void tryCollectStats(Packet* parsedPacket)
            {
                // if the stats collector doesn't parse a packet, it returns false
                bool collectStatus = (this->sslStatsCollector->tryCollectStats(parsedPacket)
                    || this->httpStatsCollector->tryCollectStats(parsedPacket));
                // if none of the collectors have parsed the packet, just take the source/destination IP.
                if (!collectStatus && parsedPacket->isPacketOfType(pcpp::IPv4))
                {
                    std::string srcIP = parsedPacket->getLayerOfType<pcpp::IPv4Layer>()->getSrcIpAddress().toString();
                    std::string dstIP = parsedPacket->getLayerOfType<pcpp::IPv4Layer>()->getDstIpAddress().toString();
                    if (srcIP != this->interfaceInUseIP)
                    {
                        this->unknownHostCount[srcIP]++;
                    }
                    else if (dstIP != this->interfaceInUseIP)
                    {
                        this->unknownHostCount[dstIP]++;
                    }

                    qDebug("Source ip is '%s'; Dest ip is '%s'", srcIP.c_str(), dstIP.c_str());
                }
            }
            /**
             * Function to get host name by its IPv4 address
             */
            static const char* resolveIPv4(const char* ipStr)
            {
                struct in_addr ip;
                struct hostent* hp;

                if (!inet_pton(AF_INET, ipStr, &ip))
                {
                    // can't parse IP address
                    return nullptr;
                }

                if ((hp = gethostbyaddr(
                    (const char *)&ip,
                    sizeof ip, AF_INET)) == NULL)
                {
                    // no name associated with
                    return nullptr;
                }
                return hp->h_name;
            }
        };

        ///**
        // * Print the method count table
        // */
        void printMethods(HttpRequestStats& reqStatscollector);


        // * An auxiliary method for sorting the hostname count map. Used only in printHostnames()
        // */
        static bool hostnameComparer(std::pair<std::string, int> first, std::pair<std::string, int> second);

        /**
         * An auxiliary method for sorting the string count map. Used in printServerNames() and in printCipherSuites()
         */
        static bool stringCountComparer(std::pair<std::string, int> first, std::pair<std::string, int> second);

        /**
         * An auxiliary method for sorting the uint16_t count map. Used in printPorts()
         */
        static bool uint16CountComparer(std::pair<uint16_t, int> first, std::pair<uint16_t, int> second);

        /**
         * An auxiliary method for GeneralFilter object from numeric port. Used in run()
         */
        static GeneralFilter* portToPortFilter(uint16_t p);

        /**
         * Print the hostname count map to a table sorted by popularity (most popular hostnames will be first)
         */
        void printHostnames(HttpRequestStats& reqStatscollector);

        /**
         * Print the server-name count map to a table sorted by popularity (most popular names will be first)
         */
        void printServerNames(ClientHelloStats& clientHelloStatsCollector);

        /**
         * Print SSL record version map
         */
        void printVersions(std::map<SSLVersion, int>& versionMap, std::string headline);

        /**
         * Print a summary of all statistics collected by the HttpStatsCollector. Should be called when traffic capture was finished
         */
        void printStatsSummary(HttpStatsCollector& collector);

        /**
         * Print a summary of all statistics collected by the SSLStatsCollector. Should be called when traffic capture was finished
         */
        void printStatsSummary(SSLStatsCollector& collector);

        /**
         * packet capture callback - called whenever a packet arrives
         */
        static void packetArrive(RawPacket* packet, PcapLiveDevice* dev, void* cookie);

        //void printPorts(SSLGeneralStats& stats)
        //{
        //	// create the table
        //	std::vector<std::string> columnNames;
        //	columnNames.push_back("SSL/TLS ports");
        //	columnNames.push_back("Count");
        //	std::vector<int> columnsWidths;
        //	columnsWidths.push_back(13);
        //	columnsWidths.push_back(5);
        //	TablePrinter printer(columnNames, columnsWidths);
        //
        //	// sort the port count map so the most popular names will be first
        //	// since it's not possible to sort a std::map you must copy it to a std::vector and sort it then
        //	std::vector<std::pair<uint16_t, int> > map2vec(stats.sslPortCount.begin(), stats.sslPortCount.end());
        //	std::sort(map2vec.begin(), map2vec.end(), &uint16CountComparer);
        //
        //	// go over all items (names + count) in the sorted vector and print them
        //	for (std::vector<std::pair<uint16_t, int> >::iterator iter = map2vec.begin();
        //		iter != map2vec.end();
        //		iter++)
        //	{
        //		std::stringstream values;
        //		values << iter->first << "|" << iter->second;
        //		printer.printRow(values.str(), '|');
        //	}
        //}

        /**
         * Print the current rates. Should be called periodically during traffic capture
         */
        void printCurrentRates(HttpStatsCollector& collector);

        /**
         * Print the current rates. Should be called periodically during traffic capture
         */
        void printCurrentRates(SSLStatsCollector& collector);

    public:
        NetworkManager(std::string interfaceNameOrIP);

        /*
         * Start analyzing packets in live traffic mode
         */
        void run();
    };
}

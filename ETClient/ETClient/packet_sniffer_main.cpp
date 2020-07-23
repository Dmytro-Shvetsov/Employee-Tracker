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
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <algorithm>
#if !defined(WIN32) && !defined(WINx64) && !defined(PCAPPP_MINGW_ENV)  //for using ntohl, ntohs, etc.
#include <in.h>
#endif
#include "PcapLiveDeviceList.h"
#include "PcapFilter.h"
#include "PcapFileDevice.h"
#include "HttpStatsCollector.h"
#include "SSLStatsCollector.h"
#include "TablePrinter.h"
#include "PlatformSpecificUtils.h"
#include "SystemUtils.h"
#include <HttpLayer.h>
#include "NetworkUtils.h"
#include "PcapPlusPlusVersion.h"
#include <chrono>
#include <map>
#include <vector>

using namespace pcpp;

#define EXIT_WITH_ERROR(reason, ...) do { \
	printf("\nError: " reason "\n\n", ## __VA_ARGS__); \
	system("pause"); \
	} while(0)

#define PRINT_STAT_HEADLINE(description) \
		printf("\n" description "\n--------------------\n\n")

#define DEFAULT_CALC_RATES_PERIOD_SEC 2


///**
// * Print the method count table
// */
void printMethods(HttpRequestStats& reqStatscollector)
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


// * An auxiliary method for sorting the hostname count map. Used only in printHostnames()
// */
bool hostnameComparer(std::pair<std::string, int> first, std::pair<std::string, int> second)
{
	return first.second > second.second;
}

/**
 * An auxiliary method for sorting the string count map. Used in printServerNames() and in printCipherSuites()
 */
bool stringCountComparer(std::pair<std::string, int> first, std::pair<std::string, int> second)
{
	return first.second > second.second;
}


/**
 * An auxiliary method for sorting the uint16_t count map. Used in printPorts()
 */
bool uint16CountComparer(std::pair<uint16_t, int> first, std::pair<uint16_t, int> second)
{
	return first.second > second.second;
}

/**
 * Print the hostname count map to a table sorted by popularity (most popular hostnames will be first)
 */
void printHostnames(HttpRequestStats& reqStatscollector)
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
	std::sort(map2vec.begin(), map2vec.end(), &hostnameComparer);

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


/**
 * Print the server-name count map to a table sorted by popularity (most popular names will be first)
 */
void printServerNames(ClientHelloStats& clientHelloStatsCollector)
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


/**
 * Print SSL record version map
 */
void printVersions(std::map<SSLVersion, int>& versionMap, std::string headline)
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

///**
// * Print a summary of all statistics collected by the HttpStatsCollector. Should be called when traffic capture was finished
// */
void printStatsSummary(HttpStatsCollector& collector)
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

/**
 * Print a summary of all statistics collected by the SSLStatsCollector. Should be called when traffic capture was finished
 */
void printStatsSummary(SSLStatsCollector& collector)
{
	std::cout << "Client-hello message" << collector.getClientHelloStats().numOfMessages << std::endl;
	std::cout << "Server-hello message" << collector.getServerHelloStats().numOfMessages << std::endl;

	PRINT_STAT_HEADLINE("Server-name count");
	printServerNames(collector.getClientHelloStats());
}

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

	unsigned long int c = 0;
	void calcRates()
	{
		this->sslStatsCollector->calcRates();
		this->httpStatsCollector->calcRates();
	}
	void tryCollectStats(Packet* parsedPacket)
	{
		bool collectStatus = (this->sslStatsCollector->tryCollectStats(parsedPacket) 
			|| this->httpStatsCollector->tryCollectStats(parsedPacket));
		
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

			printf("Source ip is '%s'; Dest ip is '%s'\n", (srcIP.c_str()), (dstIP.c_str()));
		}
	}

	static const char* resolveIp(const char* ipStr)
	{
		struct in_addr ip;
		struct hostent* hp;

		if (!inet_pton(AF_INET, ipStr, &ip))
		{
			//printf("can't parse IP address %s \n", ipStr);
			return ipStr;
		}

		if ((hp = gethostbyaddr(
			(const char *)&ip,
			sizeof ip, AF_INET)) == NULL)
		{
			printf("no name associated with %s \n", ipStr);
			return ipStr;
		}
		return hp->h_name;
	}

};


/**
 * packet capture callback - called whenever a packet arrives
 */
void packetArrive(RawPacket* packet, PcapLiveDevice* dev, void* cookie)
{
	PacketArrivedData* data = (PacketArrivedData*)cookie;

	// parse the packet
	Packet parsedPacket(packet);

	// give the packet to the collectors
	data->tryCollectStats(&parsedPacket);
}




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
void printCurrentRates(HttpStatsCollector& collector)
{
	PRINT_STAT_HEADLINE("Current HTTP rates");
	std::cout << "Rate of HTTP requests(Requests/sec) "  << collector.getRequestStats().messageRate.currentRate << std::endl;
	std::cout << "Rate of HTTP responses(Requests/sec) " << collector.getResponseStats().messageRate.currentRate << std::endl;
}

/**
 * Print the current rates. Should be called periodically during traffic capture
 */
void printCurrentRates(SSLStatsCollector& collector)
{
	PRINT_STAT_HEADLINE("Current SSL rates");
	std::cout << "Rate of SSL requests(Requests/sec) " << collector.getClientHelloStats().messageRate.currentRate << std::endl;
	std::cout << "Rate of SSL responses(Requests/sec) " << collector.getServerHelloStats().messageRate.currentRate << std::endl;
}

GeneralFilter* portToPortFilter(uint16_t p)
{
	return new PortFilter(p, pcpp::SRC_OR_DST);
};

/*
	activate SSL analysis from live traffic
*/
void analyzeSSLFromLiveTraffic(PcapLiveDevice* dev, bool printRatesPeriodicaly, int printRatePeriod)
{
	// open the device
	if (!dev->open())
		EXIT_WITH_ERROR("Could not open the device");

	// set HTTP SSL/TLS ports filter on the live device to capture only SSL/TLS packets
	// Ñurrently SSLLayer support only listed ones.
	std::vector<uint16_t> sslPorts = { 0, 80, 261, 443, 448, 465, 563, 614, 636, 989, 990, 992, 993, 994, 995 };
	std::vector<GeneralFilter*> portFilterVec;
	std::transform(sslPorts.begin(), sslPorts.end(), std::back_inserter(portFilterVec), portToPortFilter);

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

	dev->startCapture(packetArrive, &data);

	
	PCAP_SLEEP(60); // 2 min

	// stop capturing and close the live device
	dev->stopCapture();
	dev->close();

	// calculate final rates
	data.calcRates();
	printf("\n\nSTATS SUMMARY\n");
	printf("=============\n");
	system("cls");
	printStatsSummary(*data.httpStatsCollector);
	printStatsSummary(*data.sslStatsCollector);


#define MAX_UKNOWN_HOSTS_RESOLVE 30

	size_t nUnknownHosts = data.unknownHostCount.size();
	std::cout << "Unknown host size: " << nUnknownHosts << std::endl;

	
	std::vector<std::pair<std::string, int>> map2vec(data.unknownHostCount.begin(), data.unknownHostCount.end());
	std::sort(map2vec.begin(), map2vec.end(), &hostnameComparer);

	std::vector<std::pair<std::string, int>>::iterator itBegin, itEnd;
	itBegin = itEnd = map2vec.begin();

	itEnd += MAX_UKNOWN_HOSTS_RESOLVE > nUnknownHosts ? nUnknownHosts : MAX_UKNOWN_HOSTS_RESOLVE;
	std::cout << "################ Unknown host names: ###################" << std::endl;
	for (; itBegin != itEnd; itBegin++)
	{
		auto pair = *itBegin;
		std::cout << PacketArrivedData::resolveIp(pair.first.c_str()) << " -> " << pair.second << std::endl;
	}
}

void foo()
{
	std::string interfaceNameOrIP = "192.168.0.106";
	std::string port = "80";
	bool printRatesPeriodicaly = true;
	int printRatePeriod = DEFAULT_CALC_RATES_PERIOD_SEC;

	// if no interface nor input pcap file were provided - exit with error
	if (interfaceNameOrIP == "")
		EXIT_WITH_ERROR("Interface was not provided");

	// analyze in live traffic mode
	// extract pcap live device by interface name or IP address
	PcapLiveDevice* dev = NULL;
	IPv4Address interfaceIP(interfaceNameOrIP);
	if (interfaceIP.isValid())
	{
		dev = PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIP);
		if (dev == NULL)
			EXIT_WITH_ERROR("Couldn't find interface by provided IP");
	}
	else
	{
		dev = PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(interfaceNameOrIP);
		if (dev == NULL)
			EXIT_WITH_ERROR("Couldn't find interface by provided name");
	}

	// start capturing and analyzing traffic
	analyzeSSLFromLiveTraffic(dev, printRatesPeriodicaly, printRatePeriod);
}
/**
 * main method of this utility
 */
int main(int argc, char* argv[])
{
	foo();
	system("pause");
}
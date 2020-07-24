#pragma once

#include <map>
#include <sstream>
#include <TcpLayer.h>
#include <IPv4Layer.h>
#include <PayloadLayer.h>
#include <PacketUtils.h>
#include <SSLLayer.h>
#include <SystemUtils.h>


struct Rate;
/**
 * A base struct for collecting stats on client-hello messages
 */

#ifndef s_Rate
#define s_Rate
 /**
  * An auxiliary struct for encapsulating rate stats
  */
struct Rate
{
	double currentRate; // periodic rate
	double totalRate;	 // overall rate

	void clear()
	{
		currentRate = 0;
		totalRate = 0;
	}
};
#endif

struct ClientHelloStats
{
	int numOfMessages; // total number of client-hello messages
	Rate messageRate; // rate of client-hello messages
	std::map<std::string, int> serverNameCount; // a map for counting the server names seen in traffic

	virtual ~ClientHelloStats() {}

	virtual void clear()
	{
		numOfMessages = 0;
		messageRate.currentRate = 0;
		messageRate.totalRate = 0;
		serverNameCount.clear();
	}
};

/**
 * A base struct for collecting stats on server-hello messages
 */
struct ServerHelloStats
{
	int numOfMessages; // total number of server-hello messages
	Rate messageRate; // rate of server-hello messages

	virtual ~ServerHelloStats() {}

	virtual void clear()
	{
		numOfMessages = 0;
		messageRate.currentRate = 0;
		messageRate.totalRate = 0;
	}
};

/**
 * The SSL stats collector. Should be called for every packet arriving and also periodically to calculate rates
 */
class SSLStatsCollector
{
private:
	/**
	 * Auxiliary data collected for each flow for help calculating stats on this flow
	 */
	struct SSLFlowData
	{
		bool seenAppDataPacket; // was SSL application data seen in this flow
		bool seenAlertPacket; // was SSL alert packet seen in this flow

		void clear()
		{
			seenAppDataPacket = false;
			seenAlertPacket = false;
		}
	};

	ClientHelloStats m_ClientHelloStats;
	ClientHelloStats m_PrevClientHelloStats;
	ServerHelloStats m_ServerHelloStats;
	ServerHelloStats m_PrevServerHelloStats;

	std::map<uint32_t, SSLFlowData> m_FlowTable;

	double m_LastCalcRateTime;
	double m_StartTime;

	/**
	 * Collect stats relevant for every SSL packet (any SSL message)
	 * This method calculates and returns the flow key for this packet
	 */
	uint32_t collectSSLTrafficStats(pcpp::Packet* sslpPacket)
	{
		pcpp::TcpLayer* tcpLayer = sslpPacket->getLayerOfType<pcpp::TcpLayer>();

		// calculate a hash key for this flow to be used in the flow table
		uint32_t hashVal = hash5Tuple(sslpPacket);
		// if flow is a new flow (meaning it's not already in the flow table)
		if (m_FlowTable.find(hashVal) == m_FlowTable.end())
		{
			m_FlowTable[hashVal].clear();
		}
		return hashVal;
	}

	/**
	 * Collect stats relevant for several kinds SSL messages
	 */
	void collectSSLStats(pcpp::Packet* sslPacket, uint32_t flowKey)
	{
		// go over all SSL messages in this packet
		pcpp::SSLLayer* sslLayer = sslPacket->getLayerOfType<pcpp::SSLLayer>();
		while (sslLayer != NULL)
		{
			// check if the layer is an alert message
			pcpp::SSLRecordType recType = sslLayer->getRecordType();
			if (recType == pcpp::SSL_ALERT)
			{
				// if it's the first alert seen in this flow
				if (m_FlowTable[flowKey].seenAlertPacket == false)
				{
					//m_GeneralStats.numOfFlowsWithAlerts++;
					m_FlowTable[flowKey].seenAlertPacket = true;
				}
			}

			// check if the layer is an app data message
			else if (recType == pcpp::SSL_APPLICATION_DATA)
			{
				// if it's the first app data message seen on this flow it means handshake was completed
				if (m_FlowTable[flowKey].seenAppDataPacket == false)
				{
					//m_GeneralStats.numOfHandshakeCompleteFlows++;
					m_FlowTable[flowKey].seenAppDataPacket = true;
				}
			}

			// check if the layer is an handshake message
			else if (recType == pcpp::SSL_HANDSHAKE)
			{
				pcpp::SSLHandshakeLayer* handshakeLayer = dynamic_cast<pcpp::SSLHandshakeLayer*>(sslLayer);
				if (handshakeLayer == NULL)
					continue;

				// try to find client-hello message
				pcpp::SSLClientHelloMessage* clientHelloMessage = handshakeLayer->getHandshakeMessageOfType<pcpp::SSLClientHelloMessage>();

				// collect client-hello stats
				if (clientHelloMessage != NULL)
				{
					collecClientHelloStats(clientHelloMessage);
				}
			}
			sslLayer = sslPacket->getNextLayerOfType<pcpp::SSLLayer>(sslLayer);
		}
	}

	/**
	 * Collect stats relevant only to client-hello messages
	 */
	void collecClientHelloStats(pcpp::SSLClientHelloMessage* clientHelloMessage)
	{
		m_ClientHelloStats.numOfMessages++;

		pcpp::SSLServerNameIndicationExtension* sniExt = clientHelloMessage->getExtensionOfType<pcpp::SSLServerNameIndicationExtension>();
		if (sniExt != NULL)
			m_ClientHelloStats.serverNameCount[sniExt->getHostName()]++;
	}

	double getCurTime(void)
	{
		struct timeval tv;

		gettimeofday(&tv, NULL);

		return (((double)tv.tv_sec) + (double)(tv.tv_usec / 1000000.0));
	}

public:
	/**
	 * C'tor - clear all structures
	 */
	SSLStatsCollector()
	{
		clear();
	}
	/**
	 * Collect stats for a single packet
	 */
	bool tryCollectStats(pcpp::Packet* parsedPacket)
	{
		// verify packet is TCP and SSL/TLS
		if (!parsedPacket->isPacketOfType(pcpp::TCP) || !parsedPacket->isPacketOfType(pcpp::SSL))
			return false;
		
		// collect general SSL traffic stats on this packet
		uint32_t hashVal = collectSSLTrafficStats(parsedPacket);

		// if packet contains one or more SSL messages, collect stats on them
		if (parsedPacket->isPacketOfType(pcpp::SSL))
		{
			collectSSLStats(parsedPacket, hashVal);
		}
		return true;
	}

	/**
	 * Calculate rates. Should be called periodically
	 */
	void calcRates()
	{
		// getting current machine time
		double curTime = getCurTime();

		// getting time from last rate calculation until now
		double diffSec = curTime - m_LastCalcRateTime;

		// calculating current rates which are the changes from last rate calculation until now divided by the time passed from
		// last rate calculation until now
		if (diffSec != 0)
		{
			m_ClientHelloStats.messageRate.currentRate = (m_ClientHelloStats.numOfMessages - m_PrevClientHelloStats.numOfMessages) / diffSec;
			m_ServerHelloStats.messageRate.currentRate = (m_ServerHelloStats.numOfMessages - m_PrevServerHelloStats.numOfMessages) / diffSec;
		}

		// getting the time from the beginning of stats collection until now
		double diffSecTotal = curTime - m_StartTime;

		// calculating total rate which is the change from beginning of stats collection until now divided by time passed from
		// beginning of stats collection until now
		if (diffSecTotal != 0)
		{
			m_ClientHelloStats.messageRate.totalRate = m_ClientHelloStats.numOfMessages / diffSecTotal;
			m_ServerHelloStats.messageRate.totalRate = m_ServerHelloStats.numOfMessages / diffSecTotal;
		}

		// saving current numbers for using them in the next rate calculation
		m_PrevClientHelloStats = m_ClientHelloStats;
		m_PrevServerHelloStats = m_ServerHelloStats;

		// saving the current time for using in the next rate calculation
		m_LastCalcRateTime = curTime;
	}

	/**
	 * Clear all stats collected so far
	 */
	void clear()
	{
		m_ClientHelloStats.clear();
		m_PrevClientHelloStats.clear();
		m_ServerHelloStats.clear();
		m_PrevServerHelloStats.clear();
		m_LastCalcRateTime = getCurTime();
		m_StartTime = m_LastCalcRateTime;
	}

	/**
	 * Get client-hello stats
	 */
	ClientHelloStats& getClientHelloStats() { return m_ClientHelloStats; }

	/**
	 * Get server-hello stats
	 */
	ServerHelloStats& getServerHelloStats() { return m_ServerHelloStats; }
};

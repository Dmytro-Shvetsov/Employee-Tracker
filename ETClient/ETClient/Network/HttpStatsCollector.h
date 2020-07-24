#pragma once

#include <map>
#include <sstream>
#include <HttpLayer.h>
#include <TcpLayer.h>
#include <IPv4Layer.h>
#include <PayloadLayer.h>
#include <PacketUtils.h>
#include <SystemUtils.h>


struct Rate;

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

/**
 * A base struct for collecting stats on HTTP messages
 */
struct HttpMessageStats
{
    size_t numOfMessages; // total number of HTTP messages of that type (request/response)
	Rate messageRate; // rate of HTTP messages of that type
    size_t totalMessageHeaderSize; // total size (in bytes) of data in headers
	double averageMessageHeaderSize; // average header size

	virtual ~HttpMessageStats() {}

	virtual void clear()
	{
		numOfMessages = 0;
		messageRate.clear();
		totalMessageHeaderSize = 0;
		averageMessageHeaderSize = 0;
	}
};


/**
 * A struct for collecting stats on all HTTP requests
 */
struct HttpRequestStats : HttpMessageStats
{
	std::map<pcpp::HttpRequestLayer::HttpMethod, int> methodCount; // a map for counting the different HTTP methods seen in traffic
	std::map<std::string, int> hostnameCount; // a map for counting the hostnames seen in traffic

	void clear()
	{
		HttpMessageStats::clear();
		methodCount.clear();
		hostnameCount.clear();
	}
};


/**
 * A struct for collecting stats on all HTTP responses
 */
struct HttpResponseStats : HttpMessageStats
{
	std::map<std::string, int> statusCodeCount; // a map for counting the different status codes seen in traffic
	std::map<std::string, int> contentTypeCount; // a map for counting the content-types seen in traffic
    size_t numOfMessagesWithContentLength; // total number of responses containing the "content-length" field
    size_t totalConentLengthSize; // total body size extracted by responses containing "content-length" field
	double averageContentLengthSize; // average body size

	void clear()
	{
		HttpMessageStats::clear();
		numOfMessagesWithContentLength = 0;
		totalConentLengthSize = 0;
		averageContentLengthSize = 0;
		statusCodeCount.clear();
		contentTypeCount.clear();
	}
};


/**
 * The HTTP stats collector. Should be called for every packet arriving and also periodically to calculate rates
 */
class HttpStatsCollector
{
private:

	/**
	 * Auxiliary data collected for each flow for help calculating stats on this flow
	 */
	struct HttpFlowData
	{
		int numOfOpenTransactions; // number of transactions that were started (request has arrived) but weren't closed yet (response hasn't arrived yet)
		pcpp::ProtocolType lastSeenMessage; // the last HTTP message seen on this flow (request, response or neither). Used to identify HTTP pipelining
		bool httpPipeliningFlow; // was HTTP pipelining identified on this flow
		uint32_t curSeqNumberRequests; // the current TCP sequence number from client to server. Used to identify TCP re-transmission
		uint32_t curSeqNumberResponses; // the current TCP sequence number from server to client. Used to identify TCP re-transmission

		void clear()
		{
			numOfOpenTransactions = 0;
			lastSeenMessage = pcpp::UnknownProtocol;
			httpPipeliningFlow = false;
		}
	};

	HttpRequestStats m_RequestStats;
	HttpRequestStats m_PrevRequestStats;
	HttpResponseStats m_ResponseStats;
	HttpResponseStats m_PrevResponseStats;

	std::map<uint32_t, HttpFlowData> m_FlowTable;

	double m_LastCalcRateTime;
	double m_StartTime;
	uint16_t m_DstPort;

	/**
	 * Collect stats relevant for every HTTP packet (request, response or any other)
	 * This method calculates and returns the flow key for this packet
	 */
	uint32_t collectHttpTrafficStats(pcpp::Packet* httpPacket)
	{
//		pcpp::TcpLayer* tcpLayer = httpPacket->getLayerOfType<pcpp::TcpLayer>();

		// calculate a hash key for this flow to be used in the flow table
		uint32_t hashVal = pcpp::hash5Tuple(httpPacket);

		// if flow is a new flow (meaning it's not already in the flow table)
		if (m_FlowTable.find(hashVal) == m_FlowTable.end())
		{
			m_FlowTable[hashVal].clear();
		}

		return hashVal;
	}

	/**
	 * Collect stats relevant for HTTP messages (requests or responses)
	 */
	void collectHttpGeneralStats(pcpp::TcpLayer* tcpLayer, pcpp::HttpMessage* message, uint32_t flowKey)
	{
		// if num of current opened transaction is negative it means something went completely wrong
		if (m_FlowTable[flowKey].numOfOpenTransactions < 0)
			return;

		if (message->getProtocol() == pcpp::HTTPRequest)
		{
			// if new packet seq number is smaller than previous seen seq number current it means this packet is
			// a re-transmitted packet and should be ignored
			if (m_FlowTable[flowKey].curSeqNumberRequests >= ntohl(tcpLayer->getTcpHeader()->sequenceNumber))
				return;

			// a new request - increase num of open transactions
			m_FlowTable[flowKey].numOfOpenTransactions++;

			// if the previous message seen on this flow is HTTP request and if flow is not already marked as HTTP pipelining -
			// mark it as so and increase number of HTTP pipelining flows
			if (!m_FlowTable[flowKey].httpPipeliningFlow && m_FlowTable[flowKey].lastSeenMessage == pcpp::HTTPRequest)
			{
				m_FlowTable[flowKey].httpPipeliningFlow = true;
			}

			// set last seen message on flow as HTTP request
			m_FlowTable[flowKey].lastSeenMessage = pcpp::HTTPRequest;

			// set last seen sequence number
			m_FlowTable[flowKey].curSeqNumberRequests = ntohl(tcpLayer->getTcpHeader()->sequenceNumber);
		}
		else if (message->getProtocol() == pcpp::HTTPResponse)
		{
			// if new packet seq number is smaller than previous seen seq number current it means this packet is
			// a re-transmitted packet and should be ignored
			if (m_FlowTable[flowKey].curSeqNumberResponses >= ntohl(tcpLayer->getTcpHeader()->sequenceNumber))
				return;

			// a response - decrease num of open transactions
			m_FlowTable[flowKey].numOfOpenTransactions--;

			// if the previous message seen on this flow is HTTP response and if flow is not already marked as HTTP pipelining -
			// mark it as so and increase number of HTTP pipelining flows
			if (!m_FlowTable[flowKey].httpPipeliningFlow && m_FlowTable[flowKey].lastSeenMessage == pcpp::HTTPResponse)
			{
				m_FlowTable[flowKey].httpPipeliningFlow = true;
			}

			// set last seen message on flow as HTTP response
			m_FlowTable[flowKey].lastSeenMessage = pcpp::HTTPResponse;

			// set last seen sequence number
			m_FlowTable[flowKey].curSeqNumberResponses = ntohl(tcpLayer->getTcpHeader()->sequenceNumber);
		}
	}

	/**
	 * Collect stats relevant for HTTP request messages
	 */
	void collectRequestStats(pcpp::HttpRequestLayer* req)
	{
		m_RequestStats.numOfMessages++;
		m_RequestStats.totalMessageHeaderSize += req->getHeaderLen();
		if (m_RequestStats.numOfMessages != 0)
			m_RequestStats.averageMessageHeaderSize = (double)m_RequestStats.totalMessageHeaderSize / (double)m_RequestStats.numOfMessages;

		// extract hostname and add to hostname count map
		pcpp::HeaderField* hostField = req->getFieldByName(PCPP_HTTP_HOST_FIELD);
		if (hostField != NULL)
			m_RequestStats.hostnameCount[hostField->getFieldValue()]++;

		m_RequestStats.methodCount[req->getFirstLine()->getMethod()]++;
	}


	/**
	 * Collect stats relevant for HTTP response messages
	 */
	void collectResponseStats(pcpp::HttpResponseLayer* res)
	{
		m_ResponseStats.numOfMessages++;
		m_ResponseStats.totalMessageHeaderSize += res->getHeaderLen();
		if (m_ResponseStats.numOfMessages != 0)
			m_ResponseStats.averageMessageHeaderSize = (double)m_ResponseStats.totalMessageHeaderSize / (double)m_ResponseStats.numOfMessages;

		// extract content-length (if exists)
		pcpp::HeaderField* contentLengthField = res->getFieldByName(PCPP_HTTP_CONTENT_LENGTH_FIELD);
		if (contentLengthField != NULL)
		{
			m_ResponseStats.numOfMessagesWithContentLength++;
			m_ResponseStats.totalConentLengthSize += atoi(contentLengthField->getFieldValue().c_str());
			if (m_ResponseStats.numOfMessagesWithContentLength != 0)
				m_ResponseStats.averageContentLengthSize = (double)m_ResponseStats.totalConentLengthSize / (double)m_ResponseStats.numOfMessagesWithContentLength;
		}

		// extract content-type and add to content-type map
		pcpp::HeaderField* contentTypeField = res->getFieldByName(PCPP_HTTP_CONTENT_TYPE_FIELD);
		if (contentTypeField != NULL)
		{
			std::string contentType = contentTypeField->getFieldValue();

			// sometimes content-type contains also the charset it uses.
			// for example: "application/javascript; charset=UTF-8"
			// remove charset as it's not relevant for these stats
			size_t charsetPos = contentType.find(";");
			if (charsetPos != std::string::npos)
				contentType = contentType.substr(0, charsetPos);

			m_ResponseStats.contentTypeCount[contentType]++;
		}

		// collect status code - create one string from status code and status description (for example: 200 OK)
		std::ostringstream stream;
		stream << res->getFirstLine()->getStatusCodeAsInt();
		std::string statusCode = stream.str() + " " + res->getFirstLine()->getStatusCodeString();
		m_ResponseStats.statusCodeCount[statusCode]++;
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
	HttpStatsCollector(uint16_t dstPort)
	{
		clear();
		m_DstPort = dstPort;
	}

	/**
	 * Collect stats for a single packet
	 */
	bool tryCollectStats(pcpp::Packet* httpPacket)
	{
		// verify packet is TCP
		if (!httpPacket->isPacketOfType(pcpp::TCP))
			return false;

		// verify packet is port 80
		pcpp::TcpLayer* tcpLayer = httpPacket->getLayerOfType<pcpp::TcpLayer>();
		if (!(tcpLayer->getTcpHeader()->portDst == htons(m_DstPort) || tcpLayer->getTcpHeader()->portSrc == htons(m_DstPort)))
			return false;

		// collect general HTTP traffic stats on this packet
		uint32_t hashVal = collectHttpTrafficStats(httpPacket);

		// if packet is an HTTP request - collect HTTP request stats on this packet
		//pcpp::TcpLayer* tcpLayer = httpPacket->getLayerOfType<pcpp::TcpLayer>();
		if (httpPacket->isPacketOfType(pcpp::HTTPRequest))
		{
			pcpp::HttpRequestLayer* req = httpPacket->getLayerOfType<pcpp::HttpRequestLayer>();
			this->collectHttpGeneralStats(tcpLayer, req, hashVal);
			this->collectRequestStats(req);
		}
		// if packet is an HTTP response - collect HTTP response stats on this packet
		else if (httpPacket->isPacketOfType(pcpp::HTTPResponse))
		{
			pcpp::HttpResponseLayer* res = httpPacket->getLayerOfType<pcpp::HttpResponseLayer>();
			this->collectHttpGeneralStats(tcpLayer, res, hashVal);
			this->collectResponseStats(res);
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
			m_RequestStats.messageRate.currentRate = (m_RequestStats.numOfMessages - m_PrevRequestStats.numOfMessages) / diffSec;
			m_ResponseStats.messageRate.currentRate = (m_ResponseStats.numOfMessages - m_PrevResponseStats.numOfMessages) / diffSec;
		}

		// getting the time from the beginning of stats collection until now
		double diffSecTotal = curTime - m_StartTime;

		// calculating total rate which is the change from beginning of stats collection until now divided by time passed from
		// beginning of stats collection until now
		if (diffSecTotal != 0)
		{
			m_RequestStats.messageRate.totalRate = m_RequestStats.numOfMessages / diffSecTotal;
			m_ResponseStats.messageRate.totalRate = m_ResponseStats.numOfMessages / diffSecTotal;
		}

		// saving current numbers for using them in the next rate calculation
		m_PrevRequestStats = m_RequestStats;
		m_PrevResponseStats = m_ResponseStats;

		// saving the current time for using in the next rate calculation
		m_LastCalcRateTime = curTime;
	}

	/**
	 * Clear all stats collected so far
	 */
	void clear()
	{
		m_RequestStats.clear();
		m_PrevRequestStats.clear();
		m_ResponseStats.clear();
		m_PrevResponseStats.clear();
		m_LastCalcRateTime = getCurTime();
		m_StartTime = m_LastCalcRateTime;
	}

	/**
	 * Get HTTP request stats
	 */
	HttpRequestStats& getRequestStats() { return m_RequestStats; }

	/**
	 * Get HTTP response stats
	 */
	HttpResponseStats& getResponseStats() { return m_ResponseStats; }


};

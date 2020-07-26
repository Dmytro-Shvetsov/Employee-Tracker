#pragma once


#include <HttpLayer.h>
#include "BaseStatsCollector.h"

namespace pcpp
{
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
    class HttpStatsCollector : public BaseStatsCollector
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

        uint16_t m_DstPort;

        /**
         * Collect stats relevant for every HTTP packet (request, response or any other)
         * This method calculates and returns the flow key for this packet
         */
        uint32_t collectHttpTrafficStats(pcpp::Packet* httpPacket);

        /**
         * Collect stats relevant for HTTP messages (requests or responses)
         */
        void collectHttpGeneralStats(pcpp::TcpLayer* tcpLayer, pcpp::HttpMessage* message, uint32_t flowKey);

        /**
         * Collect stats relevant for HTTP request messages
         */
        void collectRequestStats(pcpp::HttpRequestLayer* req);

        /**
         * Collect stats relevant for HTTP response messages
         */
        void collectResponseStats(pcpp::HttpResponseLayer* res);
    public:
        /**
         * C'tor - clear all structures
         */
        HttpStatsCollector(uint16_t dstPort);
        virtual ~HttpStatsCollector();
        bool tryCollectStats(pcpp::Packet* httpPacket)override;
        void calcRates()override;
        void clear()override;

        /**
         * Get HTTP request stats
         */
        HttpRequestStats& getRequestStats();

        /**
         * Get HTTP response stats
         */
        HttpResponseStats& getResponseStats();
    };
}

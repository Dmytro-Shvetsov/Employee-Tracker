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
        quint32 numOfMessages; // total number of HTTP messages of that type (request/response)

        virtual ~HttpMessageStats() {}

        virtual void clear()
        {
            this->numOfMessages = 0;
        }
    };

    /**
     * A struct for collecting stats on all HTTP requests
     */
    struct HttpRequestStats : HttpMessageStats
    {
        QMap<pcpp::HttpRequestLayer::HttpMethod, quint32> methodCount; // a map for counting the different HTTP methods seen in traffic
        std::map<QString, quint32> hostnameCount; // a map for counting the hostnames seen in traffic

        void clear()
        {
            HttpMessageStats::clear();
            this->methodCount.clear();
            this->hostnameCount.clear();
        }
    };

    /**
     * A struct for collecting stats on all HTTP responses
     */
    struct HttpResponseStats : HttpMessageStats
    {
        std::map<std::string, int> statusCodeCount; // a map for counting the different status codes seen in traffic
        std::map<std::string, int> contentTypeCount; // a map for counting the content-types seen in traffic

        void clear()
        {
            HttpMessageStats::clear();
            this->contentTypeCount.clear();
            this->statusCodeCount.clear();
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
            uint32_t numOfOpenTransactions; // number of transactions that were started (request has arrived) but weren't closed yet (response hasn't arrived yet)
            pcpp::ProtocolType lastSeenMessage; // the last HTTP message seen on this flow (request, response or neither). Used to identify HTTP pipelining
            bool httpPipeliningFlow; // was HTTP pipelining identified on this flow
            uint32_t curSeqNumberRequests; // the current TCP sequence number from client to server. Used to identify TCP re-transmission
            uint32_t curSeqNumberResponses; // the current TCP sequence number from server to client. Used to identify TCP re-transmission

            void clear()
            {
                this->numOfOpenTransactions = 0;
                this->lastSeenMessage = pcpp::UnknownProtocol;
                this->httpPipeliningFlow = false;
            }
        };

        HttpRequestStats requestStats;
        HttpResponseStats responseStats;

        QMap<uint32_t, HttpFlowData> flowTable;

        uint16_t dstPort;

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

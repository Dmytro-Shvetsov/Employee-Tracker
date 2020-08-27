#include "HttpStatsCollector.h"


namespace pcpp
{
    HttpStatsCollector::HttpStatsCollector(uint16_t dstPort) : dstPort(dstPort)
    {
        this->clear();
    }

    HttpStatsCollector::~HttpStatsCollector()
    {
    }

    uint32_t HttpStatsCollector::collectHttpTrafficStats(Packet *httpPacket)
    {
        // calculate a hash key for this flow to be used in the flow table
        uint32_t hashVal = pcpp::hash5Tuple(httpPacket);

        // if flow is a new flow (meaning it's not already in the flow table)
        if (this->flowTable.find(hashVal) == this->flowTable.end())
        {
            this->flowTable[hashVal].clear();
        }

        return hashVal;
    }

    void HttpStatsCollector::collectHttpGeneralStats(TcpLayer* tcpLayer, HttpMessage *message, uint32_t flowKey)
    {
        // if num of current opened transaction is negative it means something went completely wrong
        if (this->flowTable[flowKey].numOfOpenTransactions < 0)
        {
            return;
        }
        if (message->getProtocol() == pcpp::HTTPRequest)
        {
            // if new packet seq number is smaller than previous seen seq number current it means this packet is
            // a re-transmitted packet and should be ignored
            if (this->flowTable[flowKey].curSeqNumberRequests >= ntohl(tcpLayer->getTcpHeader()->sequenceNumber))
            {
                return;
            }
            // a new request - increase num of open transactions
            this->flowTable[flowKey].numOfOpenTransactions++;

            // if the previous message seen on this flow is HTTP request and if flow is not already marked as HTTP pipelining -
            // mark it as so and increase number of HTTP pipelining flows
            if (!this->flowTable[flowKey].httpPipeliningFlow && this->flowTable[flowKey].lastSeenMessage == pcpp::HTTPRequest)
            {
                this->flowTable[flowKey].httpPipeliningFlow = true;
            }

            // set last seen message on flow as HTTP request
            this->flowTable[flowKey].lastSeenMessage = pcpp::HTTPRequest;

            // set last seen sequence number
            this->flowTable[flowKey].curSeqNumberRequests = ntohl(tcpLayer->getTcpHeader()->sequenceNumber);
        }
        else if (message->getProtocol() == pcpp::HTTPResponse)
        {
            // if new packet seq number is smaller than previous seen seq number current it means this packet is
            // a re-transmitted packet and should be ignored
            if (this->flowTable[flowKey].curSeqNumberResponses >= ntohl(tcpLayer->getTcpHeader()->sequenceNumber))
            {
                return;
            }
            // a response - decrease num of open transactions
            this->flowTable[flowKey].numOfOpenTransactions--;

            // if the previous message seen on this flow is HTTP response and if flow is not already marked as HTTP pipelining -
            // mark it as so and increase number of HTTP pipelining flows
            if (!this->flowTable[flowKey].httpPipeliningFlow && this->flowTable[flowKey].lastSeenMessage == pcpp::HTTPResponse)
            {
                this->flowTable[flowKey].httpPipeliningFlow = true;
            }

            // set last seen message on flow as HTTP response
            this->flowTable[flowKey].lastSeenMessage = pcpp::HTTPResponse;

            // set last seen sequence number
            this->flowTable[flowKey].curSeqNumberResponses = ntohl(tcpLayer->getTcpHeader()->sequenceNumber);
        }
    }

    void HttpStatsCollector::collectRequestStats(HttpRequestLayer* req)
    {
        this->requestStats.numOfMessages++;
        // extract hostname and add to hostname count map
        pcpp::HeaderField* hostField = req->getFieldByName(PCPP_HTTP_HOST_FIELD);
        if (hostField != NULL)
        {
            this->requestStats.hostnameCount[QString::fromStdString(hostField->getFieldValue())]++;
        }
        this->requestStats.methodCount[req->getFirstLine()->getMethod()]++;
    }

    void HttpStatsCollector::collectResponseStats(HttpResponseLayer* res)
    {
        this->responseStats.numOfMessages++;

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

            this->responseStats.contentTypeCount[contentType]++;
        }

        // collect status code - create one string from status code and status description (for example: 200 OK)
        std::ostringstream stream;
        stream << res->getFirstLine()->getStatusCodeAsInt();
        std::string statusCode = stream.str() + " " + res->getFirstLine()->getStatusCodeString();
        this->responseStats.statusCodeCount[statusCode]++;
    }

    bool HttpStatsCollector::tryCollectStats(Packet *httpPacket)
    {
        // verify packet is TCP
        if (!httpPacket->isPacketOfType(pcpp::TCP))
        {
            return false;
        }
        // verify packet is port 80
        pcpp::TcpLayer* tcpLayer = httpPacket->getLayerOfType<pcpp::TcpLayer>();
        if (!(tcpLayer->getTcpHeader()->portDst == htons(this->dstPort) || tcpLayer->getTcpHeader()->portSrc == htons(this->dstPort)))
        {
            return false;
        }
        // collect general HTTP traffic stats on this packet
        uint32_t hashVal = collectHttpTrafficStats(httpPacket);

        // if packet is an HTTP request - collect HTTP request stats on this packet
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

    void HttpStatsCollector::clear()
    {
        this->requestStats.clear();
        this->responseStats.clear();
    }

    HttpRequestStats& HttpStatsCollector::getRequestStats()
    {
        return this->requestStats;
    }

    HttpResponseStats& HttpStatsCollector::getResponseStats()
    {
        return this->responseStats;
    }
}

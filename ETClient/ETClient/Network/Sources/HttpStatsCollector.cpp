#include "HttpStatsCollector.h"


namespace pcpp
{
    HttpStatsCollector::HttpStatsCollector(uint16_t dstPort):m_DstPort(dstPort)
    {
        this->clear();
    }

    HttpStatsCollector::~HttpStatsCollector()
    {
    }

    uint32_t HttpStatsCollector::collectHttpTrafficStats(Packet *httpPacket)
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

    void HttpStatsCollector::collectHttpGeneralStats(TcpLayer *tcpLayer, HttpMessage *message, uint32_t flowKey)
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

    void HttpStatsCollector::collectRequestStats(HttpRequestLayer *req)
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

    void HttpStatsCollector::collectResponseStats(HttpResponseLayer *res)
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


    bool HttpStatsCollector::tryCollectStats(Packet *httpPacket)
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

    void HttpStatsCollector::calcRates()
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

    void HttpStatsCollector::clear()
    {
        m_RequestStats.clear();
        m_PrevRequestStats.clear();
        m_ResponseStats.clear();
        m_PrevResponseStats.clear();
        m_LastCalcRateTime = getCurTime();
        m_StartTime = m_LastCalcRateTime;
    }

    HttpRequestStats &HttpStatsCollector::getRequestStats()
    {
        return m_RequestStats;
    }

    HttpResponseStats &HttpStatsCollector::getResponseStats()
    {
        return m_ResponseStats;
    }
}

#pragma once

#include <SSLLayer.h>
#include "SSLStatsCollector.h"


namespace pcpp
{
    SSLStatsCollector::SSLStatsCollector()
    {
        this->clear();
    }

    SSLStatsCollector::~SSLStatsCollector()
    {
    }

    uint32_t SSLStatsCollector::collectSSLTrafficStats(Packet *sslpPacket)
    {
    //		pcpp::TcpLayer* tcpLayer = sslpPacket->getLayerOfType<pcpp::TcpLayer>();
        // calculate a hash key for this flow to be used in the flow table
        uint32_t hashVal = hash5Tuple(sslpPacket);
        // if flow is a new flow (meaning it's not already in the flow table)
        if (m_FlowTable.find(hashVal) == m_FlowTable.end())
        {
            m_FlowTable[hashVal].clear();
        }
        return hashVal;
    }

    void SSLStatsCollector::collectSSLStats(Packet* sslPacket, uint32_t flowKey)
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

    void SSLStatsCollector::collecClientHelloStats(SSLClientHelloMessage *clientHelloMessage)
    {
        m_ClientHelloStats.numOfMessages++;

        pcpp::SSLServerNameIndicationExtension* sniExt = clientHelloMessage->getExtensionOfType<pcpp::SSLServerNameIndicationExtension>();
        if (sniExt != NULL)
            m_ClientHelloStats.serverNameCount[sniExt->getHostName()]++;
    }

    bool SSLStatsCollector::tryCollectStats(Packet* parsedPacket)
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

    void SSLStatsCollector::calcRates()
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

    void SSLStatsCollector::clear()
    {
        m_ClientHelloStats.clear();
        m_PrevClientHelloStats.clear();
        m_ServerHelloStats.clear();
        m_PrevServerHelloStats.clear();
        m_LastCalcRateTime = getCurTime();
        m_StartTime = m_LastCalcRateTime;
    }

    ClientHelloStats &SSLStatsCollector::getClientHelloStats()
    {
        return m_ClientHelloStats;
    }

    ServerHelloStats &SSLStatsCollector::getServerHelloStats()
    {
        return m_ServerHelloStats;
    }
}

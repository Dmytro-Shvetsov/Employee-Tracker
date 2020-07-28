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

    uint32_t SSLStatsCollector::collectSSLTrafficStats(Packet* sslpPacket)
    {
        // calculate a hash key for this flow to be used in the flow table
        uint32_t hashVal = hash5Tuple(sslpPacket);
        // if flow is a new flow (meaning it's not already in the flow table)
        if (this->flowTable.find(hashVal) == this->flowTable.end())
        {
            this->flowTable[hashVal].clear();
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
                if (this->flowTable[flowKey].seenAlertPacket == false)
                {
                    this->flowTable[flowKey].seenAlertPacket = true;
                }
            }

            // check if the layer is an app data message
            else if (recType == pcpp::SSL_APPLICATION_DATA)
            {
                // if it's the first app data message seen on this flow it means handshake was completed
                if (this->flowTable[flowKey].seenAppDataPacket == false)
                {
                    this->flowTable[flowKey].seenAppDataPacket = true;
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
                    this->collecClientHelloStats(clientHelloMessage);
                }
            }
            sslLayer = sslPacket->getNextLayerOfType<pcpp::SSLLayer>(sslLayer);
        }
    }

    void SSLStatsCollector::collecClientHelloStats(SSLClientHelloMessage *clientHelloMessage)
    {
        this->clientHelloStats.numOfMessages++;

        pcpp::SSLServerNameIndicationExtension* sniExt = clientHelloMessage->getExtensionOfType<pcpp::SSLServerNameIndicationExtension>();
        if (sniExt != NULL)
            this->clientHelloStats.serverNameCount[QString::fromStdString(sniExt->getHostName())]++;
    }

    bool SSLStatsCollector::tryCollectStats(Packet* parsedPacket)
    {
        // verify packet is TCP and SSL/TLS
        if (!parsedPacket->isPacketOfType(pcpp::TCP) || !parsedPacket->isPacketOfType(pcpp::SSL))
            return false;

        // collect general SSL traffic stats on this packet
        uint32_t hashVal = this->collectSSLTrafficStats(parsedPacket);

        // if packet contains one or more SSL messages, collect stats on them
        if (parsedPacket->isPacketOfType(pcpp::SSL))
        {
            this->collectSSLStats(parsedPacket, hashVal);
        }
        return true;
    }

    void SSLStatsCollector::clear()
    {
        this->clientHelloStats.clear();
        this->serverHelloStats.clear();
    }

    ClientHelloStats &SSLStatsCollector::getClientHelloStats()
    {
        return this->clientHelloStats;
    }

    ServerHelloStats &SSLStatsCollector::getServerHelloStats()
    {
        return this->serverHelloStats;
    }
}

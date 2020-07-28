#pragma once

#include <SSLLayer.h>
#include "BaseStatsCollector.h"


namespace pcpp
{
    struct ClientHelloStats
    {
        int numOfMessages; // total number of client-hello messages
        std::map<QString, quint32> serverNameCount; // a map for counting the server names seen in traffic

        virtual ~ClientHelloStats() {}

        virtual void clear()
        {
            this->serverNameCount.clear();
            this->numOfMessages = 0;
        }
    };

    /**
     * A base struct for collecting stats on server-hello messages
     */
    struct ServerHelloStats
    {
        int numOfMessages; // total number of server-hello messages

        virtual ~ServerHelloStats() {}

        virtual void clear()
        {
            this->numOfMessages = 0;
        }
    };

    /**
     * The SSL stats collector.
     */
    class SSLStatsCollector : public BaseStatsCollector
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
                this->seenAppDataPacket = false;
                this->seenAlertPacket = false;
            }
        };

        ClientHelloStats clientHelloStats;
        ServerHelloStats serverHelloStats;

        std::map<uint32_t, SSLFlowData> flowTable;

        /**
         * Collect stats relevant for every SSL packet (any SSL message)
         * This method calculates and returns the flow key for this packet
         */
        uint32_t collectSSLTrafficStats(pcpp::Packet* sslpPacket);

        /**
         * Collect stats relevant for several kinds SSL messages
         */
        void collectSSLStats(pcpp::Packet* sslPacket, uint32_t flowKey);

        /**
         * Collect stats relevant only to client-hello messages
         */
        void collecClientHelloStats(pcpp::SSLClientHelloMessage* clientHelloMessage);

    public:
        /**
         * C'tor - clear all structures
         */
        SSLStatsCollector();
        virtual ~SSLStatsCollector();
        bool tryCollectStats(pcpp::Packet* parsedPacket)override;
        void clear()override;

        /**
         * Get client-hello stats
         */
        ClientHelloStats& getClientHelloStats();

        /**
         * Get server-hello stats
         */
        ServerHelloStats& getServerHelloStats();
    };
}

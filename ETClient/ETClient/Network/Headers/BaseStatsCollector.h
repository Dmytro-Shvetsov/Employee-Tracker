#pragma once

#include <QMap>
#include <sstream>
#include <TcpLayer.h>
#include <IPv4Layer.h>
#include <PayloadLayer.h>
#include <PacketUtils.h>
#include <SystemUtils.h>


namespace pcpp
{
    /**
     * The abstract stats collector class. Should be called for every packet arriving and also periodically to calculate rates
     */
    class BaseStatsCollector
    {
    protected:
        double m_LastCalcRateTime;
        double m_StartTime;

    public:
        /**
         * Collect stats for a single packet
         */
        virtual bool tryCollectStats(pcpp::Packet* parsedPacket) = 0;

        /**
         * Clear all stats collected so far
         */
        virtual void clear() = 0;
    };
}

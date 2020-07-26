#pragma once

#include <map>
#include <sstream>
#include <TcpLayer.h>
#include <IPv4Layer.h>
#include <PayloadLayer.h>
#include <PacketUtils.h>
#include <SystemUtils.h>


namespace pcpp
{
    /**
     * An auxiliary struct for encapsulating rate stats
     */
    struct Rate;

    #ifndef s_Rate
    #define s_Rate
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
         * Calculate rates. Should be called periodically
         */
        virtual void calcRates() = 0;

        /**
         * Clear all stats collected so far
         */
        virtual void clear() = 0;

        double getCurTime(void);
    };
}

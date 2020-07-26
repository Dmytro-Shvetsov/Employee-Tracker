#include "BaseStatsCollector.h"


namespace pcpp
{
    double BaseStatsCollector::getCurTime()
    {
        struct timeval tv;

        gettimeofday(&tv, NULL);

        return (((double)tv.tv_sec) + (double)(tv.tv_usec / 1000000.0));
    }
}

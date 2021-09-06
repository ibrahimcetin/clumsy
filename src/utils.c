#include <stdlib.h>
#include <Windows.h>
#include "common.h"

short calcChance(short chance)
{
    // notice that here we made a copy of chance, so even though it's volatile it is still ok
    return (chance == 10000) || ((rand() % 10000) < chance);
}

static short resolutionSet = 0;

void startTimePeriod()
{
    if (!resolutionSet)
    {
        // begin only fails when period out of range
        timeBeginPeriod(TIMER_RESOLUTION);
        resolutionSet = 1;
    }
}

void endTimePeriod()
{
    if (resolutionSet)
    {
        timeEndPeriod(TIMER_RESOLUTION);
        resolutionSet = 0;
    }
}

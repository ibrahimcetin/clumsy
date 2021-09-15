#include <stdlib.h>
#include <Windows.h>
#include <unistd.h>
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

int parseArgs(int argc, char *argv[], char *filterText, ModuleData dataOfModules[MODULE_CNT], DWORD *runTimePointer)
{
    int opt;

    int moduleIndex = 0;
    int valueIndex = 0;
    while ((opt = getopt(argc, argv, ":f:m:v:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            strcpy(filterText, optarg);
            break;
        case 'm':
            dataOfModules[moduleIndex] = (ModuleData){optarg, 0};
            moduleIndex += 1;
            break;
        case 'v':
            dataOfModules[valueIndex].value = strtol(optarg, NULL, 10);
            valueIndex += 1;
            break;
        case ':':
            printf("Option needs a value: %c\n", optopt);
            break;
        case '?':
            printf("Unknown option: %c\n"
                   "Please, check help section.\n",
                   optopt);
            exit(1);
            break;
        }
    }

    *runTimePointer = strtol(argv[optind], NULL, 10) * 1000;

    return moduleIndex == valueIndex ? moduleIndex : -1; // return number of modules
}
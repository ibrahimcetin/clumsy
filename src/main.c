#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "common.h"

Module *modules[MODULE_CNT] = {
    &lagModule,
    &dropModule,
    &oodModule,
    &bandwidthModule,
};

static BOOL checkIsRunning()
{
    //It will be closed and destroyed when programm terminates (according to MSDN).
    HANDLE hStartEvent = CreateEventW(NULL, FALSE, FALSE, L"Global\\CLUMSY_IS_RUNNING_EVENT_NAME");

    if (hStartEvent == NULL)
        return TRUE;

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(hStartEvent);
        hStartEvent = NULL;
        return TRUE;
    }

    return FALSE;
}

// in fact only 32bit binary would run on 64 bit os
// if this happens pop out message box and exit
static BOOL check32RunningOn64()
{
    BOOL is64ret;
    // consider IsWow64Process return value
    if (IsWow64Process(GetCurrentProcess(), &is64ret) && is64ret)
    {
        return TRUE;
    }
    return FALSE;
}

static int checkRequirements()
{
    BOOL exit;

    exit = checkIsRunning();
    if (exit)
    {
        LOG("Theres' already an instance of clumsy running.\nAborting");
        return exit;
    }

#ifdef _WIN32
    exit = check32RunningOn64();
    if (exit)
    {
        LOG("You're running 32bit clumsy on 64bit Windows, which wouldn't work. Please use the 64bit clumsy version.\nAborting");
        return exit;
    }
#endif

    exit = tryElevate(TRUE);
    if (exit)
    {
        LOG("Clumsy needs to be elevated to work. Run Terminal as Administrator.\nAborting");
        return exit;
    }

    return FALSE;
}

static void startClumsy(char *filterText)
{
    char buf[MSG_BUFSIZE];

    divertStart(filterText, buf);

    LOG("Started filtering. Enable functionalities to take effect.");
}

int main(int argc, char *argv[])
{
    LOG("Is Run As Admin: %d", IsRunAsAdmin());
    LOG("Is Elevated: %d", IsElevated());
    srand((unsigned int)time(NULL));

    BOOL exit = checkRequirements();
    if (exit)
        return 1;

    ModuleData dataOfModules[MODULE_CNT];
    char *filterText = malloc(sizeof(char) * 512);
    DWORD runTime = 0;
    DWORD *runTimePointer = &runTime;

    int numberOfModules = parseArgs(argc, argv, filterText, dataOfModules, runTimePointer);

    UINT ix;
    for (ix = 0; ix < MODULE_CNT; ++ix)
    {
        Module *module = *(modules + ix);

        int im;
        for (im = 0; im < numberOfModules; im++)
        {
            ModuleData moduleData = dataOfModules[im];

            if (strcmp(module->shortName, moduleData.name) == 0)
            {
                if (moduleData.value > 0)
                {
                    *(module->enabledFlag) = 1;

                    int moduleValue = moduleData.value;
                    if (!strcmp(module->shortName, "drop") || !strcmp(module->shortName, "ood"))
                        moduleValue *= 100;
                    *(module->value) = moduleValue;
                }
            }
        }
    }

    DWORD startTime = timeGetTime();
    DWORD currentTime, currentRunTime;

    startClumsy(filterText);

    while (1)
    {
        if (runTime)
        {
            currentTime = timeGetTime();
            currentRunTime = currentTime - startTime;
            if (currentRunTime >= runTime)
                break;
        }

        SleepEx(1000, 0);
    }

    divertStop();

    return 0;
}

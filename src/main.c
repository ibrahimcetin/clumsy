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

void setModuleValue(UINT moduleIndex, short value)
{
    short *valuePointer = (modules[moduleIndex]->value);
    InterlockedExchange16(valuePointer, (short)value);
}

int main(int argc, char *argv[])
{
    LOG("Is Run As Admin: %d", IsRunAsAdmin());
    LOG("Is Elevated: %d", IsElevated());
    srand((unsigned int)time(NULL));

    BOOL exit = checkRequirements();
    if (exit)
        return 1;

    if (argc != 5)
        return 1; // TODO print help message

    char *filterText = argv[1];

    UINT ix;
    for (ix = 0; ix < MODULE_CNT; ++ix)
    {
        Module *module = *(modules + ix);

        if (strcmp(module->shortName, argv[2]) == 0)
        {
            DWORD startTime = timeGetTime();
            DWORD currentTime, currentRunTime;

            *(module->enabledFlag) = 1;
            startClumsy(filterText);

            int moduleValue = strtol(argv[3], NULL, 10);
            if (!strcmp(module->shortName, "drop") || !strcmp(module->shortName, "ood"))
                moduleValue *= 100;
            setModuleValue(ix, moduleValue);

            DWORD runTime = strtol(argv[4], NULL, 10) * 1000;
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

            break;
        }
    }

    return 0;
}

// Reset injection packet module
#include <stdlib.h>
#include "common.h"
#define NAME "reset"

static const unsigned int TCP_MIN_SIZE = sizeof(WINDIVERT_IPHDR) + sizeof(WINDIVERT_TCPHDR);

static volatile short resetEnabled = 0,
                      resetInbound = 1, resetOutbound = 1,
                      chance = 0, // [0-10000]
    setNextCount = 0;

static void resetStartup()
{
    LOG("reset enabled");
    InterlockedExchange16(&setNextCount, 0);
}

static void resetCloseDown(PacketNode *head, PacketNode *tail)
{
    UNREFERENCED_PARAMETER(head);
    UNREFERENCED_PARAMETER(tail);
    LOG("reset disabled");
    InterlockedExchange16(&setNextCount, 0);
}

static short resetProcess(PacketNode *head, PacketNode *tail)
{
    short reset = FALSE;
    PacketNode *pac = head->next;
    while (pac != tail)
    {
        if (checkDirection(pac->addr.Outbound, resetInbound, resetOutbound) && pac->packetLen > TCP_MIN_SIZE && (setNextCount || calcChance(chance)))
        {
            PWINDIVERT_TCPHDR pTcpHdr;
            WinDivertHelperParsePacket(
                pac->packet,
                pac->packetLen,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                &pTcpHdr,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL);

            if (pTcpHdr != NULL)
            {
                LOG("injecting reset w/ chance %.1f%%", chance / 100.0);
                pTcpHdr->Rst = 1;
                WinDivertHelperCalcChecksums(pac->packet, pac->packetLen, NULL, 0);

                reset = TRUE;
                if (setNextCount > 0)
                {
                    InterlockedDecrement16(&setNextCount);
                }
            }
        }

        pac = pac->next;
    }
    return reset;
}

Module resetModule = {
    "Set TCP RST",
    NAME,
    (short *)&resetEnabled,
    (short *)&chance,
    resetStartup,
    resetCloseDown,
    resetProcess,
    // runtime fields
    0,
    0,
};
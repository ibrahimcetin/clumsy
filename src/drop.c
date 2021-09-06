// dropping packet module
#include <stdlib.h>
#include <Windows.h>
#include "common.h"
#define NAME "drop"

static volatile short dropEnabled = 0,
                      dropInbound = 1, dropOutbound = 1,
                      chance = 1000; // [0-10000]

static void dropStartUp()
{
    LOG("drop enabled");
}

static void dropCloseDown(PacketNode *head, PacketNode *tail)
{
    UNREFERENCED_PARAMETER(head);
    UNREFERENCED_PARAMETER(tail);
    LOG("drop disabled");
}

static short dropProcess(PacketNode *head, PacketNode *tail)
{
    int dropped = 0;
    while (head->next != tail)
    {
        PacketNode *pac = head->next;
        // chance in range of [0, 10000]
        if (checkDirection(pac->addr.Outbound, dropInbound, dropOutbound) && calcChance(chance))
        {
            LOG("dropped with chance %.1f%%, direction %s",
                chance / 100.0, pac->addr.Outbound ? "OUTBOUND" : "INBOUND");
            freeNode(popNode(pac));
            ++dropped;
        }
        else
        {
            head = head->next;
        }
    }

    return dropped > 0;
}

Module dropModule = {
    "Drop",
    NAME,
    (short *)&dropEnabled,
    (short *)&chance,
    dropStartUp,
    dropCloseDown,
    dropProcess,
    // runtime fields
    0,
    0,
};

// duplicate packet module
#include <stdlib.h>
#include "common.h"
#define NAME "duplicate"
#define COPIES_MIN "2"
#define COPIES_MAX "50"
#define COPIES_COUNT 2

static volatile short dupEnabled = 0,
                      dupInbound = 1, dupOutbound = 1,
                      chance = 1000, // [0-10000]
    count = COPIES_COUNT;            // how many copies to duplicate

static void dupStartup()
{
    LOG("dup enabled");
}

static void dupCloseDown(PacketNode *head, PacketNode *tail)
{
    UNREFERENCED_PARAMETER(head);
    UNREFERENCED_PARAMETER(tail);
    LOG("dup disabled");
}

static short dupProcess(PacketNode *head, PacketNode *tail)
{
    short duped = FALSE;
    PacketNode *pac = head->next;
    while (pac != tail)
    {
        if (checkDirection(pac->addr.Outbound, dupInbound, dupOutbound) && calcChance(chance))
        {
            short copies = count - 1;
            LOG("duplicating w/ chance %.1f%%, cloned additionally %d packets", chance / 100.0, copies);
            while (copies--)
            {
                PacketNode *copy = createNode(pac->packet, pac->packetLen, &(pac->addr));
                insertBefore(copy, pac); // must insertBefore or next packet is still pac
            }
            duped = TRUE;
        }
        pac = pac->next;
    }
    return duped;
}

Module dupModule = {
    "Duplicate",
    NAME,
    (short *)&dupEnabled,
    (short *)&chance,
    dupStartup,
    dupCloseDown,
    dupProcess,
    // runtime fields
    0,
    0,
};
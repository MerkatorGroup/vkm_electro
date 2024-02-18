#include <stdint.h>
#include <UserCAPI.h>
#include <gsOsyServer.h>
#include "vartab.h"

void gsOsyPrivWriteEvent(const uint8_t dataPoolIndex, const uint16_t listIndex, const uint16_t elementIndex)
{
    if(GSEVISU_RUNTIME_DATA_DATA_POOL_INDEX == dataPoolIndex)
    {
        if(GSEVISU_RUNTIME_DATA_LIST_INDEX_RUNTIMEDATA == listIndex)
        {
            // Empty, nothing to do here.
        }
        else if(GSEVISU_RUNTIME_DATA_LIST_INDEX_VARIABLES == listIndex)
        {
            // Empty, nothing to do here.
        }
    }
}

int16_t gsOsyPrivReadEvent(const uint8_t dataPoolIndex, const uint16_t listIndex, const uint16_t elementIndex)
{
    if(GSEVISU_RUNTIME_DATA_DATA_POOL_INDEX == dataPoolIndex)
    {
        if(GSEVISU_RUNTIME_DATA_LIST_INDEX_RUNTIMEDATA == listIndex)
        {
            // Empty, nothing to do here.
        }
        else if(GSEVISU_RUNTIME_DATA_LIST_INDEX_VARIABLES == listIndex)
        {
            // Empty, nothing to do here.
        }
    }
    return 0;
}


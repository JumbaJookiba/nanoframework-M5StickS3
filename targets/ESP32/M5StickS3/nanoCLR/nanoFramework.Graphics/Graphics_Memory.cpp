//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef GRAPHICS_MEMORY_SETUP_
#define GRAPHICS_MEMORY_SETUP_

#include <nanoPAL.h>
#include <target_platform.h>
#include <esp32_idf.h>
#include <Esp32_DeviceMapping.h>
#include <GraphicsMemoryHeap.h>

struct GraphicsMemory g_GraphicsMemory;

// M5StickS3 has 8MB OPI PSRAM which is used for graphics memory.
// The PSRAM is mapped into the data address space starting at 0x3C000000.
// Applications can manually place data in external memory by creating
// pointers to this region. The graphics subsystem allocates from SPIRAM
// to keep the internal RAM free for the CLR.

static CLR_UINT8 *heapStartingAddress = 0;
static CLR_UINT8 *heapEndingAddress = 0;

bool GraphicsMemory::GraphicsHeapLocation(
    CLR_UINT32 requested,
    CLR_UINT8 *&graphicsStartingAddress,
    CLR_UINT8 *&graphicsEndingAddress)
{
    CLR_UINT32 graphicsMemoryBlockSize = requested;

    CLR_INT32 memoryCaps = MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM;

    if (heapStartingAddress != 0)
    {
        graphicsStartingAddress = heapStartingAddress;
        graphicsEndingAddress = heapEndingAddress;
        return true;
    }

    // We don't want to allocate upfront
    if (graphicsMemoryBlockSize == 0)
    {
        // We don't allocate anything here
        return false;
    }

    // Get Largest free block in SPIRam
    CLR_UINT32 spiramMaxSize = heap_caps_get_largest_free_block(memoryCaps);

    if (spiramMaxSize == 0)
    {
        // No SPIRAM, try and allocate small block in normal ram to keep allocator happy for
        // people trying to run graphics on boards without SPIRAM
        // Should be able to use with small screens
        memoryCaps ^= MALLOC_CAP_SPIRAM;

        spiramMaxSize = heap_caps_get_largest_free_block(memoryCaps);
    }

    if (spiramMaxSize < graphicsMemoryBlockSize) // limit the size to what is available
    {
        graphicsMemoryBlockSize = spiramMaxSize;
    }

    graphicsStartingAddress = (CLR_UINT8 *)heap_caps_malloc(graphicsMemoryBlockSize, memoryCaps);

    ASSERT(graphicsStartingAddress != NULL);
    graphicsEndingAddress = (CLR_UINT8 *)(graphicsStartingAddress + graphicsMemoryBlockSize);

    // Save where we allocated it for restarts
    heapStartingAddress = graphicsStartingAddress;
    heapEndingAddress = graphicsEndingAddress;

    return true;
}

#endif // GRAPHICS_MEMORY_SETUP_

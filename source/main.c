#include "screen.h"

#include <coreinit/screen.h>
#include <coreinit/memory.h>
#include <coreinit/cache.h>
#include <whb/proc.h>

int main(int argc, char** argv) {
    WHBProcInit();
    OSScreenInit();
    size_t tvBufferSize = OSScreenGetBufferSizeEx(SCREEN_TV);
    size_t drcBufferSize = OSScreenGetBufferSizeEx(SCREEN_DRC);

    void* tvBuffer = memalign(0x100, tvBufferSize);
    void* drcBuffer = memalign(0x100, drcBufferSize);

    OSScreenSetBufferEx(SCREEN_TV, tvBuffer);
    OSScreenSetBufferEx(SCREEN_DRC, drcBuffer);

    OSScreenEnableEx(SCREEN_TV, true);
    OSScreenEnableEx(SCREEN_DRC, true);
    initFont(NULL, 0);

    while(WHBProcIsRunning()) {
        OSScreenClearBufferEx(SCREEN_TV, 0x00000000);
        OSScreenClearBufferEx(SCREEN_DRC, 0x00000000);

        console_print_pos_aligned(3, 0, 1, "Savemii Mod");
        DCFlushRange(tvBuffer, tvBufferSize);
        DCFlushRange(drcBuffer, drcBufferSize);

        OSScreenFlipBuffersEx(SCREEN_TV);
        OSScreenFlipBuffersEx(SCREEN_DRC);
    }

    if (tvBuffer) free(tvBuffer);
    if (drcBuffer) free(drcBuffer);

    OSScreenShutdown();
    WHBProcShutdown();
    return 1;
}
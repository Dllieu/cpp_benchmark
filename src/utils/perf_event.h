#pragma once

#include <cstdint>
// Explicit include
#include <linux/perf_event.h>

namespace perf
{
    class PerfEvent
    {
    public:
        PerfEvent( perf_hw_id hardwareEvent );
        ~PerfEvent();

        bool        start();
        uint64_t    stop();

    private:
        int     fd_;
    };
}

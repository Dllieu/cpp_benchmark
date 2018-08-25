#pragma once

#include <cstdint>
#include <linux/perf_event.h>
#include <utils/macros.h>

namespace experimental
{
    class PerfEvent
    {
    public:
        explicit PerfEvent(perf_hw_id hardwareEvent);
        DELETE_COPY_MOVE_CONSTRUCTOR(PerfEvent);
        ~PerfEvent();

        bool start();
        uint64_t stop();

    private:
        int m_Fd;
    };
}

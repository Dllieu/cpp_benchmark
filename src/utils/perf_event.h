#pragma once

#include <sys/types.h>
// Explicit include
#include <linux/perf_event.h>

namespace perf
{
    long int event_open( perf_event_attr& hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags );
}

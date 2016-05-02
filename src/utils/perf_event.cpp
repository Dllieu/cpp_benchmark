#include "perf_event.h"

#include <sys/syscall.h>
#include <unistd.h>

namespace perf
{
    long int event_open( perf_event_attr& hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags )
    {
        return syscall( __NR_perf_event_open, &hw_event, pid, cpu, group_fd, flags );
    }
}

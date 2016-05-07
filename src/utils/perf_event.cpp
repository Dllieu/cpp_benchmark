#include "perf_event.h"

#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>

#include "macros.h"

namespace
{
    // pid == 0 and cpu == -1:  This measures the calling process/thread on any CPU.
    // pid == 0 and cpu >= 0:   This measures the calling process/thread only when running on the specified CPU.
    // pid > 0 and cpu == -1:   This measures the specified process/thread on any CPU.
    // pid > 0 and cpu >= 0:    This measures the specified process/thread only when running on the specified CPU.
    // pid == -1 and cpu >= 0:  This measures all processes/threads on the specified CPU. This requires CAP_SYS_ADMIN capability or a /proc/sys/kernel/perf_event_paranoid value of less than 1.
    // pid == -1 and cpu == -1: This setting is invalid and will return an error.
    int     perf_event_open( perf_hw_id hardwareEvent, pid_t pid, int cpu, int group_fd, unsigned long flags )
    {
        perf_event_attr pe;

        memset( &pe, 0, sizeof( perf_event_attr ) );

        pe.type = PERF_TYPE_HARDWARE;
        pe.size = sizeof( perf_event_attr );
        pe.config = hardwareEvent;

        pe.disabled = 1;
        pe.exclude_kernel = 1;
        pe.exclude_hv = 1;

        return syscall( __NR_perf_event_open, &pe, pid, cpu, group_fd, flags );
    }
}

using namespace perf;

PerfEvent::PerfEvent( perf_hw_id hardwareEvent )
    : fd_( perf_event_open( hardwareEvent, 0, -1, -1, 0 ) )
{
    // NOTHING
}

PerfEvent::~PerfEvent()
{
    if ( likely( fd_ >= 0 ) )
        close( fd_ );
}

bool    PerfEvent::start()
{
    if ( unlikely( fd_ < 0 ) )
        return false;

    ioctl( fd_, PERF_EVENT_IOC_RESET, 0 );
    ioctl( fd_, PERF_EVENT_IOC_ENABLE, 0 );
    return true;
}

uint64_t    PerfEvent::stop()
{
    if ( unlikely( fd_ < 0 ) )
        return 0;

    ioctl( fd_, PERF_EVENT_IOC_DISABLE, 0 );
    uint64_t result;
    if ( unlikely( sizeof( result ) != read( fd_, &result, sizeof( result ) ) ) )
        return result;

    return result;
}

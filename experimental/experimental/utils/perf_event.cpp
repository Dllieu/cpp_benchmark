#include <string>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <utils/macros.h>
#include <utils/perf_event.h>
#include <cstring>
#include <cstddef>

namespace
{
    // pid == 0 and cpu == -1:  This measures the calling process/thread on any CPU.
    // pid == 0 and cpu >= 0:   This measures the calling process/thread only when running on the specified CPU.
    // pid > 0 and cpu == -1:   This measures the specified process/thread on any CPU.
    // pid > 0 and cpu >= 0:    This measures the specified process/thread only when running on the specified CPU.
    // pid == -1 and cpu >= 0:  This measures all processes/threads on the specified CPU. This requires CAP_SYS_ADMIN capability or a /proc/sys/kernel/perf_event_paranoid value of less than 1.
    // pid == -1 and cpu == -1: This setting is invalid and will return an error.
    int     perf_event_open( perf_hw_id hardwareEvent, pid_t pid, int cpu, int group_fd, std::uint64_t flags )
    {
        perf_event_attr pe{};

        pe.type = PERF_TYPE_HARDWARE;
        pe.size = sizeof( perf_event_attr );
        pe.config = hardwareEvent;

        pe.disabled = 1;
        pe.exclude_kernel = 1;
        pe.exclude_hv = 1;

        return syscall( __NR_perf_event_open, &pe, pid, cpu, group_fd, flags );
    }
}

namespace experimental
{
    PerfEvent::PerfEvent( perf_hw_id hardwareEvent )
        : m_Fd( perf_event_open( hardwareEvent, 0, -1, -1, 0 ) )
    {
        // NOTHING
    }

    PerfEvent::~PerfEvent()
    {
        if ( likely( this->m_Fd >= 0 ) )
        {
            close( this->m_Fd );
        }
    }

    bool    PerfEvent::start()
    {
        if ( unlikely( this->m_Fd < 0 ) )
        {
            return false;
        }

        ioctl( this->m_Fd, PERF_EVENT_IOC_RESET, 0 );
        ioctl( this->m_Fd, PERF_EVENT_IOC_ENABLE, 0 );

        return true;
    }

    uint64_t    PerfEvent::stop()
    {
        if ( unlikely( this->m_Fd < 0 ) )
        {
            return 0;
        }

        ioctl( this->m_Fd, PERF_EVENT_IOC_DISABLE, 0 );

        uint64_t result;

        if ( unlikely( sizeof( result ) != read( this->m_Fd, &result, sizeof( result ) ) ) )
        {
            return result;
        }

        return result;
    }
}

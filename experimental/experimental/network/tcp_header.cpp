#include <network/tcp_header.h>
#include <utils/bits.h>

namespace experimental
{
    bool DataOffSetWithFlags::GetNS() const
    {
        return GetBit(this->m_DataOffsetWithNS, 7);
    }

    bool DataOffSetWithFlags::GetCWR() const
    {
        return GetBit(this->m_ControlBits, 0);
    }

    bool DataOffSetWithFlags::GetECE() const
    {
        return GetBit(this->m_ControlBits, 1);
    }

    bool DataOffSetWithFlags::GetURG() const
    {
        return GetBit(this->m_ControlBits, 2);
    }

    bool DataOffSetWithFlags::GetACK() const
    {
        return GetBit(this->m_ControlBits, 3);
    }

    bool DataOffSetWithFlags::GetPSH() const
    {
        return GetBit(this->m_ControlBits, 4);
    }

    bool DataOffSetWithFlags::GetRST() const
    {
        return GetBit(this->m_ControlBits, 5);
    }

    bool DataOffSetWithFlags::GetSYN() const
    {
        return GetBit(this->m_ControlBits, 6);
    }

    bool DataOffSetWithFlags::GetFIN() const
    {
        return GetBit(this->m_ControlBits, 7);
    }
}

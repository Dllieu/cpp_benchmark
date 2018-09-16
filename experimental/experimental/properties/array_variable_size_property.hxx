#include <cstddef>
#include <properties/array_variable_size_property.h>

namespace experimental
{
    template <typename T, bool TLengthIncludeSizeOfT>
    ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT>::ArrayVariableSizePropertyIterator(const std::byte* iBuffer)
        : m_Buffer(iBuffer)
    {
    }

    template <typename T, bool TLengthIncludeSizeOfT>
    std::tuple<const T*, const std::byte*, std::size_t> ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT>::operator*() const
    {
        const T* pValue = reinterpret_cast<const T*>(this->m_Buffer);
        std::size_t length = pValue->GetLength();

        if constexpr (true == TLengthIncludeSizeOfT)
        {
            return std::make_tuple(pValue, this->m_Buffer + sizeof(T), length - sizeof(T));
        }
        else
        {
            return std::make_tuple(pValue, this->m_Buffer + sizeof(T), length);
        }
    }

    template <typename T, bool TLengthIncludeSizeOfT>
    ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT>& ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT>::operator++()
    {
        std::size_t length = reinterpret_cast<const T*>(this->m_Buffer)->GetLength();

        if constexpr (true == TLengthIncludeSizeOfT)
        {
            std::advance(this->m_Buffer, length);
        }
        else
        {
            std::advance(this->m_Buffer, length + sizeof(T));
        }

        return *this;
    }

    template <typename T, bool TLengthIncludeSizeOfT>
    bool ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT>::operator!=(const ArrayVariableSizePropertyIterator& iArrayVariableSizePropertyIterator) const
    {
        return this->m_Buffer != iArrayVariableSizePropertyIterator.m_Buffer;
    }

    template <typename T, bool TLengthIncludeSizeOfT>
    ArrayVariableSizeProperty<T, TLengthIncludeSizeOfT>::ArrayVariableSizeProperty(const std::byte* iBuffer, std::size_t iLength)
        : m_Buffer(iBuffer)
        , m_Length(iLength)
    {
    }

    template <typename T, bool TLengthIncludeSizeOfT>
    ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT> ArrayVariableSizeProperty<T, TLengthIncludeSizeOfT>::begin() const
    {
        return ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT>(this->m_Buffer);
    }

    template <typename T, bool TLengthIncludeSizeOfT>
    ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT> ArrayVariableSizeProperty<T, TLengthIncludeSizeOfT>::end() const
    {
        return ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT>(this->m_Buffer + this->m_Length);
    }
}

#pragma once

#include <cstddef>

namespace experimental
{
    template <typename T, bool TLengthIncludeSizeOfT>
    struct ArrayVariableSizePropertyIterator
    {
        explicit ArrayVariableSizePropertyIterator(const std::byte* iBuffer);

        std::tuple<const T*, const std::byte*, std::size_t> operator*() const;
        ArrayVariableSizePropertyIterator& operator++();
        bool operator!=(const ArrayVariableSizePropertyIterator& iArrayVariableSizePropertyIterator) const;

        const std::byte* m_Buffer;
    };

    template <typename T, bool TLengthIncludeSizeOfT>
    struct ArrayVariableSizeProperty
    {
        ArrayVariableSizeProperty(const std::byte* iBuffer, std::size_t iLength);

        ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT> begin() const;
        ArrayVariableSizePropertyIterator<T, TLengthIncludeSizeOfT> end() const;

        const std::byte* m_Buffer;
        std::size_t m_Length;
    };
}

#include <properties/array_variable_size_property.hxx>

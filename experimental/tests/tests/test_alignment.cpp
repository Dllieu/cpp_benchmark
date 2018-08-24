#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace
{
    // Objects of type Alignment1 can be allocated at any address
    // because both Alignment1::a and Alignment1::b can be allocated at any address
    struct Alignment1
    {
        std::byte a;
        std::byte b;
    };

    static_assert(2 == sizeof(Alignment1));
    static_assert(1 == std::alignment_of_v<Alignment1>);

    struct Alignment1Bis
    {
        std::byte a[2];
    };

    static_assert(2 == sizeof(Alignment1Bis));
    static_assert(1 == std::alignment_of_v<Alignment1Bis>);

    struct __attribute__((packed)) Alignment1Packed
    {
        std::byte b1;
        std::int32_t i;
        std::byte b2;
    };

    static_assert(6 == sizeof(Alignment1Packed));
    static_assert(1 == std::alignment_of_v<Alignment1Packed>);

#pragma pack(push, 1)
    struct Alignment1PackedBis
    {
        std::byte b1;
        std::int32_t i;
        std::byte b2;
    };
#pragma pack(pop)

    static_assert(6 == sizeof(Alignment1PackedBis));
    static_assert(1 == std::alignment_of_v<Alignment1PackedBis>);

    // Objects of type Alignment4 must be allocated at 4-byte boundaries
    // because Alignment4::i must be allocated at 4-byte boundaries
    // because int's alignment requirement is (usually) 4
    struct Alignment4
    {
        std::int16_t s;
        std::int32_t i;
        std::byte a;
    };

    static_assert(12 == sizeof(Alignment4));
    static_assert(4 == std::alignment_of_v<Alignment4>);

    struct Alignment4Bis
    {
        std::int32_t i;
        std::int16_t s;
        std::byte a;
    };

    static_assert(8 == sizeof(Alignment4Bis));
    static_assert(4 == std::alignment_of_v<Alignment4Bis>);

    // Objects of type Alignment8 must be allocated at 8-byte boundaries
    // because Alignment8::next and Alignment8::prev must be allocated at 8-byte boundaries
    // because pointer's alignment requirement is (usually) 8
    struct Alignment8
    {
        void* next;
        void* prev;
    };

    static_assert(16 == sizeof(Alignment8));
    static_assert(8 == std::alignment_of_v<Alignment8>);

    struct Alignment8Derived : Alignment8
    {
        std::byte c;
    };

    static_assert(24 == sizeof(Alignment8Derived));
    static_assert(8 == std::alignment_of_v<Alignment8Derived>);

    // Objects of type Alignment16 must be allocated at 16-byte boundaries
    // because Alignment16::ll must be allocated at 16-byte boundaries
    // because long double aignment requirement is (usually) 16
    // 16-byte alignment is (usually) the maximum alignment possible, but it can be forced to have higher alignment
    struct Alignment16
    {
        long double ll;
    };

    static_assert(16 == sizeof(Alignment16));
    static_assert(16 == std::alignment_of_v<Alignment16>);
    static_assert(16 == std::alignment_of_v<std::max_align_t>);

    struct alignas(64) Alignment64
    {
        std::byte cacheLine[64];
    };

    static_assert(64 == sizeof(Alignment64));
    static_assert(64 == std::alignment_of_v<Alignment64>);
}

TEST(AlignmentTest, Alignment) // NOLINT
{
}

namespace
{
    static_assert(0u == offsetof(Alignment1, a));
    static_assert(1u == offsetof(Alignment1, b));

    static_assert(0u == offsetof(Alignment8, next));
    static_assert(8u == offsetof(Alignment8, prev));

    // Generate warning (Non-standard-layout type) but is not undefined behaviour in C++17, and gcc handle it correctly
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    static_assert(16u == offsetof(Alignment8Derived, c));
#pragma GCC diagnostic pop
}

TEST(AlignmentTest, OffsetOf) // NOLINT
{
}

namespace
{
    // Used by gcc for certains node-based container (e.g. forward_list / unorderd_map / ...)
    // Rather than storing T, store a buffer of sizeof(T) within the node which is then used with placement new to construct T*
    template <typename T>
    struct AlignedBuffer
    {
        typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type m_Storage;

        void* Address() noexcept
        {
            return static_cast<void*>(std::addressof(this->m_Storage));
        }

        const void* Address() const noexcept
        {
            return static_cast<const void*>(std::addressof(this->m_Storage));
        }
    };
}

TEST(AlignmentTest, AlignedStorage) // NOLINT
{
    AlignedBuffer<Alignment1> alignedBuffer{};
    static_assert(1 == std::alignment_of_v<decltype(alignedBuffer)>);

    Alignment1* alignment1 = new (alignedBuffer.Address()) Alignment1();
    alignment1->a = std::byte(0);

    EXPECT_EQ(alignment1, alignedBuffer.Address());

    std::aligned_storage<sizeof(std::byte) * 512, 8>::type storage{};
    static_assert(8 == std::alignment_of_v<decltype(storage)>);

    Alignment8* alignment8 = new (std::addressof(storage)) Alignment8();
    alignment8->next = nullptr;
}

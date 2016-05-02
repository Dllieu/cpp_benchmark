#pragma once

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#define HOT             __attribute__((hot))

//namespace
//{
//    void escape(void * p)
//    {
//        asm volatile("" : : "g"(p) /**input from c++ to c*/: "memory"/*clobber: what part of the program is modified while this assembly run*/);
//    }
//
//    void clobber()
//    {
//        asm volatile("" : : : "memory"); // write in all memory
//    }
//}

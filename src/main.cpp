#include <benchmark/benchmark.h>

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

BENCHMARK_MAIN();

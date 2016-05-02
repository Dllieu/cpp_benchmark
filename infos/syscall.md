#[get|set]rlimit
limits resource usage of a process
```c++
rlimit rl;
auto r = getrlimit(RLIMIT_STACK, &rl);
```

#perf_event_open
set up performance monitoring

#TODO
taskset / pthread: sched_setaffinity - http://stackoverflow.com/questions/14170127/pros-and-cons-of-cpu-affinity
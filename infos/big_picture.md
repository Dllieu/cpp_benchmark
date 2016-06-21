#Big Picture
*to take with a grain of salt*

|operation|time|
|:--|:--|
|1 CPU cycle|0.3 ns|
|Level 1 cache access (48 bytes per cycle in Sandy bridge)|0.9 ns|
|Level 2 cache access|3.1 ns|
|Branch mispredict|5 ns|
|Level 3 cache access|7.7 ns|
|Atomic lock|8.25 ns|
|Optimal spinlock lock/unlock|16 ns|
|UserSpace syscall|25 ns|
|Main Memory access|110 ns|
|OS network stack (half round-trip) Kernel bypass|1 micro|
|Roundtrip 100 bytes over 1 Gbps proximity network with Solarflare|3.5 micros|
|OS network stack (half round-trip)|5 micros|
|Send 1K bytes over 1 Gbps network|10 micros|
|Context switch|20-50 micros|
|Solid-state disk I/O|50-150 micros|
|Rotational disk I/O|1-10 ms|

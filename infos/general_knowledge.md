# Processor Architecture
###MESIF

*To remember: clean / dirty / request for ownership*

###Store forwarding
If a load follows a store and reloads the data that the store writes to memory, the data can forward directly from the store operation to the load. This process, called store to load forwarding, saves cycles by enabling the load to obtain the data directly from the store operation instead of through memory.


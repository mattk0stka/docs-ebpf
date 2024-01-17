# BPF_MAP_TYPE_HASH

`BPF_MAP_TYPE_HASH` provide general purpose hash map storage. Both the key and the value can be `struct`, allowing for composite keys and values.

The kernel is responsible for allocating and freeing key/value pairs, up to the `max_entries` limit that you specify. Hash maps use pre-allocation of hash tables elements by default. `BPF_F_NO_PREALLOC` flag can be used to disbale pre-allocation when it is to memory expensive.
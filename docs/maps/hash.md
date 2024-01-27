# BPF_MAP_TYPE_HASH

`BPF_MAP_TYPE_HASH` provide general purpose hash map storage. Both the key and the value can be `struct`, allowing for composite keys and values.

The kernel is responsible for allocating and freeing key/value pairs, up to the `max_entries` limit that you specify. Hash maps use pre-allocation of hash tables elements by default. `BPF_F_NO_PREALLOC` flag can be used to disbale pre-allocation when it is to memory expensive.

```c
--8<-- "docs/examples/maps/pkt_info_eth.c"
```


```bash
2024/01/23 17:11:17 Map contents:
        3e:22:fb:3a:69:65 => 7666
        00:50:56:ed:15:fa => 40428
2024/01/23 17:11:18 Map contents:
        3e:22:fb:3a:69:65 => 7682
        00:50:56:ed:15:fa => 40428
2024/01/23 17:11:19 Map contents:
        3e:22:fb:3a:69:65 => 7698
        00:50:56:ed:15:fa => 40430
2024/01/23 17:11:20 Map contents:
        3e:22:fb:3a:69:65 => 7714
        00:50:56:ed:15:fa => 42194
^C2024/01/23 17:11:21 Received signal, exiting..

```
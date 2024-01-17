# BPF_MAP_TYPE_ARRAY

`BPF_MAP_TYPE_ARRAY`provide generic array storage. The key type is an usinged 32-bit integer (4 bytes) and the map is of constant size. The size of the array is defined in `max_entries` at creation time. All array elements are pre-allocated and zero initalized when created. `BPF_MAP_TYPE_ARRAY` uses the same memory region for each CPU. The value stored can be of any size, however, all array elements are aligned to 8 bytes.

## Kernel BPF
This snippet shows howw to declase an array in a BPF program.
```c
struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __type(key, u32);
    __type(value, long);
    __uint(max_entries, 256);
} my_map SEC(".maps");
```

## Example Implementation

=== "C v1"

	```c title="pkt_counter.c" linenums="1"
	//
	// Created by mattk0stka on 17.01.24.
	//

	//go:build ignore

	#include <linux/bpf.h>
	#include <bpf/bpf_helpers.h>

	struct {
	    __uint(type, BPF_MAP_TYPE_ARRAY);
	    __type(key, __u32);
	    __type(value, __u64);
	    __uint(max_entries, 1);
	} pkt_count SEC(".maps");

	SEC("xdp")
	int count_packets() {

	    __u32 key = 0;
	    __u64 *count =bpf_map_lookup_elem(&pkt_count, &key);

	    if (count) {
	        __sync_fetch_and_add(count, 1);
	    }

	    return XDP_PASS;
	}

	char LICENSE[] SEC("license") = "Dual BSD/GPL";
	```

=== "C v2"

	```c title="pkt_counter.c" linenums="1"
	//
	// Created by mattk0stka on 17.01.24.
	//

	//go:build ignore

	#include <linux/bpf.h>
	#include <bpf/bpf_helpers.h>

	struct {
	    __uint(type, BPF_MAP_TYPE_ARRAY);
	    __type(key, __u32);
	    __type(value, __u64);
	    __uint(max_entries, 1);
	} pkt_count SEC(".maps");

	SEC("xdp")
	int count_packets_v2() {

	    __u32 key = 0;
	    __u64 init = 1, *count;

	    count = bpf_map_lookup_elem(&pkt_count, &key);

	    if (!count) {
		bpf_map_update_elem(&pkt_count, &key, &init, BPF_ANY);
		return 0;
	    }

	    __sync_fetch_and_add(count, 1);
  
	    return XDP_PASS;
	}

	char LICENSE[] SEC("license") = "Dual BSD/GPL";
	```

### bpf_map_lookup_elem()
> void \*bpf_map_lookup_elem(struct bpf_map \*map, const void \*key)

Array elements can be retrieved using the `bpf_map_lookup_elem()` helper. This helper returns a pointer into the array element, so to avoid data races with user-space reading the value, the user must use primitices like `__sync_fetch_and_add()` when updating the value in-place.

### bpf_map_update_elem()
> long bpf_map_update_elem(struct bpf_map \*map, const void \*key, const void \*value, u64 flags)

Array elements can be updated using the `bpf_map_update_elem()` helper. `bpf_map_update_elem()` returns 0 on success, or negative error in case of failure.

### Concurrency

something to add here.....

```go title="main.go" linenums="1"
package main

import (
	"bufio"
	"fmt"
	"github.com/cilium/ebpf/link"
	"github.com/cilium/ebpf/rlimit"
	"log"
	"net"
	"os"
	"os/signal"
	"time"
)

func main() {
	// Remove resource limits for kernels <5.11.
	if err := rlimit.RemoveMemlock(); err != nil { // (1)!
		log.Fatal("Removing memlock:", err)
	}

	// Load the compiled eBPF ELF and load it into the kernel.
	var objs pkt_counterObjects // (2)!
	if err := loadPkt_counterObjects(&objs, nil); err != nil {
		log.Fatal("Loading eBPF objects:", err)
	}
	defer objs.Close() // (3)!

	ifname := "ens33" // Change this to an interface on your machine.
	iface, err := net.InterfaceByName(ifname)
	if err != nil {
		log.Fatalf("Getting interface %s: %s", ifname, err)
	}

	// Attach count_packets to the network interface.
	link, err := link.AttachXDP(link.XDPOptions{ // (4)!
		Program:   objs.CountPackets,
		Interface: iface.Index,
	})
	if err != nil {
		log.Fatal("Attaching XDP:", err)
	}
	defer link.Close() // (5)!

	log.Printf("Counting incoming packets on %s..", ifname)

	// Periodically fetch the packet counter from PktCount,
	// exit the program when interrupted.
	tick := time.Tick(time.Second)
	stop := make(chan os.Signal, 5)
	signal.Notify(stop, os.Interrupt)
	for {
		select {
		case <-tick:
			var count uint64
			err := objs.PktCount.Lookup(uint32(0), &count) // (6)!
			if err != nil {
				log.Fatal("Map lookup:", err)
			}
			log.Printf("Received %d packets", count)
		case <-stop:
			log.Print("Received signal, exiting..")
			return
		}
	}
}
```
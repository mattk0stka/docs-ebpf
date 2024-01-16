# Page 2

## Another heading

```c title="trace_printk_example.c" linenums="1"
//
// Created by mattk0stka on 16.01.24.
//

//go:build ignore

#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>


SEC("kprobe/sys_execve")
int kprobe_execve(void *ctx) {

    int pid = bpf_get_current_pid_tgid() >> 32;// (1)
    const char fmt_str[] = "Hack the planet!, my pid is %d\n";
    bpf_trace_printk(fmt_str, sizeof(fmt_str), pid);
    return 0;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
```

1.  is a helper function that gets the ID of the process that triggered this eBPF program to run. It returns a 64-bit value with the process ID in the top 32 bits.<br/><br/> The lower 32 bits are the thread group ID. For a single-threaded process, this is the same as the process ID, but additional threads for the process would be given different IDs.

```go title="main.go" linenums="1"
package main

import (
	"bufio"
	"fmt"
	"github.com/cilium/ebpf/link"
	"github.com/cilium/ebpf/rlimit"
	"log"
	"os"
	"time"
)

func main() {
	// Remove resource limits for kernels <5.11.
	if err := rlimit.RemoveMemlock(); err != nil {
		log.Fatal("Removing memlock:", err)
	}

	// Load the compiled eBPF ELF and load it into the kernel.
	//var objs helloObjects
	objs := helloObjects{}
	if err := loadHelloObjects(&objs, nil); err != nil {
		log.Fatal("Loading eBPF objects:", err)
	}
	defer objs.Close()

	// name of the kernel function to trace
	fn := "sys_execve"
	kp, err := link.Kprobe(fn, objs.KprobeExecve, nil)
	if err != nil {
		log.Fatal("opening kprobe: %s", err)
	}

	defer kp.Close()

	filePath := "/sys/kernel/debug/tracing/trace_pipe"

	// Open the trace_pipe file for reading
	file, err := os.Open(filePath)
	if err != nil {
		fmt.Println("Error opening file:", err)
		return
	}
	defer file.Close()

	// some code here, to print out trace_pipe ... 
}
```

`bpf_trace_printk()` helper function in the kernel alway sends output to the same predefind location: */sys/kernel/debug/tracing/trace_pipe*. By using the `cat` command  you can confirm the content, for example:
```bash
 <...>-371439  [000] ...21 128224.956635: bpf_trace_printk: Hack the planet!, my pid is 371439
 <...>-371440  [005] ...21 128226.063314: bpf_trace_printk: Hack the planet!, my pid is 371440
 <...>-371440  [005] ...21 128226.065154: bpf_trace_printk: Hack the planet!, my pid is 371440
 bash-371441  [004] ...21 128226.067414: bpf_trace_printk: Hack the planet!, my pid is 371441
 <...>-371444  [007] ...21 128226.069998: bpf_trace_printk: Hack the planet!, my pid is 371444
 bash-371440  [005] ...21 128226.083741: bpf_trace_printk: Hack the planet!, my pid is 371440
 <...>-371450  [000] ...21 128226.103567: bpf_trace_printk: Hack the planet!, my pid is 371450
```
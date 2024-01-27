# BPF_MAP_TYPE_PERF_EVENT_ARRAY

These types of maps store `perf_events` data in a buffer ring that communicates between BPF programs and user-space programs in real time. They are defined with thy type `BPF_MAP_TYPE_PERF_EVENT_ARRAY`. They are designed to forward events that the kernels's tracing tools emit to user-space programs for further processing. This is the base for many **observability** tools. The user-space program acts as a listener that waits for events coming from the kernel, so you need to make sure that the code starts listening before the BPF program in the kernek is initialized.


```go title="gen.go" linenums="1"
//go:generate go run github.com/cilium/ebpf/cmd/bpf2go -target amd64 -type event bash uretprobe_bash.c -- -I../headers
```

## vmlinux.h
**vmlinux.h** is generated code. It contains all the type definitions that your running Linux kernel uses in its own source code. There's a tool, aptly names `bpftool`, that is maintained within the Linux repository. It has a feature to read the **vmlinux** object file and generate a **vmlinux.h**

```bash
$ bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h
```

```c title="uretprobe_bash.c" linenums="1"
--8<-- "docs/examples/maps/uretprobe_bash.c"
```


```bash
2024/01/27 21:33:15 Listening for events..
2024/01/27 21:33:26 /bin/bash:readline return value: exit
2024/01/27 21:33:32 /bin/bash:readline return value: ls -la
2024/01/27 21:33:37 /bin/bash:readline return value: cd ..
2024/01/27 21:33:45 /bin/bash:readline return value: ls -lh
```
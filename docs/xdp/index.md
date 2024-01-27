# XDP (eXpress Data Path)

**XDP** ist an eBPF-based high-performance data path used to send and receive network packets at high rates by bypassing most of the operationg system networking stack.

```c
struct xdp_md {
	__u32 data;
	__u32 data_end;
	__u32 data_meta;
	__u32 ingress_ifindex;
	__u32 rx_queue_index;
	__u32 egress_ifindex;
};
```
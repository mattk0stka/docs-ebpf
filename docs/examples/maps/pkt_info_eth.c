//
// Created by mattk0stka on 19.01.24.
//

//go:build ignore
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

#define MAX_MAP_ENTRIES 16

struct ethhdr {
    unsigned char h_dest[6];
    unsigned char h_source[6];
    __be16 h_proto;
};

struct counter {
    __u32 count;
    unsigned char h_source[6];
};

struct {
    __uint(type, BPF_MAP_TYPE_LRU_HASH);
    __uint(max_entries, MAX_MAP_ENTRIES);
    __type(key, __u32);
    __type(value, struct counter);
} xdp_stats_map SEC(".maps");


SEC("xdp")
int xdp_prog_func(struct xdp_md *ctx) {

    void *data = (void *)(long) ctx->data;
    void *data_end = (void *) (long) ctx->data_end;

    struct ethhdr *eth = data;
    if ((void *) (eth + 1) > data_end) {
        return 0;
    }

    struct counter *c = bpf_map_lookup_elem(&xdp_stats_map, eth->h_source);
    if (!c) {

        struct counter cu = {1};
        bpf_probe_read_kernel(&cu.h_source, sizeof(cu.h_source), eth->h_source);
        bpf_map_update_elem(&xdp_stats_map, eth->h_source, &cu, BPF_ANY);
    } else {

        __sync_fetch_and_add(&c->count, 1);
    }
    return XDP_PASS;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
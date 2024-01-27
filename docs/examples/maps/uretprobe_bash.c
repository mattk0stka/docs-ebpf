//
// Created by mattk0stka on 27.01.24.
//

//go:build ignore

#include "headers/vmlinux.h"

#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

struct event {
    __u32 pid;
    __u8 line[80];
};

struct {
    __uint(type, BPF_MAP_TYPE_PERF_EVENT_ARRAY);
} events SEC(".maps");

const struct event *unused __attribute__((unused));

SEC("uretprobe/bash_readline")
int uretprobe_bash_readline(struct pt_regs *ctx) {
    struct event event;

    event.pid = bpf_get_current_pid_tgid();
    //bpf_probe_read(&event.line, sizeof(event.line), (void *)PT_REGS_RC(ctx));
    bpf_probe_read_user(&event.line, sizeof(event.line), (void *)PT_REGS_RC(ctx));

    bpf_perf_event_output(ctx, &events, BPF_F_CURRENT_CPU, &event, sizeof(event));

    return 0;
}

char __license[] SEC("license") = "Dual MIT/GPL";
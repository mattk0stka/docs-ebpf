# Welcome to MkDocs

For full documentation visit [mkdocs.org](https://www.mkdocs.org).

## Commands

* `mkdocs new [dir-name]` - Create a new project.
* `mkdocs serve` - Start the live-reloading docs server.
* `mkdocs build` - Build the documentation site.
* `mkdocs -h` - Print help message and exit.

## Project layout

    mkdocs.yml    # The configuration file.
    docs/
        index.md  # The documentation homepage.
        ...       # Other markdown pages, images and other files.

## Codeblocks
some `code` here...
``` c title="some_basic_shit.c" linenums="1"
//
// Created by mk0stka on 14.01.24.
// (1)


#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

#define MAX_ENTRIES 10240
#define TASK_COMM_LEN 16

struct event {
    unsigned int pid;
    unsigned int tpid;
    int sig;
    int ret;
    char comm[TASK_COMM_LEN];
};

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __type(key, __u32);
    __type(value, struct event);
} values SEC(".maps");

int hello(void *ctx) {

    bpf_map_lookup_elem
    return 0;
}
```

1.  :man_raising_hand: I'm an annotation! I can contain `code`, __formatted
    text__, images, ... basically anything that can be expressed in Markdown.

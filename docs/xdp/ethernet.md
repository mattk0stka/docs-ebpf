# Ethernet


```c
#define ETH_P_IP 0x0800

// Ethernet Header Frame
struct ethhdr {
	unsigned char h_dest[6];
	unsigned char h_source[6];
	__be16 h_proto;
};
```



#ifndef FT_PING
#define FT_PING


#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


// netinet / [ip, ip_icmp, ...]
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include <stdlib.h>


#define SIZE_PACKET 64

struct icmp_packet {
	struct icmphdr icmphdr;
	char data[SIZE_PACKET - sizeof(struct icmphdr)];
};

unsigned short		checksum(void *data, int len);
struct			icmp_packet create_packet();
struct icmp_packet	create_packet();

// recv.c
unsigned char 	*recv_packet(int sockfd, struct sockaddr_in *ip_src);
int 		parse_packet(unsigned char *data);


#endif

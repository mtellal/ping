


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

#include <signal.h>

#include <sys/time.h>


#define SIZE_PACKET 64

struct timeval_s {
	struct timeval tv_send;
	struct timeval tv_recv;
};



struct stat_s {
	char		*host;
	unsigned short	p_sent;
	unsigned short	p_recv;
	suseconds_t	min; 
	suseconds_t	avg; 
	suseconds_t	max; 
	suseconds_t	stddev; 
};


// stat.c
struct stat_s	*get_stat();



struct icmp_packet {
	struct icmphdr icmphdr;
	char data[SIZE_PACKET - sizeof(struct icmphdr)];
};

unsigned short		checksum(void *data, int len);
struct			icmp_packet create_packet();
struct icmp_packet	create_packet();

// recv.c
unsigned char 	*recv_packet(int sockfd, struct sockaddr_in *ip_src, struct timeval_s *tv);
int 		parse_packet(unsigned char *data, struct timeval_s *tv);

int 		send_packet(int sockfd, struct sockaddr_in ip_dst, struct icmp_packet packet, struct timeval_s *tv);


#endif

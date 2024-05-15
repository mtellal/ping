#ifndef FT_PING
#define FT_PING

#include <errno.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netdb.h>

#include <ifaddrs.h>

#include <arpa/inet.h>

#include <string.h>

#include <unistd.h>

#include <netinet/ip.h>

#include <netinet/ip_icmp.h>

#include <stdlib.h>

#include <stdio.h>

#include <signal.h>

#include <sys/time.h>

#include <math.h>

#include <sysexits.h>


#define OPT_VERBOSE	1

#define OPT_TTL		2

#define SIZE_PACKET 	64

#define DEFAULT_TTL		64

#define DATA_BYTES		SIZE_PACKET - sizeof(struct icmphdr)

struct stat_s {
	char *					host;
	uint16_t				p_sent;
	uint16_t				p_recv;
	suseconds_t				min; 
	suseconds_t				tot; 
	suseconds_t				max; 
	suseconds_t				stddev; 
	suseconds_t				rtts[0xffff];
	unsigned short			options;
	uint8_t					ttl;
	uint8_t					err;
	uint16_t				pid;
	int						sockfd;
	struct sockaddr_in 		ip_dst;
};

struct icmp_packet {
	struct icmphdr 		icmphdr;
	char 				data[SIZE_PACKET - sizeof(struct icmphdr)];
};

#define SIZE_IP_PACKET	sizeof(struct final_packet_s)



unsigned short			checksum(void *data, int len);
struct icmp_packet 		create_packet();
struct icmp_packet		create_packet();

// parse.c
char *					parse_args(int argc, char **argv);


// recv.c
int						recv_packet(int sockfd);

// send.c
unsigned short 			checksum(void *b, int len);
struct icmp_packet		create_packet(struct stat_s * stat);
int						send_packet(struct stat_s * stat);

// stat.c
struct stat_s *			get_stat();
void 					signalhandler(int s);


// utils.c
void 					display_datas(unsigned char *datas, int len);
void 					print_iphdr(struct iphdr *iphdr);
void 					print_icmphdr(struct icmphdr *icmphdr);
void					exit_miss_host();
void					exit_failure(char *msg);



#endif

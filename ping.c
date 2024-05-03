#include <errno.h>
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


#define SIZE_PACKET 64

struct icmp_packet {
	struct icmphdr icmphdr;
	char data[SIZE_PACKET - sizeof(struct icmphdr)];
};


int calcul_checksum(void *data, int len) {

	unsigned int sum = 0;
	unsigned short *b = data;

	for (; len > 1; len -= 2) {
		sum += *b++;
	}
	if (len == 1) {
		sum += *(unsigned char *)b;
	}

	sum = (sum >> 16) & (sum & 0x00FF);
	sum += (sum >>16);
	sum = ~sum;
	return (sum);
	
}


int main(int argc, char **argv) {


	(void)argc;
	(void)argv;

	if (argc != 2) {
		printf("ping: usage error: Destination address required\n");
		return 1;
	}


		
	struct sockaddr_in ip_dst;


	memset(&ip_dst, 0, sizeof(ip_dst));
	ip_dst.sin_family = AF_INET;
	if (inet_pton(AF_INET, "127.0.0.5", &(ip_dst.sin_addr)) != 1)
	{
		printf("inet_pton  call failed: %s \n", strerror(errno));
		return 0;
	}

	printf("Address successfully created and initialized\n");

	/////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	printf("Creating socket \n");


	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (sockfd == -1)
	{
		printf("socker call failed: %s  \n", strerror(errno));
		return 0;
	}

	printf("socket successfuly created\n");

	
	///////////////////////////////////////////////////////////////////////
	//

	int msg_count = 0;

	while (1) {
	printf("creating the packet \n");
	//  create icmp packet 
	//
	struct icmp_packet packet;


	memset(&packet, 0, sizeof(packet));

	packet.icmphdr.type = ICMP_ECHO; // marco in netinet/ip_icmp.h 
	packet.icmphdr.code = 0; // code from rfc
	
	packet.icmphdr.un.echo.id = getpid(); 
	packet.icmphdr.un.echo.sequence = msg_count++;

	memset(&packet.data, 0, sizeof(packet.data));

	packet.icmphdr.checksum = htons(calcul_checksum(&packet, sizeof(packet)));
	printf("checksum calculated: %i\n", packet.icmphdr.checksum);


	printf("sending bytes over network ...");
	int bytes = sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&ip_dst, sizeof(ip_dst));
	if (bytes == -1) {
		printf("sendto call failed: %s \n", strerror(errno));
		return 0;	
	}

	printf("sent %i bytes  \n", bytes);
		sleep(1);
	
	////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////
	
	struct sockaddr_in ip_src;

	memset(&ip_src, 0, sizeof(ip_src));
	ip_src.sin_family = AF_INET;
	if (inet_pton(AF_INET, "127.0.0.1", &(ip_src.sin_addr)) != 1)
	{
		printf("inet_pton (src) call failed: %s \n", strerror(errno));
		return 1;
	}

	int len = sizeof(ip_src);

	char recv[4096];

	bytes = recvfrom(sockfd, recv, 4096, 0, (struct sockaddr*)&ip_src, (socklen_t*)&len);

	if (bytes == -1) {
		printf("bytes = -1\n");
	}

	printf("recv %i bytes, %s\n", bytes, recv);
	
	unsigned short *b = (unsigned short *)recv;

	for (int l = 0; l < bytes; l += 2) {
		printf("0x%x  ", *b);
		b++;
	}
	
	}
}

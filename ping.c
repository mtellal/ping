#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

// netinet / [ip, ip_icmp, ...]
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>


struct icmp_packet {
	struct icmphdr icmphdr;
	char data[1];
};


int calcul_checksum(void *data) {

	int sum = 0;
	unsigned short *b = data;
	(void)b;
	(void)sum;

	printf("sizeof data %i\n", (int)sizeof(data)); 
	for (int len = sizeof(data); len > 1; len -= 2) {
		printf("16 bits word\n");
	}

	return (0);
	
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
	if (inet_pton(AF_INET, "127.0.0.1", &(ip_dst.sin_addr)) != 1)
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


	printf("creating the packet \n");
	//  create icmp packet 
	//
	struct icmp_packet packet;


	memset(&packet, 0, sizeof(packet));

	packet.icmphdr.type = ICMP_ECHO; // marco in netinet/ip_icmp.h 
	packet.icmphdr.code = 0; // code from rfc
	
	//packet.icmphdr.un.echo.id =  
	//packet.icmphdr.un.echo.sequence = 
	
	packet.icmphdr.checksum = 0; 
	calcul_checksum(&packet);



	printf("trying to send bytes over network");
	int bytes = sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&ip_dst, sizeof(ip_dst));
	if (bytes == -1) {
		printf("sendto call failed: %s \n", strerror(errno));
		return 0;	
	}

	printf("sent %i bytes  \n", bytes);
}

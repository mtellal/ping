#include <ft_ping.h>



// init socket
//
// send
// recv 


int send_packet(int sockfd, struct sockaddr_in ip_dst, struct icmp_packet packet) {

	printf("sending packet ...\n");
	int bytes = sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&ip_dst, sizeof(ip_dst));
	if (bytes == -1) {
		printf("sendto call failed: %s \n", strerror(errno));
		return 1;	
	}
	printf("sent %i bytes\n", bytes);
	return 0;
}

int recv_packet(int sockfd, char *ip_s) {

	struct sockaddr_in ip_src;
	int len;
	int bytes;
	unsigned short size = 64;
	char recv[size];

	memset(&ip_src, 0, sizeof(ip_src));
	ip_src.sin_family = AF_INET;
	if (inet_pton(AF_INET, ip_s, &(ip_src.sin_addr)) != 1)
	{
		printf("inet_pton (src) call failed: %s \n", strerror(errno));
		return 1;
	}

	len = sizeof(ip_src);

	bytes = recvfrom(sockfd, recv, size, 0, (struct sockaddr*)&ip_src, (socklen_t*)&len);

	if (bytes == -1) {
		printf("bytes = -1\n");
	}
	printf("received %i bytes\n", bytes);

	return 0;

}

struct sockaddr_in *resolve_addr(char * addr){

	struct addrinfo hints;
	struct addrinfo *res;


	memset(&hints, 0, sizeof(hints));
	memset(&res, 0, sizeof(res));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int error_code = getaddrinfo(addr, 0, &hints, &res);
	if (error_code != 0)
	{
		//printf("getaddrinfo call failed: %s\n", gai_strerror(error_code));
		return NULL;
	}	
	if (res && res->ai_addr)
		return (struct sockaddr_in *)res->ai_addr;
	return NULL;
}

int main(int argc, char **argv) {


	(void)argc;
	(void)argv;

	if (argc != 2) {
		printf("ping: usage error: Destination address required\n");
		return 1;
	}


	
	struct sockaddr_in *ip_dst;

	/*
	memset(&ip_dst, 0, sizeof(ip_dst));
	ip_dst.sin_family = AF_INET;
	if (inet_pton(AF_INET, "127.0.0.1", &(ip_dst.sin_addr)) != 1)
	{
		printf("inet_pton  call failed: %s \n", strerror(errno));
		return 0;
	}

	printf("Address successfully created and initialized\n");
	*/

	ip_dst = resolve_addr(argv[1]);

	if (!ip_dst) {
		printf("ping: unknown host\n"); 
		return 1;
	}

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

	
	while (1) {
	
		printf("creating the packet \n");
	
		struct icmp_packet packet;

		packet = create_packet();

		if (send_packet(sockfd, *ip_dst, packet) == -1)
			return 1;

		recv_packet(sockfd, "127.0.0.5");

		/*
		unsigned char *b = (unsigned char*)recv;
		for (int i = 0; i < 64; i++) {
			printf("%x ", b[i]);
		}
		*/
		sleep(1);
		printf("\n");
	}
}

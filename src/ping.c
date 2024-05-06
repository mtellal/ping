#include <ft_ping.h>


int send_packet(int sockfd, struct sockaddr_in ip_dst, struct icmp_packet packet) {

	int bytes = sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&ip_dst, sizeof(ip_dst));
	if (bytes == -1) {
		printf("sendto call failed: %s \n", strerror(errno));
		return 1;	
	}
	return 0;
}

int recv_packet(int sockfd, char *ip_s, struct sockaddr_in *ip_src) {

	int len;
	int bytes;
	unsigned short size = 64;
	char recv[size];

	len = sizeof(ip_src);
	bytes = recvfrom(sockfd, recv, size, 0, (struct sockaddr*)ip_src, (socklen_t*)&len);
	if (bytes == -1) {
		printf("bytes = -1\n");
	}
	// 64 bytes from 216.58.213.78: icmp_seq=0 ttl=63 time=2.461 ms
	printf("%u bytes from %s: icmp_seq=", bytes, ip_s);
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
		//printf("ping: usage error: Destination address required\n");
		return 1;
	}
	
	struct sockaddr_in *ip_dst;
	ip_dst = resolve_addr(argv[1]);

	if (!ip_dst) {
		printf("ping: unknown host\n"); 
		return 1;
	}

	char host[NI_MAXHOST];

	int err = getnameinfo((struct sockaddr *)ip_dst, sizeof(*ip_dst), host, NI_MAXHOST, 0 ,0, NI_NUMERICHOST);
	if (err != 0) {
		//printf("getnameinfo call failed %s \n", gai_strerror(err));
		return 1;
	}

	printf("PING %s (%s): 56 data bytes\n", argv[1], host);
	// 64 bytes from 216.58.213.78: icmp_seq=0 ttl=63 time=2.461 ms

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd == -1)
	{
		printf("socker call failed: %s  \n", strerror(errno));
		return 0;
	}
	
	while (1) {	
		struct icmp_packet packet;
		packet = create_packet();
		if (send_packet(sockfd, *ip_dst, packet) == -1)
			return 1;
		recv_packet(sockfd, host, ip_dst);
		sleep(1);
		printf("\n");
	}
}

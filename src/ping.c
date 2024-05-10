#include <ft_ping.h>

int send_packet(int sockfd, struct sockaddr_in ip_dst, struct icmp_packet packet) {

	int bytes = sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&ip_dst, sizeof(ip_dst));
	if (bytes == -1) {
		printf("sendto call failed: %s \n", strerror(errno));
		return 1;	
	}
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


// ttl
// icmp (sequence)
// timestamp
// 

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	struct sockaddr_in *	ip_dst;
	char			host[NI_MAXHOST];
	unsigned short		data_bytes;
	int			sockfd;	
	unsigned char *		data;
	struct icmp_packet	icmp_packet;


	data_bytes = SIZE_PACKET - sizeof(struct icmphdr);

	if (argc != 2) {
		//printf("ping: usage error: Destination address required\n");
		return 1;
	}
	
	ip_dst = resolve_addr(argv[1]);

	if (!ip_dst) {
		printf("ping: unknown host\n"); 
		return 1;
	}

	int err = getnameinfo((struct sockaddr *)ip_dst, sizeof(*ip_dst), host, NI_MAXHOST, 0 ,0, NI_NUMERICHOST);
	if (err != 0) {
		//printf("getnameinfo call failed %s \n", gai_strerror(err));
		return 1;
	}

	printf("PING %s (%s): %d data bytes\n", argv[1], host, data_bytes);

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd == -1)
	{
		printf("socker call failed: %s  \n", strerror(errno));
		return 0;
	}

	while (1) {	
		icmp_packet = create_packet();
		if (send_packet(sockfd, *ip_dst, icmp_packet) == -1)
			return 1;
		data = recv_packet(sockfd, ip_dst);
		parse_packet(data);
		sleep(1);
		printf("\n");
	}
}

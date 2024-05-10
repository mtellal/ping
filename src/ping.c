#include <ft_ping.h>

int send_packet(int sockfd, struct sockaddr_in ip_dst, struct icmp_packet packet, struct timeval_s *tv) {

	int		bytes;
	struct stat_s	*stat;

	stat = get_stat();
	gettimeofday(&tv->tv_send, NULL);
	bytes = sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&ip_dst, sizeof(ip_dst));
	if (bytes == -1) {
		printf("sendto call failed: %s \n", strerror(errno));
		return 1;	
	}
	stat->p_sent++;
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


void signalhandler(int s) {
	
	struct stat_s	*stat;
	unsigned short	loss;

	(void)s;	
	stat = get_stat();
	loss = ((1 - ((float)stat->p_recv / (float)stat->p_sent)) * 100);
	printf("--- %s ping statistics ---\n", stat->host);
	printf("%i packets transmitted, %i packets received, %u%% packet loss\n", stat->p_sent, stat->p_recv, loss);
	printf("round-trip min/avg/max/stddev = %s/%s/%s/%s ms\n", "", "", "", "");
	exit(EXIT_SUCCESS);
}


int main(int argc, char **argv) {

	struct sockaddr_in *	ip_dst;
	char			host[INET_ADDRSTRLEN];
	unsigned short		data_bytes;
	int			sockfd;	
	unsigned char *		data;
	struct icmp_packet	icmp_packet;
	struct stat_s		*stat;

	unsigned short		w_time;


	signal(SIGINT, signalhandler);
	stat = get_stat();
	w_time = 1;
	data_bytes = SIZE_PACKET - sizeof(struct icmphdr);

	if (argc != 2) {
		printf("ping: usage error: Destination address required\n");
		return 1;
	}

	stat->host = argv[1];	
	if ((ip_dst = resolve_addr(argv[1])) == NULL) {
		printf("ping: unknown host\n"); 
		return 1;
	}

	if (inet_ntop(AF_INET, &ip_dst->sin_addr, host, INET_ADDRSTRLEN) == NULL) {
		printf("inet_ntop call returned NULL (bad address)\n");
		return 1;
	}

	stat->host = argv[1];
	printf("PING %s (%s): %d data bytes\n", argv[1], host, data_bytes);

	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
		printf("socker call failed: %s  \n", strerror(errno));
		return 0;
	}

	struct timeval_s tv;

	while (1) {	
		icmp_packet = create_packet();
		if (send_packet(sockfd, *ip_dst, icmp_packet, &tv) == -1)
			return 1;
		data = recv_packet(sockfd, ip_dst, &tv);
		parse_packet(data, &tv);
		sleep(w_time);
	}
}

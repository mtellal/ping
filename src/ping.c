#include <ft_ping.h>

void  resolve_addr(struct stat_s * stat){

	struct addrinfo 	hints;
	struct addrinfo 	*res;

	memset(&res, 0, sizeof(res));
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(stat->host, 0, &hints, &res) != 0)
		exit_failure("ping: unknown host\n");
	if (res && res->ai_addr) {
		stat->ip_dst = *(struct sockaddr_in *)res->ai_addr;
		freeaddrinfo(res);
	}
}


void	print_ping(struct stat_s *stat) {

	char	host[INET_ADDRSTRLEN];

	if (inet_ntop(AF_INET, &stat->ip_dst.sin_addr, host, INET_ADDRSTRLEN) == NULL) {
		exit_failure("ping: inet_ntop call returned NULL (bad address)\n");
	}
	
	if (stat->options & OPT_VERBOSE) 
		printf("PING %s (%s): %ld data bytes, id 0x%x = %u\n", stat->host, host, DATA_BYTES, stat->pid, stat->pid);
	else
		printf("PING %s (%s): %ld data bytes\n", stat->host, host, DATA_BYTES);
}

int	init_socket(struct stat_s *stat) {
	
	int				sockfd;
	int				i;

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd  == -1) {
		fprintf(stderr, "socket call failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, (const char *)&stat->opt_ttl, sizeof(stat->opt_ttl)) == -1){
		fprintf(stderr, "ping: setsockopt call failed: (iphdr) %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	i = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char *)&i, sizeof(i)) == -1){
		fprintf(stderr, "ping: setsockopt call failed: (broadcast) %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return sockfd;
}


int main(int argc, char **argv) {

	struct stat_s *		stat;

	if (argc < 2)
		exit_miss_host();


	stat = get_stat();
	memset(stat, 0, sizeof(struct stat_s));

	stat->host = parse_args(argc, argv);
	if (stat->host == NULL)
		exit_miss_host();	
	
	
	stat->pid = getpid();
	stat->sockfd = init_socket(stat);
	resolve_addr(stat);

	print_ping(stat);

	signal(SIGINT, signalhandler);
	signal(SIGALRM, signalhandler);

	signalhandler(SIGALRM);

	while (1) {
		if (recv_packet(stat->sockfd) == -1)
			return(EXIT_FAILURE);
	}
}

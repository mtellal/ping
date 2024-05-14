#include <ft_ping.h>

struct sockaddr_in *resolve_addr(char * addr){

	struct addrinfo hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof(hints));
	memset(&res, 0, sizeof(res));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(addr, 0, &hints, &res) != 0)
		exit_failure("ping: unknown host\n");
	if (res && res->ai_addr)
		return (struct sockaddr_in *)res->ai_addr;
	return NULL;
}


void	ping_main_info(struct sockaddr_in *ip_dst, struct stat_s *stat) {

	pid_t	pid;
	char	host[INET_ADDRSTRLEN];

	if (inet_ntop(AF_INET, &ip_dst->sin_addr, host, INET_ADDRSTRLEN) == NULL) 
		exit_failure("ping: inet_ntop call returned NULL (bad address)\n");
	
	if (stat->options & OPT_VERBOSE) {
		pid = getpid();
		printf("PING %s (%s): %ld data bytes, id 0x%x = %u\n", stat->host, host, DATA_BYTES, pid, pid);
	}
	else
		printf("PING %s (%s): %ld data bytes\n", stat->host, host, DATA_BYTES);
}

int	init_socket() {
	
	int				sockfd;
	struct stat_s	*stat;
	int				i;

	stat = get_stat();
	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
		printf("socket call failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (!stat->ttl)
		stat->ttl = DEFAULT_TTL;
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, (const char *)&stat->ttl, sizeof(stat->ttl)) == -1){
		printf("ping: setsockopt call failed: (iphdr) %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	i = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char *)&i, sizeof(i)) == -1){
		printf("ping: setsockopt call failed: (broadcast) %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return sockfd;
}

int main(int argc, char **argv) {

	struct sockaddr_in *	ip_dst;
	int			sockfd;	
	struct stat_s *		stat;

	unsigned short		w_time;
	
	char *			arg_host;

	signal(SIGINT, signalhandler);
	stat = get_stat();
	w_time = 1;

	if (argc < 2)
		exit_miss_host();

	if ((arg_host = parse_args(argc, argv)) == NULL)
		exit_miss_host();	

	stat->host = arg_host;	
	
	ip_dst = resolve_addr(arg_host);

	ping_main_info(ip_dst, stat);

	sockfd = init_socket();

	while (1) {	
		if (send_packet(sockfd, ip_dst) == -1)
			return(EXIT_FAILURE);
		if (recv_packet(sockfd, ip_dst) == -1)
			return(EXIT_FAILURE);
		sleep(w_time);
	}
}

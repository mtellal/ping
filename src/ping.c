#include <ft_ping.h>


struct sockaddr_in *resolve_addr(char * addr){

	struct addrinfo hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof(hints));
	memset(&res, 0, sizeof(res));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int error_code = getaddrinfo(addr, 0, &hints, &res);
	if (error_code != 0)
		return NULL;
	if (res && res->ai_addr)
		return (struct sockaddr_in *)res->ai_addr;
	return NULL;
}

double	calcul_stddev(struct stat_s *stat) {
	
	double		mean;
	unsigned int	i;
	
	double		mu;
	double		tmp;


	mean = stat->tot / stat->p_recv;
	i = 0;
	while (i < stat->p_recv) {
		tmp = (stat->rtts[i] - mean);
		tmp *= tmp;
		mu += tmp;
		i++;	
	}
	return sqrt(mu / stat->p_recv);
}


void signalhandler(int s) {
	
	struct stat_s	*stat;
	unsigned short	loss;
	suseconds_t	stddev;
	suseconds_t	avg;

	(void)s;	
	stat = get_stat();
	avg = stat->tot / stat->p_recv;
	loss = ((1 - ((float)stat->p_recv / (float)stat->p_sent)) * 100);
	stddev = (suseconds_t)calcul_stddev(stat);
	
	printf("--- %s ping statistics ---\n", stat->host);
	printf("%i packets transmitted, %i packets received, %u%% packet loss\n", stat->p_sent, stat->p_recv, loss);
	printf("round-trip min/avg/max/stddev = %ld,%03ld/%ld,%03ld/%ld,%03ld/%ld,%03ld ms\n", stat->min / 1000, stat->min % 1000, avg / 1000, avg % 1000, stat->max / 1000, stat->max % 1000, stddev / 1000, stddev % 1000);
	exit(EXIT_SUCCESS);
}


void	print_help() {

	printf("Usage: ping [OPTION...] HOST ...\nSend ICMP ECHO_REQUEST packets to network hosts.\n");
	printf("\n");
	printf("Options:\n");
	printf("\t-?,  give this help list\n");
	printf("\t-v,  verbose output\n");
	printf("\t--ttl=N,  specify N as time-to-live\n");
	exit(EXIT_SUCCESS);
}


int	valid_options(char *argv) {
	char		*options[] = { "-v", "-?", "--ttl=" };
	uint8_t		i;
	
	i = 0;
	while (i < 3) {
		if (!memcmp(options[i], argv, strlen(options[i])))
			return 1;
		i++;
	}
	return 0;
}

void	exit_miss_host() {
	printf("ping: missing host operand\nTry 'ping -?' for more information\n");
	exit(EX_USAGE); //64 - sysexits.h
}

char	*parse_args(int argc, char **argv) {

	struct stat_s	*stat;
	uint16_t	i;
	
	i = 1;
	stat = get_stat();
	stat->options = 0;
	while (i < argc) {
		if (valid_options(argv[i]) && argc - 1 == i) 
			exit_miss_host();
		else if (!memcmp(argv[i], "-?", 2))
			print_help();
		else if (!memcmp(argv[i], "-v", 2))
			stat->options += OPT_VERBOSE;
		else if (!memcmp(argv[i], "--ttl=", sizeof("--ttl=")))
			stat->options += OPT_TTL;
		else if (argv[i][0] == '-') {
			printf("%s: invalid option -- '%s'\nTry 'ping -?' for more information.\n", argv[0], argv[i]);
			exit(64);
		}
		else
			return argv[i];
		i++;
	}
	return NULL;
}

int main(int argc, char **argv) {

	struct sockaddr_in *	ip_dst;
	char			host[INET_ADDRSTRLEN];
	unsigned short		data_bytes;
	int			sockfd;	
	struct stat_s *		stat;

	unsigned short		w_time;
	
	char *			arg_host;
	pid_t			pid;

	signal(SIGINT, signalhandler);
	stat = get_stat();
	w_time = 1;
	data_bytes = SIZE_PACKET - sizeof(struct icmphdr);

	if (argc < 2)
		exit_miss_host();

	arg_host = parse_args(argc, argv);
	if (!arg_host)
		exit_miss_host();	

	stat->host = arg_host;	
	if ((ip_dst = resolve_addr(stat->host)) == NULL) {
		printf("ping: unknown host\n"); 
		return(EXIT_FAILURE);
	}

	if (inet_ntop(AF_INET, &ip_dst->sin_addr, host, INET_ADDRSTRLEN) == NULL) {
		printf("inet_ntop call returned NULL (bad address)\n");
		return 1;
	}

	if (stat->options & OPT_VERBOSE) {
		pid = getpid();
		printf("PING %s (%s): %d data bytes, id 0x%x = %u\n", stat->host, host, data_bytes, pid, pid);
	}
	else
		printf("PING %s (%s): %d data bytes\n", stat->host, host, data_bytes);

	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
		printf("socker call failed: %s  \n", strerror(errno));
		return 0;
	}

	while (1) {	
		if (send_packet(sockfd, *ip_dst) == -1)
			return 1;
		if (recv_packet(sockfd, ip_dst) == -1)
			return 1;
		sleep(w_time);
	}
}

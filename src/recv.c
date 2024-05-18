
#include <ft_ping.h>

void 	print_infos_error(struct icmphdr *icmphdr, int verbose) {
	
	char * 			TYPES[] = {
		[ICMP_DEST_UNREACH] = "Destination Unreachable",
		[ICMP_SOURCE_QUENCH] = "Source Quench",
		[ICMP_REDIRECT] = "Redirect (change route)",
		[ICMP_TIME_EXCEEDED] = "Time To Live Exceeded",
		[ICMP_PARAMETERPROB] = "Parameter Problem",
	};	

	if (verbose)
		printf("type=%u code=%u", icmphdr->type, icmphdr->code);
	else 
		printf("%s", TYPES[icmphdr->type]);
	printf("\n");
}

void	print_time(struct timeval *tv_send, struct timeval *tv_recv) {

	struct timeval	diff;
	struct stat_s	*stat;

	stat = get_stat();

	diff.tv_usec = tv_recv->tv_usec - tv_send->tv_usec;

	if (!stat->min || stat->min > diff.tv_usec)
		stat->min = diff.tv_usec;
	if (!stat->max || stat->max < diff.tv_usec)
		stat->max = diff.tv_usec;	
	stat->tot += diff.tv_usec;
	stat->rtts[stat->p_recv - 1] = diff.tv_usec;
	printf(" time=%ld,%03ld ms\n", diff.tv_usec / 1000, diff.tv_usec % 1000);
}

int	print_packet_infos(struct iphdr *iphdr, struct icmphdr *icmphdr, int error) {

	char				src[INET_ADDRSTRLEN];
	struct stat_s *		stat;

	stat = get_stat();

	memset(src, 0, INET_ADDRSTRLEN);
	if (inet_ntop(AF_INET, &iphdr->saddr, src, INET_ADDRSTRLEN) == NULL) {
		printf("Error: (recv packet) inet_ntop call failed (bad address) %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}	

	printf("%u bytes from %s: ", ntohs(iphdr->tot_len) - (iphdr->ihl * 4), src);
	if (error) {
		print_infos_error(icmphdr, stat->options & OPT_VERBOSE);
	}
	else {
		printf("icmp_seq=%u ttl=%u", htons(icmphdr->un.echo.sequence), iphdr->ttl);
	}
	return 0;
}

int	parse_packet(unsigned char *datas, struct timeval *tv_recv) {

	struct iphdr *		iphdr;
	struct icmphdr *	icmphdr;
	struct stat_s *		stat;
	struct icmphdr *	rpid;
	struct timeval *	tv_send;
	uint16_t			len_iphdr;

	stat = get_stat();
	iphdr = (struct iphdr *)datas;
	len_iphdr = (uint16_t)(iphdr->ihl * 4);
	icmphdr = (struct icmphdr *)(datas + len_iphdr);		
	
	if (icmphdr->type != ICMP_ECHOREPLY && 
		(icmphdr->type == ICMP_DEST_UNREACH || 
		icmphdr->type == ICMP_TIME_EXCEEDED ||
		icmphdr->type == ICMP_PARAMETERPROB ||
		icmphdr->type == ICMP_SOURCE_QUENCH ||
		icmphdr->type == ICMP_REDIRECT)) {

		rpid = (struct icmphdr *)(datas + ((iphdr->ihl * 4) * 2) + sizeof(struct icmphdr)); 

		if (ntohs(rpid->un.echo.id) == stat->pid) {
			stat->err = 1;
			print_packet_infos(iphdr, icmphdr, 1);
		}
	}
	else if (ntohs(icmphdr->un.echo.id) == stat->pid) {
		stat->p_recv++;
		tv_send = (struct timeval *)(datas + (iphdr->ihl * 4) + 8);
		print_packet_infos(iphdr, icmphdr, 0);
		print_time(tv_send, tv_recv);
		if (stat->options & OPT_NUMBER && stat->opt_number == stat->p_recv)
			signalhandler(SIGINT);
	}
	return 1;
}

int	recv_packet(int sockfd) {

	int 				bytes;
	unsigned char 		datas[IP_MAXPACKET];
	struct timeval		tv_recv;

	memset(&datas, 0, sizeof(datas));	
	bytes = recvfrom(sockfd, datas, IP_MAXPACKET, MSG_DONTWAIT, NULL, 0);
	gettimeofday(&tv_recv, NULL);
	if (bytes == -1) 
		return 0;
	parse_packet(datas, &tv_recv);
	return 0;
}

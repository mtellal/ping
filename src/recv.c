
#include <ft_ping.h>

int	recv_packet(int sockfd, struct sockaddr_in *ip_src) {

	int 			len;
	int 			bytes;
	unsigned short 		size = 64;
	unsigned char 		datas[SIZE_PACKET + sizeof(struct iphdr)];
	struct timeval		tv_recv;
	struct sockaddr_in	ip;

	memcpy(&ip, ip_src, sizeof(struct sockaddr_in));
	len = sizeof(ip);
	bytes = recvfrom(sockfd, datas, size, MSG_DONTWAIT, (struct sockaddr*)&ip, (socklen_t*)&len);
	gettimeofday(&tv_recv, NULL);
	if (bytes == -1) 
		return 0;
	parse_packet(datas, &tv_recv);
	return 0;
}

int	print_packet_infos(struct iphdr *iphdr, uint16_t len_iphdr, struct icmphdr *icmphdr, int error) {

	unsigned short	bytes;
	char			src[INET_ADDRSTRLEN];
	struct stat_s *	stat;
	char * 			TYPES[] = {
		[ICMP_ECHOREPLY] = "Echo Reply",
		[ICMP_DEST_UNREACH] = "Destination Unreachable",
		[ICMP_SOURCE_QUENCH] = "Source Quench",
		[ICMP_REDIRECT] = "Redirect (change route)",
		[ICMP_ECHO] = "Echo Request",
		[ICMP_TIME_EXCEEDED] = "Time Exceeded",
		[ICMP_PARAMETERPROB] = "Parameter Problem",
		[ICMP_TIMESTAMP] = "Timestamp Request",
		[ICMP_TIMESTAMPREPLY] = "Timestamp Reply",
		[ICMP_INFO_REQUEST] = "Information Request",
		[ICMP_INFO_REPLY] = "Information Reply",
		[ICMP_ADDRESS] = "Address Mask Request",
		[ICMP_ADDRESSREPLY] = "Address Mask Reply",
	};	

	stat = get_stat();
	bytes = ntohs(iphdr->tot_len) - len_iphdr;	

	memset(src, 0, INET_ADDRSTRLEN);
	if (inet_ntop(AF_INET, &iphdr->saddr, src, INET_ADDRSTRLEN) == NULL) {
		printf("Error: (recv packet) inet_ntop call failed (bad address) %s\n", strerror(errno));
		return (-1);
	}	

	

	printf("%u bytes from %s: ", bytes, src);
	if (error) {
		if (stat->options & OPT_VERBOSE)
			printf("type=%u code=%u", icmphdr->type, icmphdr->code);
		else 
			printf("%s", TYPES[icmphdr->type]);
	}
	else
		printf("icmp_seq=%u ttl=%u", icmphdr->un.echo.sequence, iphdr->ttl);
	return 0;
}


void	print_time(struct timeval *tv_send, struct timeval *tv_recv, int error) {

	struct timeval	diff;
	struct stat_s	*stat;


	stat = get_stat();
	if (error) {
		printf("\n");
		stat->err = error;
	}
	else {
		stat->p_recv++;

		//diff.tv_sec = tv->tv_recv.tv_sec - tv->tv_send.tv_sec;
		diff.tv_usec = tv_recv->tv_usec - tv_send->tv_usec;

		if (!stat->min || stat->min > diff.tv_usec)
			stat->min = diff.tv_usec;
		if (!stat->max || stat->max < diff.tv_usec)
			stat->max = diff.tv_usec;	
		stat->tot += diff.tv_usec;
		stat->rtts[stat->p_recv - 1] = diff.tv_usec;
			printf(" time=%ld,%03ld ms\n", diff.tv_usec / 1000, diff.tv_usec % 1000);
	}
}

int	parse_packet(unsigned char *datas, struct timeval *tv_recv) {

	struct iphdr	*iphdr;
	uint16_t		len_iphdr;
	struct icmphdr	*icmphdr;
	struct timeval	*tv_send;

	iphdr = (struct iphdr *)datas;
	len_iphdr = iphdr->ihl * 4;	
	icmphdr = (struct icmphdr *)(datas + len_iphdr);		
	tv_send = (struct timeval *)(datas + len_iphdr + 8);

	print_packet_infos(iphdr, len_iphdr, icmphdr, icmphdr->type != ICMP_ECHOREPLY);
	print_time(tv_send, tv_recv, icmphdr->type != ICMP_ECHOREPLY);
	return 1;
}


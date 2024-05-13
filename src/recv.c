
#include <ft_ping.h>

int	recv_packet(int sockfd, struct sockaddr_in *ip_src) {

	int 			len;
	int 			bytes;
	unsigned short 		size = 64;
	unsigned char 		*datas = NULL;
	struct timeval		tv_recv;

	datas = malloc(size);
	len = sizeof(ip_src);
	bytes = recvfrom(sockfd, datas, size, 0, (struct sockaddr*)ip_src, (socklen_t*)&len);
	gettimeofday(&tv_recv, NULL);
	if (bytes == -1) {
		printf("receive packet: 0 bytes\n");
		return -1;
	}

	parse_packet(datas, &tv_recv);
	return 0;
}

int	print_packet_infos(struct iphdr *iphdr, uint16_t len_iphdr, struct icmphdr *icmphdr) {

	unsigned short	bytes;
	char		src[INET_ADDRSTRLEN];

	memset(src, 0, INET_ADDRSTRLEN);
	bytes = ntohs(iphdr->tot_len) - len_iphdr;	
	if (inet_ntop(AF_INET, &iphdr->saddr, src, INET_ADDRSTRLEN) == NULL) {
		printf("Error: (recv packet) inet_ntop call failed (bad address) %s\n", strerror(errno));
		return (-1);
	}	
	printf("%u bytes from %s: icmp_seq=%u ttl=%u ", bytes, src, icmphdr->un.echo.sequence, iphdr->ttl);
	return 0;
}


void	print_time(struct timeval *tv_send, struct timeval *tv_recv) {

	struct timeval	diff;
	struct stat_s	*stat;


	stat = get_stat();
	stat->p_recv++;

	//diff.tv_sec = tv->tv_recv.tv_sec - tv->tv_send.tv_sec;
	diff.tv_usec = tv_recv->tv_usec - tv_send->tv_usec;

	if (!stat->min || stat->min > diff.tv_usec)
		stat->min = diff.tv_usec;
	if (!stat->max || stat->max < diff.tv_usec)
		stat->max = diff.tv_usec;	
	stat->tot += diff.tv_usec;
	stat->rtts[stat->p_recv - 1] = diff.tv_usec;
	
	printf("time=%ld,%03ld ms\n", diff.tv_usec / 1000, diff.tv_usec % 1000);
}


int	parse_packet(unsigned char *datas, struct timeval *tv_recv) {

	struct iphdr	*iphdr;
	uint16_t	len_iphdr;
	struct icmphdr	*icmphdr;
	struct timeval	*tv_send;

	iphdr = (struct iphdr *)datas;
	len_iphdr = iphdr->ihl * 4;	
	icmphdr = (struct icmphdr *)(datas + len_iphdr);		
	tv_send = (struct timeval *)(datas + len_iphdr + 8);
	
	char *TYPES[] = {
		[ICMP_ECHOREPLY] = "Echo Reply",
		[ICMP_DEST_UNREACH] = "Destination Unreachable",
		[ICMP_SOURCE_QUENCH] = "Source Quench",
		[ICMP_REDIRECT] = "Redirect (change route)",
		[ICMP_ECHO] = "Echo Request",
		[ICMP_TIME_EXCEEDED] = "Destination Unreachable",
		[ICMP_PARAMETERPROB] = "Parameter Problem",
		[ICMP_TIMESTAMP] = "Timestamp Request",
		[ICMP_TIMESTAMPREPLY] = "Timestamp Reply",
		[ICMP_INFO_REQUEST] = "Information Request",
		[ICMP_INFO_REPLY] = "Information Reply",
		[ICMP_ADDRESS] = "Address Mask Request",
		[ICMP_ADDRESSREPLY] = "Address Mask Reply",
	};	
	(void)TYPES;	

	print_packet_infos(iphdr, len_iphdr, icmphdr);
	print_time(tv_send, tv_recv);
	return 1;
}


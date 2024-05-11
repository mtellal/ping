
#include <ft_ping.h>

unsigned char *recv_packet(int sockfd, struct sockaddr_in *ip_src, struct timeval_s *tv) {

	int len;
	int bytes;
	unsigned short size = 64;
	unsigned char *datas = NULL;

	datas = malloc(size);
	len = sizeof(ip_src);
	bytes = recvfrom(sockfd, datas, size, 0, (struct sockaddr*)ip_src, (socklen_t*)&len);
	gettimeofday(&tv->tv_recv, NULL);
	if (bytes == -1) {
		printf("bytes = -1\n");
	}

	return datas;
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


void	print_time(struct timeval_s *tv, struct stat_s *stat) {

	struct timeval	diff;

	//diff.tv_sec = tv->tv_recv.tv_sec - tv->tv_send.tv_sec;
	diff.tv_usec = tv->tv_recv.tv_usec - tv->tv_send.tv_usec;

	if (!stat->min || stat->min > diff.tv_usec)
		stat->min = diff.tv_usec;
	if (!stat->max || stat->max < diff.tv_usec)
		stat->max = diff.tv_usec;	
	stat->tot += diff.tv_usec;
	stat->rtts[stat->p_recv - 1] = diff.tv_usec;
	
	printf("time=%ld,%03ld ms\n", diff.tv_usec / 1000, diff.tv_usec % 1000);
}


int	parse_packet(unsigned char *datas, struct timeval_s *tv) {

	struct iphdr	*iphdr;
	uint16_t	len_iphdr;
	struct icmphdr	*icmphdr;
	
	struct stat_s	*stat;

	iphdr = (struct iphdr *)datas;
	len_iphdr = iphdr->ihl * 4;	
	icmphdr = (struct icmphdr *)(datas + len_iphdr);		
	
	stat = get_stat();
	stat->p_recv++;
	
	print_packet_infos(iphdr, len_iphdr, icmphdr);
	print_time(tv, stat);
	return 1;
}


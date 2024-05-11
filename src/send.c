#include <ft_ping.h>

unsigned short checksum(void *b, int len) {    
	unsigned short *buf = b;
	unsigned int sum = 0;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char *)buf;
	while (sum >> 16)
		sum += (sum >> 16) + (sum & 0xffff);
	return ~sum;
}

struct icmp_packet create_packet(){

	static int msg_count = 0;
	struct icmp_packet packet;

        memset(&packet, 0, sizeof(packet));

	packet.icmphdr.type = ICMP_ECHO; // marco in netinet/ip_icmp.h 
	packet.icmphdr.code = 0; // code from rfc

	packet.icmphdr.un.echo.id = getpid();
	packet.icmphdr.un.echo.sequence = msg_count++;

	packet.icmphdr.checksum = checksum(&packet, sizeof(packet));
	//printf("checksum calculated: %i\n", packet.icmphdr.checksum);
	return packet;
}

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
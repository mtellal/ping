#include <ft_ping.h>

unsigned short checksum(void *b, int len) {    

	uint16_t *	buf;
	uint32_t 	sum;

	buf = b;
	sum = 0;
	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(uint8_t *)buf;
	while (sum >> 16)
		sum = (sum >> 16) + (sum & 0xffff);
	return ~sum;
}

struct icmp_packet create_packet(struct stat_s *stat){

	struct timeval		t;
	static int			msg_count = 0;
	struct icmp_packet	packet;

    memset(&packet, 0, sizeof(packet));
	packet.icmphdr.type = ICMP_ECHO;  
	packet.icmphdr.un.echo.id = htons(stat->pid);
	packet.icmphdr.un.echo.sequence = htons(msg_count++);
	gettimeofday(&t, NULL);
	memcpy(packet.data, &t, sizeof(t));
	packet.icmphdr.checksum = checksum(&packet.icmphdr, sizeof(packet));
	return packet;
}

int send_packet(struct stat_s * stat) {

	int						bytes;
	struct sockaddr_in		ip;
	struct icmp_packet		icmp_packet;

	icmp_packet = create_packet(stat);
	memcpy(&ip, &stat->ip_dst, sizeof(struct sockaddr_in));
	bytes = sendto(stat->sockfd, &icmp_packet, sizeof(icmp_packet), 0, (struct sockaddr *)&ip, sizeof(ip));
	if (bytes == -1) {
		printf("sending packet: %s \n", strerror(errno));
		// free(stat->ip_dst);
		exit(1);
	}
	stat->p_sent++;
	return 0;
}

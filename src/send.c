#include <ft_ping.h>

unsigned short checksum(void *b, int len) {    
	unsigned short *buf = b;
	unsigned int 	sum = 0;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char *)buf;
	while (sum >> 16)
		sum = (sum >> 16) + (sum & 0xffff);
	return ~sum;
}

struct icmp_packet create_packet(){

	static int		msg_count = 0;
	struct icmp_packet	packet;
	struct timeval		t;

        memset(&packet, 0, sizeof(packet));

	packet.icmphdr.type = ICMP_ECHO;  
	packet.icmphdr.code = 0; 
	packet.icmphdr.un.echo.id = getpid();
	packet.icmphdr.un.echo.sequence = msg_count++;

	gettimeofday(&t, NULL);
	memcpy(packet.data, &t, sizeof(t));

	packet.icmphdr.checksum = checksum(&packet, sizeof(packet));
	return packet;
}

int send_packet(int sockfd, struct sockaddr_in ip_dst) {

	int			bytes;
	struct stat_s		*stat;
	struct icmp_packet	icmp_packet;

	icmp_packet = create_packet();
	stat = get_stat();
	bytes = sendto(sockfd, &icmp_packet, sizeof(icmp_packet), 0, (struct sockaddr *)&ip_dst, sizeof(ip_dst));
	if (bytes == -1) {
		printf("sendto call failed: %s \n", strerror(errno));
		return 1;	
	}
	stat->p_sent++;
	return 0;
}

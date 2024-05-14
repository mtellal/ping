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

	static int			msg_count = 0;
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

int send_packet(int sockfd, struct sockaddr_in *ip_dst) {

	int					bytes;
	struct stat_s		*stat;
	struct icmp_packet	icmp_packet;
	struct sockaddr_in		ip;

	icmp_packet = create_packet();
	stat = get_stat();

	char host[INET_ADDRSTRLEN];
	memcpy(&ip, ip_dst, sizeof(struct sockaddr_in));
	(void)host;


/* 	if (getnameinfo((struct sockaddr *)ip_dst, sizeof(struct sockaddr), host, INET_ADDRSTRLEN, 0, 0, AI_NUMERICHOST) == -1)
		printf("error getnameinfo %s\n", strerror(errno));

	printf("host by getnameinfo %s\n", host); */

	bytes = sendto(sockfd, &icmp_packet, sizeof(icmp_packet), 0, (struct sockaddr *)&ip, sizeof(ip));
	if (bytes == -1) {
		printf("sending packet: %s \n", strerror(errno));
		exit(1);
	}
	stat->p_sent++;
	return 0;
}

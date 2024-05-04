#include <ft_ping.h>

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


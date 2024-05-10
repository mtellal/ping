
#include <ft_ping.h>

unsigned char *recv_packet(int sockfd, struct sockaddr_in *ip_src) {

	int len;
	int bytes;
	unsigned short size = 64;
	unsigned char *datas = NULL;

	datas = malloc(size);
	len = sizeof(ip_src);
	bytes = recvfrom(sockfd, datas, size, 0, (struct sockaddr*)ip_src, (socklen_t*)&len);
	if (bytes == -1) {
		printf("bytes = -1\n");
	}
	return datas;
}


struct ip_packet {
	struct iphdr *iphdr;
	struct icmphdr *icmphdr;
	char *data;
};

void display_datas(unsigned char *datas, int len) {

	
	unsigned short *hex = (unsigned short *)datas;

	for (; len > 1; len-=2) {
		printf("%x ", *hex++);
	}
	printf("\n");	
}

/*
 *struct iphdr
 *{
 *#if __BYTE_ORDER == __LITTLE_ENDIAN
 *  unsigned int ihl:4;
 *  unsigned int version:4;
 *#elif __BYTE_ORDER == __BIG_ENDIAN
 *  unsigned int version:4;
 *  unsigned int ihl:4;
 *#else
 *# error	"Please fix <bits/endian.h>"
 *#endif
 *  uint8_t tos;
 *  uint16_t tot_len;
 *  uint16_t id;
 *  uint16_t frag_off;
 *  uint8_t ttl;
 *  uint8_t protocol;
 *  uint16_t check;
 *  uint32_t saddr;
 *  uint32_t daddr;
 *};
*/

void print_iphdr(struct iphdr *iphdr) {
	printf("version: %x\n", iphdr->version);
	printf("ihl: %x (%i bytes)\n",iphdr->ihl, iphdr->ihl * 4); 
	printf("tos: %x\n", iphdr->tos);
	printf("tot_len: %x (%i bytes)\n", htons(iphdr->tot_len), htons(iphdr->tot_len));
	printf("id: %x (%i)\n", htons(iphdr->id), htons(iphdr->id));
	printf("frag_off: %x\n", htons(iphdr->frag_off));
	printf("ttl: %x (%i)\n", iphdr->ttl, iphdr->ttl);
	printf("protocol: %x\n", iphdr->protocol);
	printf("check: %x\n", htons(iphdr->check));
}


/*
 *struct icmphdr
 *{
 * uint8_t type;		// message type 
 * uint8_t code;		// type sub-code 
 * uint16_t checksum;
 * union
 * {
 *   struct
 *   {
 *     uint16_t	id;
 *     uint16_t	sequence;
 *   } echo;			/ echo datagram
 *   uint32_t	gateway;	// gateway address 
 *   struct
 *   {
 *     uint16_t	__glibc_reserved;
 *     uint16_t	mtu;
 *   } frag;			// path mtu discovery
 *} un;
 *};
*/

/*
 *    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |     Code      |          Checksum             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           Identifier          |        Sequence Number        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 
*/


void print_icmphdr(struct icmphdr *icmphdr) {
	printf("type: %x (%i)\n", icmphdr->type, icmphdr->type);
	printf("code: %x (%i)\n", icmphdr->code, icmphdr->code);
	printf("checksum: %x\n", ntohs(icmphdr->checksum));
	printf("id: %x (%i)\n", ntohs(icmphdr->un.echo.id), ntohs(icmphdr->un.echo.id));
	printf("sequence: %x (%i)\n", ntohs(icmphdr->un.echo.sequence), ntohs(icmphdr->un.echo.sequence));	
}

int	parse_packet(unsigned char *datas) {

	struct iphdr	*iphdr;
	struct icmphdr	*icmphdr;

	unsigned short	len_iphdr;
	unsigned short	data_bytes;


	(void)icmphdr;
	iphdr = (struct iphdr *)datas;
	len_iphdr = iphdr->ihl * 4;
	
	icmphdr = (struct icmphdr *)(datas + len_iphdr);	
	
	data_bytes = ntohs(iphdr->tot_len) - len_iphdr;

	char src[INET_ADDRSTRLEN];
	const char *ip_src = inet_ntop(AF_INET, &iphdr->saddr, src, INET_ADDRSTRLEN);

	printf("%u bytes from %s: icmp_seq= \n", data_bytes, ip_src);
	return 1;
}


#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>


int main(int argc, char **argv) {

	if (argc != 2) {
		printf("ping: usage error: Destination address required");
		return 1;
	}

	// Selon une address/nom de domaine
	// recuperer les informations -> 
	// getaddrinfo transform

	struct addrinfo hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;


	int error = getaddrinfo(argv[1], 0, &hints, &res);
	if (error) {
		printf("getaddrinfo call failed: %s", gai_strerror(error));
	}
	


	// ask for a socket
	// socket(domain, type, protocol)

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd == -1)
	{
		printf("socket call failed %s", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("socket command successfully executed: %d", sockfd);


}






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


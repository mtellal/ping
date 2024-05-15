#include <ft_ping.h>


void	print_help() {
	printf("Usage: ping [OPTION...] HOST ...\nSend ICMP ECHO_REQUEST packets to network hosts.\n");
	printf("\n");
	printf("Options:\n");
	printf("\t%7s,  give this help list\n", "-?");
	printf("\t%7s,  verbose output\n", "-v");
	printf("\t%7s,  specify N as time-to-live\n", "--ttl=N");
	exit(EXIT_SUCCESS);
}


int	valid_options(struct stat_s *stat, char *path, char *argv) {

    int            ttl_value;
    char            *end;

	stat = get_stat();
    if (!memcmp(argv, "--ttl=", strlen("--ttl="))) {
        ttl_value = (int)strtol(argv + 6, &end, 10);
        if (end == argv + 6 || strlen(end)){
            printf("%s: invalid value near (%s)\n", path, argv + 6);
            exit(EXIT_FAILURE);
        }
        if (ttl_value == 0) {
            printf("%s: option value too small: %s\n", path, argv + 6);
            exit(EXIT_FAILURE);
        }
        else if (ttl_value > 255 || ttl_value < 0) {
            printf("%s: option value too big: %s\n", path, argv + 6);
            exit(EXIT_FAILURE);
        }
        stat->ttl = ttl_value;
        return 1;
    }
    else if (argv[0] == '-') {
        for (size_t j = 1; j < strlen(argv); j++) {
            if (argv[j] == 'v') {
                stat->options |= OPT_VERBOSE;
            }
            else if (argv[j] == '?')
                print_help();
            else {
                printf("%s: invalid option -- '%c'\nTry 'ping -?' for more information.\n", path, argv[j]);
                exit(EX_USAGE);
            }
        }
        return 1;
    }
	return 0;
}

char	*parse_args(int argc, char **argv) {

	struct stat_s	*stat;
	uint16_t	    i;
	
	i = 1;
	stat = get_stat();
	stat->options = 0;
	while (i < argc) {
		if (valid_options(stat, argv[0], argv[i])) {
                if (argc - 1 == i)
				    exit_miss_host();
        }
		else
			return argv[i];
		i++;
	}
	return NULL;
}

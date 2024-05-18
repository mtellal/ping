#include <ft_ping.h>


void	print_help() {
	printf("Usage: ping [OPTION...] HOST ...\nSend ICMP ECHO_REQUEST packets to network hosts.\n");
	printf("\n");
	printf("Options:\n");
	printf("\t%14s,  give this help list\n", "-?");
	printf("\t%14s,  verbose output\n", "-v");
	printf("\t%14s,  specify N as time-to-live\n", "--ttl=N");
	printf("\t%14s,  stop after sending NUMBER packets\n", "--count=NUMBER");
	exit(EXIT_SUCCESS);
}


int	valid_options(struct stat_s *stat, char *path, char *argv) {

    int            tmp_value;
    char            *end;

	stat = get_stat();
    if (!memcmp(argv, "--count=", 8)) {
        tmp_value = (int)strtol(argv + 8, &end, 10);
        if (end == argv + 8 || strlen(end)){
            printf("%s: invalid value near (%s)\n", path, argv + 6);
            exit(EXIT_FAILURE);
        }
        if (tmp_value > 0) {
            stat->opt_number = tmp_value;
            stat->options |= OPT_NUMBER;
        }
        return 1;
    }
    if (!memcmp(argv, "--ttl=", 6)) {
        tmp_value = (int)strtol(argv + 6, &end, 10);
        if (end == argv + 6 || strlen(end)){
            printf("%s: invalid value near (%s)\n", path, argv + 6);
            exit(EXIT_FAILURE);
        }
        if (tmp_value == 0) {
            printf("%s: option value too small: %s\n", path, argv + 6);
            exit(EXIT_FAILURE);
        }
        else if (tmp_value > 255 || tmp_value < 0) {
            printf("%s: option value too big: %s\n", path, argv + 6);
            exit(EXIT_FAILURE);
        }
        stat->opt_ttl = tmp_value;
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
    stat->opt_ttl = DEFAULT_TTL;
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

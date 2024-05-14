#include <ft_ping.h>


struct stat_s *get_stat() {

	static struct stat_s stat;
	
	return &stat;
}

double	calcul_stddev(struct stat_s *stat) {
	
	double		mean;
	unsigned int	i;
	
	double		mu;
	double		tmp;


	mean = stat->tot / stat->p_recv;
	i = 0;
	while (i < stat->p_recv) {
		tmp = (stat->rtts[i] - mean);
		tmp *= tmp;
		mu += tmp;
		i++;	
	}
	return sqrt(mu / stat->p_recv);
}

void signalhandler(int s) {
	
	struct stat_s	*stat;
	unsigned short	loss;
	suseconds_t	stddev;
	suseconds_t	avg;

	(void)s;	
	stat = get_stat();
	printf("--- %s ping statistics ---\n", stat->host);
	loss = ((1 - ((float)stat->p_recv / (float)stat->p_sent)) * 100);
	printf("%i packets transmitted, %i packets received, %u%% packet loss\n", stat->p_sent, stat->p_recv, loss);
	if (!stat->err && stat->min) {
		avg = stat->tot / stat->p_recv;
		stddev = (suseconds_t)calcul_stddev(stat);
		printf("round-trip min/avg/max/stddev = %ld,%03ld/%ld,%03ld/%ld,%03ld/%ld,%03ld ms\n", stat->min / 1000, stat->min % 1000, avg / 1000, avg % 1000, stat->max / 1000, stat->max % 1000, stddev / 1000, stddev % 1000);
	}
	exit(EXIT_SUCCESS);
}

#include <ft_ping.h>


struct stat_s *get_stat() {

	static struct stat_s stat;
	
	return &stat;
}

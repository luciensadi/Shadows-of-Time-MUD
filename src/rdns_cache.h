//
// cached reverse lookup
//
// for version 3.3 (release 4)
//
// $Id: rdns_cache.h,v 1.4 2001/05/01 06:51:08 joost Exp $
#ifndef RDNS_CACHE
#define RDNS_CACHE

void	rdns_cache_destroy(void);
int	rdns_cache_init(void);
void	rdns_cache_set_ttl(int new_ttl);

char * gethostname_cached(const unsigned char *addr, int len, int ttl_refresh);

#endif

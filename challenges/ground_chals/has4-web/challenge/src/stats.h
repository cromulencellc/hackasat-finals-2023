#ifndef stats_h

#define stats_h

#include "bst.h"

void hostStatsTable( int fd, bstType *stats );
void hostStatsHeader( int fd );
void hostStatsFooter( int fd );
void uaStatsTable( int fd, bstType *stats );
void uaStatsHeader( int fd );
void uaStatsFooter( int fd );

#endif
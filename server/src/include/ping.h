#ifndef _PING_H_
#define _PING_H_ 1


typedef struct ping_data {
     int                  ping_count;         /* counts up to -c count or 1 */
     struct timeval       ping_ts;            /* time sent */
} PING_DATA;

#define SIZE_ICMP_HDR   ICMP_MINLEN     /* from ip_icmp.h */
#define MIN_PING_DATA   sizeof( PING_DATA )
#define DEFAULT_RETRY 3            /* number of times to retry a host */
#define DEFAULT_SELECT_TIME 2000   /* default time to wait during select() */
#define ICMP_UNREACH_MAXTYPE    15
#define DEFAULT_PING_DATA_SIZE  ( MIN_PING_DATA + 44 ) 
#define MY_ICMP_SEQ     1004
#define MY_ICMP_ID      (getpid() & 0xFFFF)


long timeval_diff( struct timeval *a, struct timeval *b );
int in_cksum( u_short *p, int n );
int isRemoteHostAlive(char *remoteHost);
int recvfrom_wto( int s, char *buf, int len, struct sockaddr *saddr, int timo );
int handle_random_icmp( struct icmp *p, struct sockaddr_in *addr );
int wait_for_reply( int s, int select_time );

#endif /* _PING_H_ */
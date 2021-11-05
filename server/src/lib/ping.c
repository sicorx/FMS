#include "include.h"

unsigned int ping_data_size = DEFAULT_PING_DATA_SIZE;
unsigned int ping_pkt_size;
struct timezone tz;

char *icmp_type_str[19] =
{
    "ICMP Echo Reply",          /* 0 */
    "",
    "",
    "ICMP Unreachable",         /* 3 */
    "ICMP Source Quench",       /* 4 */
    "ICMP Redirect",            /* 5 */
    "",
    "",
    "ICMP Echo",                /* 8 */
    "",
    "",
    "ICMP Time Exceeded",       /* 11 */
    "ICMP Paramter Problem",    /* 12 */
    "ICMP Timestamp Request",   /* 13 */
    "ICMP Timestamp Reply",     /* 14 */
    "ICMP Information Request", /* 15 */
    "ICMP Information Reply",   /* 16 */
    "ICMP Mask Request",        /* 17 */
    "ICMP Mask Reply"           /* 18 */
};

char *icmp_unreach_str[16] =
{
    "ICMP Network Unreachable",                                     /* 0 */
    "ICMP Host Unreachable",                                        /* 1 */
    "ICMP Protocol Unreachable",                                    /* 2 */
    "ICMP Port Unreachable",                                        /* 3 */
    "ICMP Unreachable (Fragmentation Needed)",                      /* 4 */
    "ICMP Unreachable (Source Route Failed)",                       /* 5 */
    "ICMP Unreachable (Destination Network Unknown)",               /* 6 */
    "ICMP Unreachable (Destination Host Unknown)",                  /* 7 */
    "ICMP Unreachable (Source Host Isolated)",                      /* 8 */
    "ICMP Unreachable (Communication with Network Prohibited)",     /* 9 */
    "ICMP Unreachable (Communication with Host Prohibited)",        /* 10 */
    "ICMP Unreachable (Network Unreachable For Type Of Service)",   /* 11 */
    "ICMP Unreachable (Host Unreachable For Type Of Service)",      /* 12 */
    "ICMP Unreachable (Communication Administratively Prohibited)", /* 13 */
    "ICMP Unreachable (Host Precedence Violation)",                 /* 14 */
    "ICMP Unreachable (Precedence cutoff in effect)"                /* 15 */
};

long timeval_diff( struct timeval *a, struct timeval *b )
{
    double temp;

    temp = ( ( ( a->tv_sec * 1000000 ) + a->tv_usec ) - ( ( b->tv_sec * 1000000 ) + b->tv_usec ) );

    return (long)temp;
}

int in_cksum( u_short *p, int n )
{
    unsigned short answer, odd_byte = 0;
    long sum = 0;

    while( n > 1 )
    {
        sum += *p++;
        n -= 2;
    }

    /* mop up an odd byte, if necessary */
    if( n == 1 )
    {
        *( u_char* )( &odd_byte ) = *( u_char* )p;
        sum += odd_byte;
    }

    sum  = ( sum >> 16 ) + ( sum & 0xffff ); /* add hi 16 to low 16 */
    sum += ( sum >> 16 );                    /* add carry */
    answer = ~sum;                           /* ones-complement, truncate*/

    return ( answer );
}

int isRemoteHostAlive(char *remoteHost)
{
    int n=0, s=0, result=0;
    unsigned int select_time = DEFAULT_SELECT_TIME * 100;
    char buffer[4096];
    PING_DATA *pdp;
    struct icmp *icp;
    struct protoent *proto;
    struct sockaddr_in client_addr;
    struct hostent *hostPtr = NULL;
    struct timeval last_send_time;

    ping_pkt_size = ping_data_size + SIZE_ICMP_HDR;

    /* confirm that ICMP is available on this machine */
    if( ( proto = getprotobyname( "icmp" ) ) == NULL )
    {
        fileLog(CRITICAL, " error getprotobyname: %s\n", strerror( errno ) );
        return -1;
    }

    /* create raw socket for ICMP calls (ping) */
    s = socket( AF_INET, SOCK_RAW, proto->p_proto );

    hostPtr = gethostbyname(remoteHost);
    if(hostPtr == NULL)
    {
        hostPtr = gethostbyaddr(remoteHost, strlen(remoteHost), AF_INET);
        if(hostPtr == NULL)
        {
            close(s);
            fileLog(CRITICAL, " error gethostbyaddr: %s\n", strerror( errno ) );
            return -1;
        }
    }

    client_addr.sin_family = AF_INET;
    (void)memcpy(&client_addr.sin_addr, hostPtr->h_addr, hostPtr->h_length);

    //send_ping( s, &client_addr, 0, 1 );
    memset( &buffer, 0, ping_pkt_size * sizeof( char ) );
    icp = ( struct icmp* )&buffer;

    icp->icmp_type  = ICMP_ECHO;
    icp->icmp_code  = 0;
    icp->icmp_cksum = 0;
    icp->icmp_seq   = MY_ICMP_SEQ;
    icp->icmp_id    = MY_ICMP_ID;

    pdp = ( PING_DATA* )( buffer + SIZE_ICMP_HDR );
    gettimeofday( &last_send_time, &tz );
    pdp->ping_ts = last_send_time;
    pdp->ping_count = 0;

    icp->icmp_cksum = in_cksum( ( unsigned short* )icp, ping_pkt_size );
    n = sendto( s, buffer, ping_pkt_size, MSG_DONTWAIT, ( struct sockaddr* )&client_addr, sizeof( struct sockaddr_in ) );

    if( n < 0 || n != ping_pkt_size )
    {
        fileLog(CRITICAL, "host(%s) error while sending ping: %s\n", remoteHost, strerror( errno ) );
        return -1;
    }

    while ( 1 )
    {
        result = wait_for_reply(s, select_time);
        if(result == 2 || result == 0)
            break;
    }

    close(s);

    return result;
}

int recvfrom_wto( int s, char *buf, int len, struct sockaddr *saddr, int timo )
{
    int nfound, slen, n;
    struct timeval to;
    fd_set readset, writeset;

    to.tv_sec = timo / 100000;
    to.tv_usec = ( timo - ( to.tv_sec * 100000 ) ) * 10;

    FD_ZERO( &readset );
    FD_ZERO( &writeset );
    FD_SET( s, &readset );
    nfound = select( s + 1, &readset, &writeset, NULL, &to );

    if( nfound < 0 )
        return -1;

    if( nfound == 0 )
        return -1;      /* timeout */

    slen = sizeof( struct sockaddr );
    n = recvfrom( s, buf, len, 0, saddr, (socklen_t *)&slen );
    if( n < 0 )
        return -1;

    return n;
}

int handle_random_icmp( struct icmp *p, struct sockaddr_in *addr )
{
    struct icmp *sent_icmp;
    unsigned char *c;

    c = ( unsigned char* )p;

    switch( p->icmp_type )
    {
    case ICMP_UNREACH:
        sent_icmp = ( struct icmp* )( c + 28 );

        if( ( sent_icmp->icmp_type == ICMP_ECHO ) &&
            ( sent_icmp->icmp_id == MY_ICMP_ID ) )
        {
            /* this is a response to a ping we sent */
            if( p->icmp_code > ICMP_UNREACH_MAXTYPE )
            {
                fileLog(CRITICAL, "ICMP Unreachable (Invalid Code) from %s for ICMP Echo sent to %s\n", inet_ntoa( addr->sin_addr ), "h->host" );
            }
            else
            {
                fileLog(CRITICAL, "%s from %s for ICMP Echo sent to\n", icmp_unreach_str[p->icmp_code], inet_ntoa( addr->sin_addr ) );
            }
        }

        return 1;

    case ICMP_SOURCEQUENCH:
    case ICMP_REDIRECT:
    case ICMP_TIMXCEED:
    case ICMP_PARAMPROB:
        sent_icmp = ( struct icmp* )( c + 28 );
        if( ( sent_icmp->icmp_type = ICMP_ECHO ) && 
            ( sent_icmp->icmp_id = MY_ICMP_ID ) )
        {
            /* this is a response to a ping we sent */
            fileLog(CRITICAL, "%s from %s for ICMP Echo sent to\n", icmp_type_str[p->icmp_type], inet_ntoa( addr->sin_addr ) );
        }

        return 2;

    /* no way to tell whether any of these are sent due to our ping */
    /* or not (shouldn't be, of course), so just discard            */
    case ICMP_TSTAMP:
    case ICMP_TSTAMPREPLY:
    case ICMP_IREQ:
    case ICMP_IREQREPLY:
    case ICMP_MASKREQ:
    case ICMP_MASKREPLY:
    default:
        return 0;
    }
}

int wait_for_reply( int s, int select_time )
{
    int result, hlen;
    static char buffer[4096];
    struct sockaddr_in response_addr;
    struct ip *ip;
    struct icmp *icp;
    struct timeval sent_time;
    struct timeval current_time;

    result = recvfrom_wto( s, buffer, 4096,( struct sockaddr* )&response_addr, select_time );
    if( result < 0 )
        return 2;   /* timeout */

    ip = ( struct ip* )buffer;

#if defined( __alpha__ ) && __STDC__ && !defined( __GLIBC__ )
    /* The alpha headers are decidedly broken.
     * Using an ANSI compiler, it provides ip_vhl instead of ip_hl and
     * ip_v.  So, to get ip_hl, we mask off the bottom four bits.
     */
    hlen = ( ip->ip_vhl & 0x0F ) << 2;
#else
    hlen = ip->ip_hl << 2;
#endif /* defined(__alpha__) && __STDC__ */

    if( result < hlen + ICMP_MINLEN )
    {
        fileLog(CRITICAL, "received packet too short for ICMP (%d bytes from %s)\n", result, inet_ntoa( response_addr.sin_addr ) );

        return 1; /* too short */ 
    }

    icp = ( struct icmp * )( buffer + hlen );
    if( icp->icmp_type != ICMP_ECHOREPLY )
    {
        /* handle some problem */
        handle_random_icmp( icp, &response_addr );
        return 1;
    }

    if( icp->icmp_id != MY_ICMP_ID && icp->icmp_seq != MY_ICMP_SEQ)
        return 1; /* packet received, but not the one we are looking for! */

    /* received ping is cool, so process it */
    gettimeofday( &current_time, &tz );

    memcpy( &sent_time, icp->icmp_data + offsetof( PING_DATA, ping_ts ), sizeof( sent_time ) );

//    printf( "host is alive " );
//    printf( "time=%ld.%03ldms\n", timeval_diff( &current_time, &sent_time ) / 100000, timeval_diff( &current_time, &sent_time ));

    return 0;
}

/*
 *  Based upon 
 *    http://www.rtems.com/ml/rtems-users/2005/may/msg00022.html
 */

/*

http://ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/index.html

Every IP multicast group has a group address. IP multicast provides only
open groups: That is, it is not necessary to be a member of a group in
order to send datagrams to the group.

Multicast address are like IP addresses used for single hosts, and is
written in the same way: A.B.C.D. Multicast addresses will never clash
with host addresses because a portion of the IP address space is
specifically reserved for multicast. This reserved range consists of
addresses from 224.0.0.0 to 239.255.255.255. However, the multicast
addresses from 224.0.0.0 to 224.0.0.255 are reserved for multicast
routing information; Application programs should use multicast
addresses outside this range.

*/

/*
 * Trying to have same effect of this command:
 *
 * /sbin/route add -net 239.9.8.0  netmask 255.255.255.0 gw 10.4.10.19 dev eth0
 */


#include <rtems/rtems_bsdnet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/route.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <sys/errno.h>


int add_mcast_route(
  char *multi_address,
  char *multi_netmask,
  char *multi_gateway
)
{
  int s;
  struct sockaddr_in address;
  struct sockaddr_in netmask;
  struct sockaddr_in gateway;

  memset(&address,0,sizeof(address));
  address.sin_len = sizeof(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr( multi_address );

  memset(&netmask,0,sizeof(netmask));
  netmask.sin_len = sizeof(netmask);
  netmask.sin_family = AF_INET;
  netmask.sin_addr.s_addr = inet_addr( multi_netmask );

  memset(&gateway,0,sizeof(gateway));
  gateway.sin_len = sizeof(gateway);
  gateway.sin_family = AF_INET;
  gateway.sin_addr.s_addr = inet_addr( multi_gateway );

  s = rtems_bsdnet_rtrequest(
    RTM_DELETE,
    (struct sockaddr *)&address,
    (struct sockaddr *)&gateway,
    (struct sockaddr *)&netmask,
    (RTF_GATEWAY | RTF_MULTICAST | RTF_STATIC),
    NULL
  );
  if ( s == -1 ) {
    fprintf( stderr, "RTM_DELETE failed errno=%d (%s)\n", 
        errno, strerror(errno) );
  }

  s = rtems_bsdnet_rtrequest(
    RTM_ADD,
    (struct sockaddr *)&address,
    (struct sockaddr *)&gateway,
    (struct sockaddr *)&netmask,
    (RTF_UP | RTF_GATEWAY | RTF_MULTICAST | RTF_STATIC),
    NULL
  );
  if ( s == -1 ) {
    fprintf( stderr, "RTM_ADD failed errno=%d (%s)\n", 
        errno, strerror(errno) );
    return -1;
  }
  fprintf( stderr, "Route added\n" );
  return 0;
}

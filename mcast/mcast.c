#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>
#ifdef  __Lynx__
#include <socket.h>
#else
#include <sys/socket.h>
#endif
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>


#include "mcast_params.h"

#define ROUTERS_TO_HOP  10              //      Keep my traffic in the same subnet


static  struct sockaddr_in addr;
static  int so;
static  char    mhost[100];
static  unsigned long   hostaddr;

int msend(char *buf, int num)
{
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons((u_short)MCAST_PORT);
  addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);
  if (sendto(so, buf, num, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    if (errno != EAGAIN)
      perror("sendto");
    return 0;
  }
  return 1;
}


int mrecv(char *buf, int num)
{
  socklen_t i;
  int n;

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons((u_short)MCAST_PORT);
  addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);


  i = sizeof(addr);
  if ((n = recvfrom(so, buf, num, 0, (struct sockaddr *)&addr, &i)) == -1) {
    if (errno != EAGAIN)
      perror("recvfrom");
    return 0;
  }
  return n;
}


#if defined(__rtems__)
int mcast_main(int ac, char **av)
#else
int main(int ac, char **av)
#endif
{
  int yes=1;      
  char line[1024];
  struct ip_mreq imr;
  struct  hostent *h;
  unsigned char mttl;
  int mlen;
  unsigned long long i, j, k;


  if (ac > 1) mlen = atoi(av[1]);
  else       mlen = 0;

  if ((gethostname(mhost, 100) < 0)) {  //  Get the my hostname
    perror("gethostname");
    exit(1);
  }
  printf( "Hostname: %s\n", mhost );
  if (!(h = gethostbyname(mhost))) {    // Get host entry
    perror("gethostbyname");
    exit(1);
  }

  printf( "host=0x%08x\n", *(unsigned int *)h->h_addr );


  printf( "Creating socket\n" );
  if ((so = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  printf( "Setting SO_REUSEADDR\n" );
  if (setsockopt(so, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
    perror("setsockopt, reuse");
    exit(1);
  }

  printf( "Binding\n" );
  //      Bind to port and address.
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons((u_short)MCAST_PORT);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(so, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(1);
  }

  printf( "Performing IP_MULTICAST_TTL\n" );
  mttl = ROUTERS_TO_HOP + 1;              //      ttl should be atleast one.
  if (setsockopt(so, IPPROTO_IP, IP_MULTICAST_TTL, &mttl, sizeof(mttl)) < 0) {
    perror("setsockopt, ttl" );
    exit(1);
  }           
  hostaddr = *(unsigned int *)h->h_addr;
  imr.imr_interface.s_addr = htonl(INADDR_ANY);
  // For some reason, this doesn't seem to work.  I suspect it is because
  // the /etc/hosts included with this demo must match the IP of the
  // network interface you want to use.  I couldn't get this to work
  // so gave up.  INADDR_ANY seems to reliably find the NIC. --joel
  // imr.imr_interface.s_addr = hostaddr;
  imr.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);

  printf( "Performing IP_ADD_MEMBERSHIP\n" );
  if (setsockopt(so, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr)) < 0) {
    perror( "can't join group" );
    close(so);
    exit(1);
  }           

  fcntl(so, F_SETFL, FNDELAY);
  j = k = 0;
  for(i = 1; 1; i++) {
    int     x;
    if (i == 0) j++;
    if (j == 0) k++;
    sleep(1);
    sprintf(line,
      "%s i %" PRId64 ", j %" PRId64 ", k %" PRId64 "", mhost, i, j, k);
    msend(line, strlen(line)+ mlen);
    while ((x = mrecv(line, 500)) > 0) {
      if (addr.sin_addr.s_addr != hostaddr) {
        line[x] = 0;
        printf("Raddr %s recv %s\n", inet_ntoa(addr.sin_addr), line);
      }
    }
  }
} 

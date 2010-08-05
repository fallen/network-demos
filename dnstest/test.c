/*
 * Test RTEMS networking
 *
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *      1. Leave this author information intact.
 *      2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id: test.c,v 1.3 1999/02/10 19:55:26 joel Exp $
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

/*
 * Show network-related statistics
 */
static void
showStatistics (void)
{
	rtems_bsdnet_show_inet_routes ();
	rtems_bsdnet_show_mbuf_stats ();
	rtems_bsdnet_show_if_stats ();
	rtems_bsdnet_show_ip_stats ();
	rtems_bsdnet_show_icmp_stats ();
	rtems_bsdnet_show_udp_stats ();
	rtems_bsdnet_show_tcp_stats ();
}

/*
 * Show host information
 */
static void
showhost (struct hostent *hp)
{
	char **ap;
	struct in_addr in;

	if (hp == NULL) {
		printf ("Host information not available.\n");
		return;
	}
	printf ("Official name: %s\n", hp->h_name);
	ap = hp->h_aliases;
	if (ap && *ap) {
		printf ("Alias%s:\n", ap[1] ? "es" : "");
		while (*ap)
			printf ("    %s\n", *ap++);
	}
	if ((hp->h_addrtype == AF_INET) && (hp->h_length == sizeof in)) {
		ap = hp->h_addr_list;
		if (ap && *ap) {
			printf ("Address%s:", ap[1] ? "es" : "");
			while (*ap) {
				memcpy (&in, *ap++, sizeof in);
				printf (" %s", inet_ntoa (in));
			}
			printf ("\n");
		}
	}
	else {
		printf ("Address type: %d\n", hp->h_addrtype);
		printf ("Address length: %d\n", hp->h_length);
	}
}

/*
 * Test Domain Name Servers
 */
void
testDNS (void)
{
	char namebuf[100];
	char *name, *cp;
	struct hostent *hp;

	for (;;) {
		printf ("\nhost? ");
		if (fgets (namebuf, sizeof namebuf, stdin) == NULL)
			return;
		cp = namebuf;
		while (isspace ((int) *cp))
			cp++;
		if (cp[0] == '\0') {
			showStatistics ();
			continue;
		}
		name = cp;
		while (isgraph ((int) *cp))
			cp++;
		*cp = '\0';
		if (isdigit((int) *name)) {
			struct in_addr addr;

			addr.s_addr = inet_addr (name);
			hp = gethostbyaddr ((char *)&addr, sizeof addr, AF_INET);
		}
		else {
			hp = gethostbyname (name);
		}
		showhost (hp);
	}
}

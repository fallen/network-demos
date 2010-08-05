/*
 * This server runs on the RTEMS target or on the host machine.
 *
 *	W. Eric Norum
 *	eric@cls.usask.ca
 */

#include "msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "msg_server.h"

int
main (int argc, char **argv)
{
	int sock;
	struct sockaddr_in myAddr;
	SVCXPRT *transp;

	/*
	 * Create socket
	 */
	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror ("socket creation problem");
		return 1;
	}

	/*
	 * Bind to local address
	 */
	myAddr.sin_family = htons (AF_INET);
	myAddr.sin_port = htons (TCP_PORT);
	myAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind (sock, (struct sockaddr *)&myAddr, sizeof myAddr) < 0) {
		perror ("bind problem");
		return 2;
	}

	/*
	 * Create TCP server
	 */
	transp = svctcp_create(sock, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "cannot create tcp service.");
		return 1;
	}
	if (!svc_register(transp, MESSAGEPROC, MESSAGEVERS, messageproc_1, 0)) {
		fprintf (stderr, "unable to register (MESSAGEPROC, MESSAGEVERS, tcp).\n");
		return 3;
	}

	/*
	 * Run server
	 */
	svc_run ();
	fprintf (stderr, "svc_run returned");
	return 0;
}

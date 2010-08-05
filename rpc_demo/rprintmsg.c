/*
 * This client runs on the RTEMS target or on the host machine.
 *
 *      W. Eric Norum
 *      eric@cls.usask.ca
 */
#include <stdio.h>
#include <rpc/rpc.h>
#include <netdb.h>
#include "msg.h"

int
main (int argc, char **argv)
{
	struct hostent *hp;
	struct sockaddr_in farAddr;
	int sock = -1;
	CLIENT *c1;
	int *result;
	char *server;
	char *message;
	int itmp;

	/*
	 * Process arguments
	 */
	if (argc != 3) {
		fprintf (stderr, "Usage: %s server message\n", argv[0]);
		return 1;
	}
	server = argv[1];
	message = argv[2];
	hp = gethostbyname (server);
	if (hp == NULL) {
		fprintf (stderr, "No server %s\n", server);
		return 2;
	}

	/*
	 * Set up server address
	 */
	farAddr.sin_family = hp->h_addrtype;
	farAddr.sin_port = htons (TCP_PORT);
	memcpy((char*)&farAddr.sin_addr, hp->h_addr, hp->h_length);

	/*
	 * Create connection to server
	 */
	c1 = clnttcp_create (&farAddr, MESSAGEPROC, MESSAGEVERS, &sock, 0, 0);
	if (c1 == NULL) {
		clnt_pcreateerror (server);
		return 3;
	}

	/*
	 * Call remote procedures
	 */
	result = printmessage_1 (&message, c1);
	if (result == NULL) {
		clnt_perror (c1, server);
		return 4;
	}
	printf ("Printed %d characters on %s.\n", *result, server);

	itmp = 12;
	result = printnum_1 (&itmp, c1);
	if (result == NULL) {
		clnt_perror (c1, server);
		return 5;
	}
	printf ("Result is %d.\n", *result);

	return 0;
}

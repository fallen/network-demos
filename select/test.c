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
 *  $Id: test.c,v 1.5 2007/09/25 16:42:29 joel Exp $
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define BASE_PORT	24742
#define CLIENT_COUNT	2

static int clientfd[CLIENT_COUNT];

static void
getClients (unsigned short port)
{
	int s, s1;
	struct sockaddr_in myAddr, farAddr;
	socklen_t addrlen;
	int clientCount;

	printf ("Create socket.\n");
	s = socket (AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		rtems_panic ("Can't create socket: %s", strerror (errno));
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons (port);
	myAddr.sin_addr.s_addr = INADDR_ANY;
	memset (myAddr.sin_zero, '\0', sizeof myAddr.sin_zero);
	printf ("Bind socket.\n");
	if (bind (s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0)
		rtems_panic ("Can't bind socket: %s", strerror (errno));
	printf ("Listen.\n");
	if (listen (s, 2) < 0)
		rtems_panic ("Can't listen on socket: %s", strerror (errno));

	/*
	 * Accumulate clients
	 */
	for (clientCount = 0 ; clientCount < CLIENT_COUNT ; clientCount++) {
		printf ("Accept.\n");
		addrlen = sizeof farAddr;
		s1 = accept (s, (struct sockaddr *)&farAddr, &addrlen);
		if (s1 < 0)
			rtems_panic ("Can't accept connection: %s", strerror (errno));
		else
			printf ("ACCEPTED:%lX\n", ntohl (farAddr.sin_addr.s_addr));
		clientfd[clientCount] = s1;
	}

	close (s);
}

static void
echoServer (unsigned short port)
{
	fd_set clientfdset;
	int clientCount;
	int topfd = 0;

	getClients (port);

	FD_ZERO (&clientfdset);

	for (clientCount = 0 ; clientCount < CLIENT_COUNT ; clientCount++) {
		int s1;

		s1 = clientfd[clientCount];
		FD_SET (s1, &clientfdset);
		if (s1 > topfd)
			topfd = s1;
	}

	/*
	 * Run clients
	 */
	for (;;) {
		fd_set readfdset;
		struct timeval tv;
		int n;
		int i;

		tv.tv_sec = 5;
		tv.tv_usec = 0;
		readfdset = clientfdset;
		n = select (topfd + 1, &readfdset, NULL, NULL, &tv);
		if (n < 0) {
			printf ("Select() error: %s\n", strerror (errno));
			return;
		}
		if (n == 0) {
			printf ("Timeout\n");
			continue;
		}
	
		printf ("Activity on %d file descriptor%s.\n", n, n == 1 ? "" : "s");
		for (i = 0 ; n && (i < CLIENT_COUNT) ; i++) {
			int fd = clientfd[i];
			if (FD_ISSET (fd, &readfdset)) {
				char buf[200];
				int nread;

				printf ("Activity on file descriptor %d.\n", fd);
				n--;
				nread = read (fd, buf, sizeof buf);
				if (nread < 0) {
					printf ("Read error %s.\n", strerror (errno));
					return;
				}
				if (nread == 0) {
					printf ("EOF\n");
					FD_CLR (fd, &clientfdset);
					close (fd);
					if (--clientCount == 0)
						return;
				}
				printf ("Read %d.\n", nread);
			}
		}
	}
}

void
doSocket (void)
{
	echoServer (BASE_PORT);
}

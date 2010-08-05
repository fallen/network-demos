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
 *  $Id: test.c,v 1.7 2007/06/21 18:26:18 joel Exp $
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
#include <netinet/in.h>

#define NSERVER		2
#define BASE_PORT	24742

#define DATA_SINK_HOST	((128 << 24) | (233 << 16) | (14 << 8) | 60)

void showbroad(int s)
{
	int opt;
	socklen_t optlen = sizeof opt;

	if (getsockopt (s, SOL_SOCKET, SO_BROADCAST, &opt, &optlen) < 0)
		printf ("getsockopt failed: %s\n", strerror (errno));
	printf ("Opt:%d    Optlen:%ld\n", opt, (long)optlen);
}

static
void
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
 * Stress the transmit queue -- send a large number of UDP packets
 */
static void
transmitUdp (void)
{
	int s;
	int i;
	int opt;
	static struct sockaddr_in myAddr, farAddr;
	static char cbuf[800];
	static char bigbuf[9000];

	printf ("Create socket.\n");
	s = socket (AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
		rtems_panic ("Can't create socket: %s", strerror (errno));
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons (1234);
	myAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	printf ("Bind socket.\n");
	if (bind (s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0)
		rtems_panic ("Can't bind socket: %s", strerror (errno));
	farAddr.sin_family = AF_INET;
	farAddr.sin_port = htons (9);	/* The `discard' port */
#if 1
	farAddr.sin_addr.s_addr = htonl (0xFFFFFFFF);
	if (sendto (s, cbuf, sizeof cbuf, 0, (struct sockaddr *)&farAddr, sizeof farAddr) >= 0)
		printf ("Broadcast succeeded, but should not have!\n");
showbroad (s);
	opt = 1;
	if (setsockopt (s, SOL_SOCKET, SO_BROADCAST, &opt, sizeof opt) < 0)
		rtems_panic ("Can't set socket broadcast: %s", strerror (errno));
showbroad (s);
	for (i = 0 ; i < 5 ; i++) {
		if (sendto (s, cbuf, sizeof cbuf, 0, (struct sockaddr *)&farAddr, sizeof farAddr) < 0)
			rtems_panic ("Can't broadcast: %s", strerror (errno));
	}
#endif
	farAddr.sin_addr.s_addr = htonl (DATA_SINK_HOST);
#if 1
	for (i = 0 ; i < 500 ; i++) {
		if (sendto (s, cbuf, sizeof cbuf, 0, (struct sockaddr *)&farAddr, sizeof farAddr) < 0)
			rtems_panic ("Can't send: %s", strerror (errno));
		if (sendto (s, cbuf, sizeof cbuf, 0, (struct sockaddr *)&farAddr, sizeof farAddr) < 0)
			rtems_panic ("Can't send: %s", strerror (errno));
	}
#endif
#if 1
	for (i = 0 ; i < 2 ; i++) {
		if (sendto (s, bigbuf, sizeof bigbuf, 0, (struct sockaddr *)&farAddr, sizeof farAddr) < 0)
			rtems_panic ("Can't send: %s", strerror (errno));
		if (sendto (s, bigbuf, sizeof bigbuf, 0, (struct sockaddr *)&farAddr, sizeof farAddr) < 0)
			rtems_panic ("Can't send: %s", strerror (errno));
	}
#endif
	close (s);
}
	
/*
 * Stress the transmit queue -- send a large number of TCP packets
 */
static void
transmitTcp (void)
{
	int s;
	int i;
	static struct sockaddr_in myAddr, farAddr;
	static char cbuf[800];
	static char bigbuf[20000];

	printf ("Create socket.\n");
	s = socket (AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		rtems_panic ("Can't create socket: %s", strerror (errno));
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons (1234);
	myAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	printf ("Bind socket.\n");
	if (bind (s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0)
		rtems_panic ("Can't bind socket: %s", strerror (errno));
	farAddr.sin_family = AF_INET;
	farAddr.sin_port = htons (9);	/* The `discard' port */
	farAddr.sin_addr.s_addr = htonl (DATA_SINK_HOST);
	printf ("Connect socket.\n");
	if (connect (s, (struct sockaddr *)&farAddr, sizeof farAddr) < 0) {
		printf ("Can't connect socket: %s\n", strerror (errno));
		close (s);
		return;
	}
	printf ("Write to socket.\n");
	for (i = 0 ; i < 500 ; i++) {
		if (write (s, cbuf, sizeof cbuf) < 0)
			rtems_panic ("Can't send: %s", strerror (errno));
	}
	for (i = 0 ; i < 2 ; i++) {
		if (write (s, bigbuf, sizeof bigbuf) < 0)
			rtems_panic ("Can't send: %s", strerror (errno));
	}
	close (s);
}

/*
 * Echo characters back to a telnet session
 *
 * With this running on the test machine you can go to
 * another machine on your network and run:
 *	telnet test_machine the_port_number_with_which_this_function_was_started
 * Everything you type should be echoed back.
 */
static void
echoTask (rtems_task_argument fd)
{
	char cbuf[512];
	int n;
	rtems_status_code sc;

	for (;;) {
#if 1
		n = read (fd, cbuf, sizeof cbuf);
#else
		n = read (fd, cbuf, 1);
#endif
		if (n == 0) {
			printf ("EOF\n");
			break;
		}
		else if (n < 0) {
			rtems_panic ("Error receiving message: %s", strerror (errno));
		}
		printf ("Received: %d\n", n);
		if (send (fd, cbuf, n, 0) < 0)
			rtems_panic ("Error sending message: %s", strerror (errno));
		if (cbuf[0] == 'Q')
			break;
	}
	if (close (fd) < 0)
		rtems_panic ("Can't close connection: %s", strerror (errno));
	sc = rtems_task_delete (RTEMS_SELF);
	rtems_panic ("Task deletion failed: %s", rtems_status_text (sc));
}

static void
echoServer (unsigned short port)
{
	int s, s1;
	struct sockaddr_in myAddr, farAddr;
	socklen_t addrlen;
	rtems_id tid;
	rtems_task_priority my_priority;
	rtems_status_code sc;
	char c = 'a';

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
	for (;;) {
		printf ("Accept.\n");
		addrlen = sizeof farAddr;
		s1 = accept (s, (struct sockaddr *)&farAddr, &addrlen);
		if (s1 < 0)
			rtems_panic ("Can't accept connection: %s", strerror (errno));
		else
			printf ("ACCEPTED:%lX\n", ntohl (farAddr.sin_addr.s_addr));

		/*
		 * Start an echo task
		 */
		rtems_task_set_priority (RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &my_priority);
		sc = rtems_task_create (rtems_build_name ('E', 'C', 'H', c),
			my_priority,
			8*1024,
			RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
			RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
			&tid);
		if (sc != RTEMS_SUCCESSFUL)
			rtems_panic ("Can't create echo task; %s\n", rtems_status_text (sc));
		if (c == 'z')
			c = 'a';
		else
			c++;
		sc = rtems_task_start (tid, echoTask, s1);
		if (sc != RTEMS_SUCCESSFUL)
			rtems_panic ("Can't start echo task; %s\n", rtems_status_text (sc));
	}
}

/*
 * Run an echo server
 */
static void
runEchoServer (rtems_task_argument arg)
{
	echoServer (arg);
	rtems_task_delete (RTEMS_SELF);
}

/*
 * Test some socket stuff
 */
void
doSocket (void)
{
	int i;
	rtems_status_code sc;
	rtems_task_priority my_priority;

#if 1
	/*
	 * Spawn other servers
	 */
	rtems_task_set_priority (RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &my_priority);
	for (i = 0 ; i < NSERVER ; i++) {
		rtems_id tid;
		sc = rtems_task_create (rtems_build_name ('S', 'R', 'V', 'A' + i),
			my_priority - 1,
			8*1024,
			RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
			RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
			&tid);
		if (sc != RTEMS_SUCCESSFUL) {
			printf ("Can't create server; %s\n", rtems_status_text (sc));
			return;
		}
		sc = rtems_task_start (tid, runEchoServer, BASE_PORT + i);
		if (sc != RTEMS_SUCCESSFUL) {
			printf ("Can't start server; %s\n", rtems_status_text (sc));
			return;
		}
	}
#endif

	/*
	 * Wait for characters from console terminal
	 */
	for (;;) {
		switch (getchar ()) {
		case '\004':
			return;

		case 't':
			/*
			 * Test the transmit queues
			 */
			transmitTcp ();
			break;

		case 'u':
			/*
			 * Test the transmit queues
			 */
			transmitUdp ();
			break;

		case 's':
			showStatistics ();
			break;
		}
	}
}

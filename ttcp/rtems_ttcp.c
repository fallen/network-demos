/*
 * A collection of hacks, glue, and patches to
 * provide a `UNIX-like' environment to ttcp.
 *
 * Some of the code here may migrate to the libc
 * support routines some day.  Some of the more sleazy
 * hacks should never make it outside this file!
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
 *  $Id: rtems_ttcp.c,v 1.11 2010/02/12 17:38:30 joel Exp $
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <rtems/cpuuse.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>

/*
 * Glue between UNIX-style ttcp code and RTEMS
 */
int rtems_ttcp_main (int argc, char **argv);

static int
select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
  rtems_panic ("select()");
  return 0;
}

#define _SYS_RESOURCE_H_
#define	RUSAGE_SELF	0		/* calling process */
#define	RUSAGE_CHILDREN	-1		/* terminated child processes */
struct rusage {
	struct timeval ru_utime;	/* user time used */
	struct timeval ru_stime;	/* system time used */
	int ru_maxrss;		/* maximum resident set size */
	int ru_ixrss;		/* currently 0 */
	int ru_idrss;		/* integral resident set size */
	int ru_isrss;		/* currently 0 */
	int ru_minflt;		/* page faults not requiring physical I/O */
	int ru_majflt;		/* page faults requiring physical I/O */
	int ru_nswap;		/* swaps */
	int ru_inblock;		/* block input operations */
	int ru_oublock;		/* block output operations */
	int ru_msgsnd;		/* messages sent */
	int ru_msgrcv;		/* messages received */
	int ru_nsignals;	/* signals received */
	int ru_nvcsw;		/* voluntary context switches */
	int ru_nivcsw;		/* involuntary context switches */
};
int
getrusage(int ignored, struct rusage *ru)
{
	struct timeval now;
	static struct rusage nullUsage;

        rtems_clock_get_tod_timeval( &now );
	*ru = nullUsage;
	ru->ru_stime.tv_sec  = now.tv_sec;
	ru->ru_stime.tv_usec = now.tv_usec;
	ru->ru_utime.tv_sec  = 0;
	ru->ru_utime.tv_usec = 0;
	return 0;
}

static void
rtems_ttcp_exit (int code)
{
	rtems_cpu_usage_report();
	rtems_task_wake_after( RTEMS_MILLISECONDS_TO_TICKS(1000) );
	rtems_bsdnet_show_mbuf_stats ();
	rtems_bsdnet_show_if_stats ();
	rtems_bsdnet_show_ip_stats ();
	rtems_bsdnet_show_tcp_stats ();
	exit (code);
}

/*
 * Task to run UNIX ttcp command
 */
char *__progname;
static void
ttcpTask (rtems_task_argument arg)
{
	int code;
	int argc;
	char arg0[10];
	char *argv[20];
	char linebuf[200];

	for (;;) {
		char *cp;

		/*
		 * Set up first argument
		 */
		argc = 1;
		strcpy (arg0, "ttcp");
		argv[0] = __progname = arg0;

#if (defined (WRITE_TEST_ONLY))
		strcpy (linebuf, "-s -t crux");
#elif (defined (READ_TEST_ONLY))
		strcpy (linebuf, "-s -r");
#else
		/*
		 * Read a line
		 */
		printf (">>> %s ", argv[0]);
		fflush (stdout);
		fgets (linebuf, sizeof linebuf, stdin);
#endif

		/*
		 * Break line into arguments
		 */
		cp = linebuf;
		for (;;) {
			while (isspace (*cp))
				*cp++ = '\0';
			if (*cp == '\0')
				break;
			if (argc >= ((sizeof argv / sizeof argv[0]) - 1)) {
				printf ("Too many arguments.\n");
				argc = 0;
				break;
			}
			argv[argc++] = cp;
			while (!isspace (*cp)) {
				if (*cp == '\0')
					break;
				cp++;
			}
		}
		if (argc > 1) {
			argv[argc] = NULL;
			break;
		}
		printf ("You must give some arguments!\n");
		printf ("At the very least, you must provide\n");
		printf ("         -r\n");
		printf ("or\n");
		printf ("         -t destination.internet.address\n");
	}
	rtems_cpu_usage_reset();
	code = rtems_ttcp_main (argc, argv);
	rtems_ttcp_exit (code);
}

/*
 * Test network throughput
 */
void
test_network (void)
{
	rtems_id tid;
	rtems_status_code sc;
	rtems_time_of_day now;
	rtems_task_priority my_priority;

	/*
	 * Set up time-of-day clock
	 */
	now.year = 1997;
	now.month = 1;
	now.day = 1;
	now.hour = 0;
	now.minute = 0;
	now.second = 0;
	now.ticks = 0;
	sc = rtems_clock_set (&now);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't set date/time; %s\n", rtems_status_text (sc));
		return;
	}

	/*
	 * Spawn test task
	 */
	rtems_task_set_priority (RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &my_priority);
	sc = rtems_task_create (rtems_build_name ('T', 'T', 'C', 'P'),
			my_priority,
			32*1024,
			RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
			RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
			&tid);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't create task; %s\n", rtems_status_text (sc));
		return;
	}
	sc = rtems_task_start (tid, ttcpTask, 0);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't start task; %s\n", rtems_status_text (sc));
		return;
	}
	rtems_task_suspend (RTEMS_SELF);
}

#define main		rtems_ttcp_main
#define exit(code)	close(fd),rtems_ttcp_exit(code)

#include "ttcp_orig/ttcp.c"

/*
 * RTEMS configuration/initialization
 * 
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *	1. Leave this author information intact.
 *	2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id: init.c,v 1.7 2009/05/12 19:59:49 joel Exp $
 */

#include <bsp.h>

#include <stdio.h>
#include <stdlib.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include "../networkconfig.h"

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
	rtems_status_code sc;
	rtems_time_of_day now;
	rtems_interval ticksPerSecond;
	int rtems_bsdnet_synchronize_ntp (int interval, rtems_task_priority priority);

	printf ("****************** NTP TEST ***************\n");
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
	sc = rtems_clock_get (RTEMS_CLOCK_GET_TOD, &now);
	if (sc == RTEMS_SUCCESSFUL)
		printf ("Got time of day -- should have failed!\n");
	else if (sc != RTEMS_NOT_DEFINED)
		printf ("Failed to get time of day: %s\n", rtems_status_text (sc));
	rtems_bsdnet_initialize_network ();
	rtems_bsdnet_synchronize_ntp (0, 0);
	sc = rtems_clock_get (RTEMS_CLOCK_GET_TOD, &now);
	if (sc != RTEMS_SUCCESSFUL)
		printf ("Failed to get time of day: %s\n", rtems_status_text (sc));
	printf (
          "The time is **** %.4ld-%.2ld-%.2ld %.2ld:%.2ld:%.2ld.%.3ld (%ld) ****\n",
          (long) now.year,
          (long) now.month,
          (long) now.day,
          (long) now.hour,
          (long) now.minute,
          (long) now.second,
          (long) ((now.ticks * 1000) / ticksPerSecond),
          (long) now.ticks);
	exit (0);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES	20
#define CONFIGURE_MAXIMUM_TASKS		20

#define CONFIGURE_MICROSECONDS_PER_TICK	10000

#define CONFIGURE_INIT_TASK_STACK_SIZE	(10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY	120
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT
rtems_task Init (rtems_task_argument argument);

#include <rtems/confdefs.h>


/*
 * RTEMS configuration/initialization
 */
 
#include <bsp.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Network configuration
 */
#include "../networkconfig.h"

#ifdef SERVER
#define T(x)            __TXT(x)
#define __TXT(s)        #s
static char *av[] = {
	"RPC client",
	T(SERVER),
	"Test Message",
	NULL
};
#else
static char *av[] = {
	"RPC server",
	NULL
};
#endif
static int ac = (sizeof av / sizeof av[0]) - 1;

extern int rtems_main (int argc, char **argv);

rtems_task
Init (rtems_task_argument ignored)
{
	rtems_bsdnet_initialize_network ();
	rtems_bsdnet_synchronize_ntp (0, 0);

	rtems_main (ac, av);
	printf ("*** RPC Test Finish ***\n");
	exit (0);
}

/*
 * Dummy portmapper routines
 */
void pmap_set () { ; }
void pmap_unset () { ; }

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_STACK_CHECKER_ENABLED
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS   2
#define CONFIGURE_MAXIMUM_SEMAPHORES	    20
#define CONFIGURE_MAXIMUM_TASKS		    12

#define CONFIGURE_MICROSECONDS_PER_TICK       20000

#define CONFIGURE_INIT_TASK_PRIORITY	99
#define CONFIGURE_INIT_TASK_STACK_SIZE	(16*1024)
#define CONFIGURE_INIT

rtems_task Init(rtems_task_argument argument);

#include <rtems/confdefs.h>


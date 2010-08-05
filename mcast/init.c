/*
 * RTEMS configuration/initialization
 * 
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *    1. Leave this author information intact.
 *    2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id: init.c,v 1.2 2009/05/12 19:59:46 joel Exp $
 */

#include <bsp.h>

#include <stdio.h>
#include <stdlib.h>
#include <rtems/rtems_bsdnet.h>
#include "../networkconfig.h"

#include <rtems/untar.h>
#include "FilesystemImage.h"

#if 0
int add_mcast_route(
  char *multi_address,
  char *multi_netmask,
  char *multi_gateway
);

#define MULTI_ADDRESS        "239.9.8.0"
#define MULTI_NETMASK        "255.255.255.0"
#define MULTI_GATEWAY       "192.168.1.244"
#endif

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
  int mcast_main(int ac, char **av);
  rtems_status_code status;

  printf("Loading filesystem image\n");
  status = Untar_FromMemory( (char *)FilesystemImage, FilesystemImage_size );
   
  printk( "Initializing network\n" );
  rtems_bsdnet_initialize_network ();

  // This appears to have no effect.
  // add_mcast_route( MULTI_ADDRESS, MULTI_NETMASK, MULTI_GATEWAY );

  printk( "Network initialized\n" );
  rtems_bsdnet_show_inet_routes ();

  printk( "Initiating mcast test\n" );
  mcast_main ( 0, 0 );
  exit (0);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_EXECUTIVE_RAM_SIZE (512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES 20
#define CONFIGURE_MAXIMUM_TASKS      20

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_INIT_TASK_STACK_SIZE (10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY   120
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT
rtems_task Init (rtems_task_argument argument);

#include <rtems/confdefs.h>


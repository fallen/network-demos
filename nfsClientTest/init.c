/*  Init
 *
 *  This routine is the initialization task for this test program.
 *
 *  Don't forget to change the IP addresses
 */

#include <bsp.h>

#include <errno.h>
#include <time.h>

#include <stdio.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/untar.h>

     
#include <rtems/error.h>
#include <rpc/rpc.h>
#include <netinet/in.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include "../networkconfig.h"


/*
 *  The tarfile image is built automatically externally.
 */
#include "FilesystemImage.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  int int_status;

  printf("\n\n*** NFS Client TEST ***\n\r" );

  printf( "Free space %d\n", malloc_free_space() );
  /*
   * Load filesystem image
   */
  printf("=============== Loading filesystem image ===============\n");
  status = Untar_FromMemory((void *)FilesystemImage, FilesystemImage_size); 
   
  printf("============== Look at Local Filesystem ==============\n");
  printf( "PWD: " );
  pwd(); 
  
  printf( "\n--->ls /\n" );
  ls("/");

  printf( "\n--->ls /etc\n" );
  ls("/etc");

  printf("============== Initializing Network ==============\n");
  rtems_bsdnet_initialize_network ();

  printf("============== Add Route ==============\n");
  rtems_bsdnet_show_inet_routes ();

  printf("============== Initializing RPC ==============\n");
  int_status = rpcUdpInit();
  if ( int_status )
    printf( "RPC UDP Initialization failed\n" );

  printf("============== Initializing NFS Subsystem ==============\n");
  nfsInit( 0, 0 );

  printf("============== Mounting Remote Filesystem ==============\n");
#if 1
  /* This code uses the NFS mount wrapper function */
  int_status = nfsMount( RTEMS_NFS_SERVER, RTEMS_NFS_SERVER_PATH, "/mnt" );
#else
  /* This section does it more explicitly */
  mkdir( "/mnt", 0777 );
  int_status = mount(
    NULL,                        /* mount_table_entry_pointer */
    &nfs_fs_ops,                 /* filesystem_operations_table_pointer */
    RTEMS_FILESYSTEM_READ_WRITE, /* options */
                                 /* device aka remote filesystem */
    RTEMS_NFS_SERVER ":" RTEMS_NFS_SERVER_PATH,
    "/mnt"                       /* mount_point */
  );
#endif
  if ( int_status )
    printf( "NFS Mount failed -- %s\n", strerror(errno) );

  printf("============== Look at Remote Filesystem ==============\n");
  printf( "\n---> ls /mnt\n" );
  ls( "/mnt" );
  printf( "\n---> ls %s\n", RTEMS_NFS_LS_PATH );
  ls( RTEMS_NFS_LS_PATH );

  exit(0);

  status = rtems_task_delete( RTEMS_SELF );
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS	20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MEMORY_OVERHEAD       256
#define CONFIGURE_MESSAGE_BUFFER_MEMORY 32 * 1024
#define CONFIGURE_MAXIMUM_SEMAPHORES	40
#define CONFIGURE_MAXIMUM_TASKS		20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES	20

#define CONFIGURE_MICROSECONDS_PER_TICK	1000

#define CONFIGURE_INIT_TASK_STACK_SIZE	(64*1024)
#define CONFIGURE_INIT_TASK_PRIORITY	120
#define CONFIGURE_INIT_TASK_ATTRIBUTES    RTEMS_FLOATING_POINT
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_MAXIMUM_DRIVERS 10
#define CONFIGURE_INIT

#include <rtems.h>
#include <librtemsNfs.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

/* configuration information */

#include <rtems/confdefs.h>

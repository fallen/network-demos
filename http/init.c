/*  Init
 *
 *  This routine is the initialization task for this test program.
 *
 *  Don't forget to change the IP addresses
 */

#include "system.h"
#include <bsp.h>

#include <errno.h>
#include <time.h>

#include <stdio.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/ftpd.h>
#include <rtems/untar.h>

     
#include <rtems/error.h>
#include <rpc/rpc.h>
#include <netinet/in.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include "../networkconfig.h"

#define ARGUMENT 0

/*
 *  The tarfile image is built automatically externally.
 */
#include "FilesystemImage.h"

#if defined(USE_FTPD)
  bool FTPD_enabled = true;
  struct rtems_ftpd_configuration rtems_ftpd_configuration = {
    10,                     /* FTPD task priority            */
    1024,                   /* Maximum buffersize for hooks  */
    21,                     /* Well-known port     */
    NULL,                   /* List of hooks       */
    NULL,                   /* Root for FTPD or 0 for / */
    0,                      /* Max. connections    */
    0,                      /* Idle timeout in seoconds
                               or 0 for no (inf) timeout */
    0,                      /* 0 - r/w, 1 - read-only,
                               2 - write-only,
                               3 - browse-only */
 };

#else
 bool FTPD_enabled = false;
#endif

#if defined(USE_GOAHEAD_HTTPD) && !defined(RTEMS_POSIX_API)
  #warning "GoAhead server requires POSIX API - switching to SHTTPD"
  #undef USE_GOAHEAD_HTTPD
  #undef USE_SIMPLE_HTTPD
#endif

#if defined(USE_GOAHEAD_HTTPD)
  bool GoAhead_HTTPD_enabled = true;

  /* GoAhead Trace Handler */
  #include <goahead/uemf.h>
  void quietTraceHandler(int level, char *buf)
  {
    /* do nothing */
  }
#else
  bool GoAhead_HTTPD_enabled = false;
#endif

#if defined(USE_SIMPLE_HTTPD)
  bool Simple_HTTPD_enabled = true;

  #include <shttpd/shttpd.h>
#else
  bool Simple_HTTPD_enabled = false;
#endif

#if defined(USE_MONGOOSE_HTTPD)
  #include <mghttpd/mongoose.h>

  void example_mongoose_addpages(
    struct mg_context *server
  );
#endif

#define bool2string(_b) ((_b) ? "true" : "false")

#if defined(USE_SIMPLE_HTTPD)
extern void example_shttpd_addpages(struct shttpd_ctx *ctx);
#endif

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  printf("\n\n*** HTTP TEST ***\n\r" );
  printf("GoAhead HTTPD Enabled: %s\n", bool2string(GoAhead_HTTPD_enabled) );
  printf("Simple HTTPD Enabled: %s\n", bool2string(Simple_HTTPD_enabled) );
  printf("FTPD Enabled: %s\n", bool2string(FTPD_enabled) );
  printf("\n");

  /*
   * Load filesystem image
   */
  printf("Loading filesystem image\n");
  status = Untar_FromMemory( (char *)FilesystemImage, FilesystemImage_size );
   
  printf("Initializing Network\n");
  rtems_bsdnet_initialize_network ();

  #if defined(USE_FTPD)
    printf( "Initializing FTPD\n" );
    rtems_initialize_ftpd();
  #endif

  #if defined(USE_GOAHEAD_HTTPD)
    printf( "Initializing GoAhead HTTPD\n" );
    status = rtems_initialize_webserver();
    if ( status )
      printf( "ERROR -- failed to initialize webserver\n" );

    traceSetHandler( quietTraceHandler );
  #endif

  #if defined(USE_SIMPLE_HTTPD)
    /*
     *  SHTTPD uses about 37K of stack even in this test on a PowerPC.
     *  There is no point in doing math for the stack size.  Bump it
     *  until there isn't a problem.
     */
    printf( "Initializing Simple HTTPD\n" );
    status = rtems_initialize_webserver(
      100,                             /* initial priority */
      (48 * 1024),                     /* stack size */
      RTEMS_DEFAULT_MODES,             /* initial modes */
      RTEMS_DEFAULT_ATTRIBUTES,        /* attributes */
      NULL,                            /* init_callback */
      example_shttpd_addpages,         /* addpages_callback */
      "/",                             /* initial priority */
      80                               /* port to listen on */
    );
    if ( status )
      printf( "ERROR -- failed to initialize webserver\n" );

  #endif

  #if defined(USE_MONGOOSE_HTTPD)
    {
      struct mg_context *WebServer;
      printf( "Initializing Mongoose HTTPD\n" );
      WebServer = mg_start();

      mg_set_option(WebServer, "root", "/" );
      mg_set_option(WebServer, "ports", "80" );
      example_mongoose_addpages( WebServer );

    }
  #endif
  status = rtems_task_delete( RTEMS_SELF );
}


/*
 *  Configuration
 */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS	20
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES	20
#define CONFIGURE_MAXIMUM_TASKS		20

#if defined(USE_MONGOOSE_HTTPD)
#define CONFIGURE_MAXIMUM_POSIX_THREADS 10
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES 30
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES 10
#define CONFIGURE_MINIMUM_TASK_STACK_SIZE (32 * 1024)
#define CONFIGURE_UNIFIED_WORK_AREAS
#endif

#define CONFIGURE_MICROSECONDS_PER_TICK	10000

#define CONFIGURE_INIT_TASK_STACK_SIZE	(10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY	120
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_STACK_CHECKER_ENABLED
#define CONFIGURE_INIT

#include <rtems/confdefs.h>


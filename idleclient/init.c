#include <bsp.h>

#include <stdio.h>
#include <stdlib.h>
#include <rtems/rtems_bsdnet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> // sleep()
#include "../networkconfig.h"

#define MM_MINIMAC_STATE0  (0xe0009008)
#define MM_MINIMAC_COUNT0  (0xe0009010)
#define MM_MINIMAC_STATE1  (0xe0009014)
#define MM_MINIMAC_COUNT1  (0xe000901C)
#define MM_MINIMAC_STATE2  (0xe0009020)
#define MM_MINIMAC_COUNT2  (0xe0009028)
#define MM_MINIMAC_STATE3  (0xe000902C)
#define MM_MINIMAC_COUNT3  (0xe0009034)

#define NB_RX_SLOTS 4

static uint32_t rx_slot_state[4] = {MM_MINIMAC_STATE0, MM_MINIMAC_STATE1,MM_MINIMAC_STATE2, MM_MINIMAC_STATE3};

static uint32_t rx_slot_count[4] = {MM_MINIMAC_COUNT0, MM_MINIMAC_COUNT1,MM_MINIMAC_COUNT2, MM_MINIMAC_COUNT3};

rtems_task Init (rtems_task_argument ignored)
{
  printf("idleclient test program\n");

  printf("== initializaing network ==\n");
  rtems_bsdnet_initialize_network();
  printf("== Route ==\n");
  rtems_bsdnet_show_inet_routes();
  
  while (1) {
    int i;
    sleep(1);
    for (i = 0 ; i < NB_RX_SLOTS ; i++)
      printf("slot n° %d => state = %d ; count = %d\n", i, *((unsigned int *)rx_slot_state[i]), *((unsigned int *)rx_slot_count[i]));
    printf("\n\n");
  }

  printf("\n\n== tftpclient test program END ==\n\n");

  exit(0);
}


#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_EXECUTIVE_RAM_SIZE        (512*1024)
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 50
#define CONFIGURE_MAXIMUM_SEMAPHORES        20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES    20
#define CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                          RTEMS_NO_TIMESLICE | \
                                          RTEMS_NO_ASR | \
                                          RTEMS_INTERRUPT_LEVEL(0))
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_PRIORITY    50
#define CONFIGURE_INIT_TASK_STACK_SIZE    (10*1024)
#define CONFIGURE_MAXIMUM_TASKS 20
#define CONFIGURE_INIT
rtems_task Init (rtems_task_argument argument);

#include <rtems/confdefs.h>

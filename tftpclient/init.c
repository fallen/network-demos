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
#include <fcntl.h> // fcntl()
#include "../networkconfig.h"

// tftp opcodes
#define OP_READ   1
#define OP_WRITE  2
#define OP_DATA   3
#define OP_ACK    4
#define OP_ERROR  5

#define MAX_DATA_SIZE 512
#define OPCODE_SIZE sizeof(uint16_t)
#define BLOCKNUM_SIZE sizeof(uint16_t)
#define END_OF_STRING_SIZE  sizeof(uint8_t)

#define TFTP_PORT 69

// Who is the tftp server ?

#define TFTP_SERVER "192.168.101.254"

// What file to read ?

#define TFTP_FILENAME 	"sample.data"

#define FILE_ASCII	"netascii"
#define FILE_BINARY	"octet"


// put FILE_ASCII for ascii file
// put FILE_BINARY for binary file
#define TFTP_FILE_TYPE	FILE_ASCII

struct tftp_packet {
  uint16_t opcode;
  uint8_t data[MAX_DATA_SIZE + BLOCKNUM_SIZE];
} __attribute__((packed, aligned(4)));

static void tftp_read(int sock, struct sockaddr_in *farAddr) {
  struct tftp_packet packet;
  int ret;
  ssize_t datasize, size;
  int i;
  packet.opcode = OP_READ;
  strncpy(packet.data, TFTP_FILENAME, strlen(TFTP_FILENAME) + 1);
  strncpy(packet.data + strlen(TFTP_FILENAME) + END_OF_STRING_SIZE, TFTP_FILE_TYPE, strlen(TFTP_FILE_TYPE) + 1);
  
  datasize = strlen(TFTP_FILENAME) + 2*END_OF_STRING_SIZE + strlen(FILE_ASCII);
  size = datasize + sizeof(packet.opcode);

  printf("size = %d\n", size);
  printf("\n");
  for (i = 0; i < size ; i++)
    printf("0x%02X ", *((unsigned char *)&packet + i ) );

  printf("\n");
  
  printf("strlen(TFTP_FILENAME) = %d ; strlen(\"%s\") = %d\n", strlen(TFTP_FILENAME), TFTP_FILE_TYPE, strlen(TFTP_FILE_TYPE));
  printf("END_OF_STRING_SIZE = %d", END_OF_STRING_SIZE);
  
  ret = sendto(sock, &packet, size, 0, (struct sockaddr *)farAddr, sizeof(struct sockaddr_in)); 
  if (ret == -1)
    perror("sendto:");
  assert(ret != -1);
}

static void tftp_ack(int sock, struct sockaddr_in *farAddr, uint16_t blocknum) {
  struct tftp_packet packet;
  int ret;
//printf("Nous sommes dans la fonction d'ack !\n");
//printf("sock = %d, &farAddr = 0x%08x, blocknum = %d\n", sock, farAddr, blocknum);
  packet.opcode = OP_ACK;
  packet.data[0] = (uint8_t)(blocknum >> 8);
  packet.data[1] = (uint8_t)blocknum;
  
  ret = sendto(sock, &packet, OPCODE_SIZE + BLOCKNUM_SIZE, 0, (struct sockaddr *)farAddr, sizeof(struct sockaddr_in));
  if (ret == -1)
    perror("sendto:");
  assert(ret != -1);
//printf("La fonction d'ack a fini, avec ret = %d\n", ret);

}

rtems_task Init (rtems_task_argument ignored)
{
  int sock, ret;
  ssize_t size, datasize;
  socklen_t addr_len;
  struct sockaddr_in farAddr, addr;
  struct tftp_packet packet;
  printf("tftpclient test program\n");

  printf("== initializaing network ==\n");
  rtems_bsdnet_initialize_network();
  printf("== Route ==\n");
  rtems_bsdnet_show_inet_routes();

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1)
      perror("socket:");
    assert(sock != -1);

  memset(&farAddr, 0, sizeof farAddr);
  farAddr.sin_family = AF_INET;
  farAddr.sin_port = htons(TFTP_PORT);
  farAddr.sin_addr.s_addr = htonl(inet_addr(TFTP_SERVER));

  printf("Sending the READ request...\n");
  tftp_read(sock, &farAddr);
  //sleep(2);
  
  printf("Content of the file : \n\n");
  
  do {
    size = recvfrom(sock, &packet, MAX_DATA_SIZE + OPCODE_SIZE + BLOCKNUM_SIZE, 0, (struct sockaddr *)&addr, &addr_len);
    switch (packet.opcode) {
      case OP_DATA:
                    if ( *((uint16_t *)(packet.data)) == 1 ) // first data packet, we store the remote port
                      farAddr.sin_port = htons(addr.sin_port);
//                  printf("== On ACK le packet ==\n");
                    tftp_ack(sock, &farAddr, *((uint16_t *)(packet.data)));
//                  printf("== On a ACK le packet ==\n");
                    datasize = size - OPCODE_SIZE - BLOCKNUM_SIZE;
                    packet.data[datasize + BLOCKNUM_SIZE] = '\0';
                    printf("%s", packet.data + OPCODE_SIZE);
                    break;
      default:
                  datasize = 0;
                  printf("ERROR, this opcode was not expected !\n");
    }
    size = 0;
  } while (datasize == MAX_DATA_SIZE);
  
  printf("\n\n== tftpclient test program END ==\n\n");


  // we do not ACK the last tftp packet without this !
  // FIXME: how do i flush the kernel buffers within BSD network stack ?
  while (1) {
    sleep(5);
  }
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

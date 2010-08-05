/*
 * Network configuration -- LOOPBACK ONLY!!!
 *
 * See one of the other networkconfig.h files for an
 * example of a system that includes a real NIC and
 * the loopback interface.
 * 
 ************************************************************
 * EDIT THIS FILE TO REFLECT YOUR NETWORK CONFIGURATION     *
 * BEFORE RUNNING ANY RTEMS PROGRAMS WHICH USE THE NETWORK! * 
 ************************************************************
 *
 *  $Id: networkconfig.h,v 1.16 2008/09/18 13:33:30 joel Exp $
 */

#ifndef _RTEMS_NETWORKCONFIG_H_
#define _RTEMS_NETWORKCONFIG_H_


/* #define RTEMS_USE_BOOTP */

#include <bsp.h>

/*
 * Loopback interface
 */
extern int rtems_ne_driver_attach();

/*
 * Default network interface
 */
static struct rtems_bsdnet_ifconfig netdriver_config = {
  "minimac0",                    /* name */
  rtems_minimac_driver_attach,  /* attach function */
  NULL,                     /* No more interfaces */
  "192.168.101.100",              /* IP address */
  "255.255.255.0",              /* IP net mask */
  NULL,                     /* Driver supplies hardware address */
  0,                        /* Use default driver parameters */
  0,                        /* default efficiency multiplier */
  0,                        /* default udp TX socket buffer size */
  0,                        /* default udp RX socket buffer size */
  0,                        /* default tcp TX socket buffer size */
  0,                        /* default tcp RX socket buffer size */
};

/*
 * Network configuration
 */
struct rtems_bsdnet_config rtems_bsdnet_config = {
  &netdriver_config,
  NULL,                /* do not use bootp */
  0,                   /* Default network task priority */
  256 * 1024,                   /* Default mbuf capacity */
  256 * 1024,                   /* Default mbuf cluster capacity */
  "rtems",             /* Host name */
  "nodomain.com",      /* Domain name */
  "192.168.101.254",         /* Gateway */
  "127.0.0.1",         /* Log host */
  {"127.0.0.1" },      /* Name server(s) */
  {"127.0.0.1" },      /* NTP server(s) */
  0,                   /* sb_efficiency */
  0,                   /* udp_tx_buf_size */
  0,                   /* udp_rx_buf_size */
  0,                   /* tcp_tx_buf_size */
  0                    /* tcp_rx_buf_size */

};

/*
 * For TFTP test application
 */
#if (defined (RTEMS_USE_BOOTP))
#define RTEMS_TFTP_TEST_HOST_NAME "BOOTP_HOST"
#define RTEMS_TFTP_TEST_FILE_NAME "BOOTP_FILE"
#else
#define RTEMS_TFTP_TEST_HOST_NAME "192.168.101.254"
#define RTEMS_TFTP_TEST_FILE_NAME "README"
#endif

/*
 * For NFS test application
 * 
 * NFS server/path to mount and a directory to ls once mounted
 */
#define RTEMS_NFS_SERVER      "192.168.101.254"
#define RTEMS_NFS_SERVER_PATH "/home"
#define RTEMS_NFS_LS_PATH     "/mnt/nfstest"
#endif /* _RTEMS_NETWORKCONFIG_H_ */

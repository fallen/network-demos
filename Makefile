#
#  Makefile,v 1.1 2002/11/18 14:13:30 joel Exp
#

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc

SUBDIRS= netdemo http dnstest ntp select tftpTest ttcp mcast telnetd

# This requires something that is optional and we need to test for it
#SUBDIRS+= rpc_demo

ifeq ($(RTEMS_HAS_POSIX_API),yes)
SUBDIRS += 
endif

include $(RTEMS_CUSTOM)
include $(RTEMS_ROOT)/make/directory.cfg


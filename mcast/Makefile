#
#  $Id: Makefile,v 1.3 2009/05/13 18:54:49 joel Exp $
#

PGM=${ARCH}/mcast.exe

MANAGERS=all

C_FILES=init.c mcast.c mcast_route.c FilesystemImage.c
C_O_FILES=$(C_FILES:%.c=${ARCH}/%.o)

H_FILES=

SRCS=$(DOCS) $(C_FILES) $(H_FILES)
OBJS=$(C_O_FILES)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

#
# (OPTIONAL) Add local stuff here using +=
#

override DEFINES  +=
CPPFLAGS +=
CFLAGS   +=
CFLAGS_LD += 
# CFLAGS_LD += -Wl,--defsym -Wl,HeapSize=0xC0000 
CFLAGS_OPTIMIZE_V   +=
## CFLAGS_DEBUG_V   += -v -qrtems_debug

LD_PATHS  +=
LD_LIBS   +=

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += FilesystemImage FilesystemImage.c FilesystemImage.h
CLEAN_ADDITIONS += listener mcast
CLOBBER_ADDITIONS +=

all:	$(ARCH) $(SRCS) $(OBJS) $(PGM) listener mcast

listener: listener.c mcast_params.h
	gcc -o $@ $<

mcast: mcast.c mcast_params.h
	gcc -o $@ $<

$(PGM): $(ARCH) $(OBJS) $(LINK_FILES)

$(ARCH)/init.o: $(ARCH) init.c FilesystemImage.c

$(ARCH)/mcast_route.o: $(ARCH) mcast_route.c

$(ARCH)/mcast.o: $(ARCH) mcast.c mcast_params.h

$(ARCH)/FilesystemImage.o: $(ARCH) FilesystemImage.c

FilesystemImage: rootfs/etc/host.conf rootfs/etc/hosts
	cd rootfs ; tar cf ../FilesystemImage --exclude CVS --exclude .cvsignore .

FilesystemImage.c: FilesystemImage
	$(PROJECT_ROOT)/bin/rtems-bin2c FilesystemImage FilesystemImage

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL)
install:  all
#	$(INSTALL_VARIANT) -m 555 ${PGM} ${PROJECT_RELEASE}/tests

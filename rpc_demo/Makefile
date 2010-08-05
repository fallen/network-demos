#
# Makefile for RTEMS RPC server or client
#
# To build an RTEMS RPC server
#	make
# To build an RTEMS RPC client
#	`make SERVER=hostname'
# where `hostname' is the name of the RPC server
#
SAMPLE=rpc
PGM=${ARCH}/$(SAMPLE).exe

MANAGERS=io event semaphore timer

# C source names, if any, go here -- minus the .c
ifdef SERVER
C_PIECES= init rprintmsg msg_clnt
else
C_PIECES= init msg_main msg_proc
endif
C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=${ARCH}/%.o)

H_FILES=

DOCTYPES=
DOCS=$(DOCTYPES:%=$(SAMPLE).%)

SRCS=$(DOCS) $(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS=$(C_O_FILES) $(CC_O_FILES) $(S_O_FILES)

PRINT_SRCS=$(DOCS)

PGM=${ARCH}/$(SAMPLE).exe

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

#
# (OPTIONAL) Add local stuff here using +=
#

override DEFINES  += 
ifdef SERVER
DEFINES  += "-DSERVER=$(SERVER)"
endif
DEFINES  += -Dmain=rtems_main
#CPPFLAGS += -I../ftp.FreeBSD.org/pub/FreeBSD/branches/-current/src/include
#CPPFLAGS += -I../ftp.FreeBSD.org/pub/FreeBSD/branches/-current/src/sys
CFLAGS   +=
CFLAGS_OPTIMIZE_V   +=

LD_PATHS  +=
#LD_LIBS   += ../ftp.FreeBSD.org/pub/FreeBSD/branches/-current/src/lib/libc/rpc/o-optimize/librpc.a
#LD_LIBS   += ../ftp.FreeBSD.org/pub/FreeBSD/branches/-current/src/lib/libc/xdr/o-optimize/libxdr.a

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS +=
CLOBBER_ADDITIONS +=

all:	${ARCH} $(SRCS) $(PGM)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

foo:
	echo $(RTEMS_BSP)
# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL)
install:  all
	$(INSTALL_VARIANT) -m 555 ${PGM} ${PROJECT_RELEASE}/tests

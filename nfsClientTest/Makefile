#
#  $Id: Makefile,v 1.4 2009/05/13 18:54:55 joel Exp $
#

SAMPLE=nfsClientTest
PGM=${ARCH}/$(SAMPLE).exe

MANAGERS=all

# C source names, if any, go here -- minus the .c
C_PIECES=init dirutils FilesystemImage
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
CPPFLAGS +=
CFLAGS_LD += # -Wl,--defsym -Wl,HeapSize=0xC0000 
CFLAGS_OPTIMIZE_V   +=
CFLAGS_DEBUG_V   += -v -qrtems_debug

LD_PATHS  +=

CFLAGS   += 
LD_LIBS  += -lnfs

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += FilesystemImage FilesystemImage.c FilesystemImage.h
CLOBBER_ADDITIONS +=

all:	${ARCH} $(SRCS) $(PGM)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

$(ARCH)/init.o: init.c FilesystemImage.h

FilesystemImage: $(ARCH) rootfs/etc/host.conf rootfs/etc/hosts rootfs/rtems_logo.jpg
	cd rootfs ; \
	    tar cf ../FilesystemImage --exclude CVS --exclude .cvsignore .

FilesystemImage.c: $(ARCH) FilesystemImage
	$(PROJECT_ROOT)/bin/rtems-bin2c FilesystemImage FilesystemImage


# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL)
install:  all
	$(INSTALL_VARIANT) -m 555 ${PGM} ${PROJECT_RELEASE}/tests

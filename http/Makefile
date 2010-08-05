#
#  $Id: Makefile,v 1.20 2009/11/23 14:31:52 joel Exp $
#

SAMPLE=http
PGM=${ARCH}/$(SAMPLE).exe

MANAGERS=all

# C source names, if any, go here -- minus the .c
C_PIECES=init FilesystemImage shttpd_ext mongoose_ext
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
CFLAGS_LD += -Wl,--defsym -Wl,HeapSize=0xC0000 
CFLAGS_OPTIMIZE_V   +=
CFLAGS_DEBUG_V   += -v -qrtems_debug

LD_PATHS  +=

## FYI GoAhead requires POSIX to be enabled
USE_GOAHEAD=no
USE_SIMPLE=no
USE_MONGOOSE=yes
USE_FTPD=yes
ifeq ($(USE_GOAHEAD),yes)
  HTTPD     = GoAhead Web Server
  CFLAGS   += -DWEBS -DUEMF -DUSE_GOAHEAD_HTTPD
  override LD_LIBS  += -lhttpd
endif

ifeq ($(USE_SIMPLE),yes)
  HTTPD     = Simple HTTPD Web Server
  CFLAGS   += -DUSE_SIMPLE_HTTPD
  override LD_LIBS  += -lshttpd
endif

ifeq ($(USE_MONGOOSE),yes)
  HTTPD     = Mongoose HTTPD Web Server
  CFLAGS   += -DUSE_MONGOOSE_HTTPD
  override LD_LIBS  += -lmghttpd
endif

ifeq ($(USE_FTPD),yes)
  CFLAGS   += -DUSE_FTPD
  override LD_LIBS  += -lftpd
endif

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'


CLEAN_ADDITIONS += FilesystemImage FilesystemImage.c FilesystemImage.h
CLEAN_ADDITIONS += stamp-index-html rootfs/index.html rootfs/shttpd.png
CLOBBER_ADDITIONS +=

all:	${ARCH} $(SRCS) $(PGM)

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

$(ARCH)/init.o: init.c FilesystemImage.c

FilesystemImage: $(ARCH) stamp-index-html \
    rootfs/etc/host.conf rootfs/etc/hosts rootfs/rtems_logo.jpg
	cd rootfs ; \
	    tar cf ../FilesystemImage --exclude CVS --exclude .cvsignore .

FilesystemImage.c: $(ARCH) FilesystemImage
	$(PROJECT_ROOT)/bin/rtems-bin2c FilesystemImage FilesystemImage

rootfs/index.html: shttpd_index.html goahead_index.html mongoose_index.html
ifeq ($(USE_SIMPLE),yes)
	cp shttpd_index.html rootfs/index.html
endif
ifeq ($(USE_GOHEAD),yes)
	cp goahead_index.html rootfs/index.html
endif
ifeq ($(USE_MONGOOSE),yes)
	cp mongoose_index.html rootfs/index.html
endif

stamp-index-html: rootfs/index.html
ifeq ($(USE_SIMPLE),yes)
	cp shttpd.png rootfs
endif
	touch stamp-index-html

# Install the program(s), appending _g or _p as appropriate.
# for include files, just use $(INSTALL)
install:  all
	$(INSTALL_VARIANT) -m 555 ${PGM} ${PROJECT_RELEASE}/tests


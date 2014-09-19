
if WANT_JANSSON
JANSSON_INCLUDES= -I$(top_srcdir)/compat/jansson
else
JANSSON_INCLUDES=
endif

EXTRA_DIST	= example-cfg.json nomacro.pl

SUBDIRS		= compat

INCLUDES	= $(PTHREAD_FLAGS) -fno-strict-aliasing $(JANSSON_INCLUDES)

bin_PROGRAMS	= minerd

dist_man_MANS	= minerd.1

minerd_SOURCES	= elist.h \
		  miner.h \
		  compat.h \
		  sha2.c \
		  cpu-miner.c \
		  util.c \
		  boulderhash.c \
		  crypto/c_keccak.c \
		  crypto/hash-ops.c \
		  crypto/boulderhash.c \
		  crypto/threadpool.c

minerd_LDFLAGS	= $(PTHREAD_FLAGS)
minerd_LDADD	= @LIBCURL@ @JANSSON_LIBS@ @PTHREAD_LIBS@ @WS2_LIBS@
minerd_CPPFLAGS = @LIBCURL_CPPFLAGS@
minerd_CFLAGS   = -Ofast -flto -fuse-linker-plugin

if HAVE_WINDOWS
minerd_CFLAGS += -Wl,--stack,10485760
endif

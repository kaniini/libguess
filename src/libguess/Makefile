LIB = ${LIB_PREFIX}guess${LIB_SUFFIX}
LIB_MAJOR = 1
LIB_MINOR = 0
DISTCLEAN = autoconf.h

SRCS = guess.c		\
       guess_impl.c	\
       dfa.c

INCLUDES = libguess.h

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${LIB_CPPFLAGS} -I. -I.. -DLIBGUESS_CORE
CFLAGS += ${LIB_CFLAGS}

PROG = guessconv${PROG_SUFFIX}
SRCS = guessconv.c

include ../../../buildsys.mk
include ../../../extra.mk

CPPFLAGS += ${MOWGLI_CFLAGS} -I../../libguess
LIBS += ${MOWGLI_LIBS} -L../../libguess -lguess

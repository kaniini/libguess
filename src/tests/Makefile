PROG_NOINST = guess_testharness${PROG_SUFFIX}
SRCS = harness.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${MOWGLI_CFLAGS} -I../libguess
LIBS += ${MOWGLI_LIBS} -L../libguess -lguess

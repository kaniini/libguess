.SILENT:
.SUFFIXES:
.SUFFIXES: .gen .run .test .out
.PHONY: all run generate

all: run

run:
	for i in */*.test ; do \
		${MAKE} ${MFLAGS} $${i%.test}.run || exit 1; \
	done

generate:
	for i in */*.test ; do \
		${MAKE} ${MFLAGS} $${i%.test}.gen || exit 1; \
	done

.test.gen:
	t=$<; \
	l=$${t%/*}; \
	../guess_testharness $< | grep $$l > ${<:.test=.out}

.test.run:
	t=$<; \
	l=$${t%/*}; \
	r=$$(../guess_testharness $< | grep $$l); \
	o=$$(cat ${<:.test=.out}); \
	if [ "$$r" = "$$o" ] ; then \
		echo Pass: $${t%.test}; \
	else \
		echo Fail: $${t%.test}; \
		false; \
	fi

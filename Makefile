MAKEFLAGS += -r

.PHONY: all compile test-compile test coverage manpage clean

CC ?= cc
CFLAGS ?= -Wall -Wextra -Wconversion -Wno-varargs -Og -g -flto
CFLAGS += -std=c99
LDFLAGS ?= -flto

all: compile

clean:
	@rm -rf -v target

%/:
	@mkdir -p $@

target/object_files/%.c.o: src/%.c Makefile | target/object_files/ target/dependencies/
	$(CC) $(CPPFLAGS) $(CFLAGS) -fpic -MMD -MP -MF target/dependencies/$*.c.mk -c -o $@ $<

target/lib/libprefam.so: target/object_files/preload.c.o target/object_files/record.c.o target/object_files/util.c.o Makefile | target/lib/
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fpic -o $@ $(filter %.o,$^)

target/test/test_derelativize: target/object_files/test_derelativize.c.o target/object_files/util.c.o Makefile | target/test/
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.o,$^)

target/manpages/prefam.1: src/prefam.1.adoc | target/manpages/
	asciidoctor -D target/manpages $<

manpage: target/manpages/prefam.1

compile: target/lib/libprefam.so

test-compile: compile target/test/test_derelativize

test: export libprefam = target/lib/libprefam.so
test: export test_derelativize = target/test/test_derelativize
test: test-compile
	@./test.sh

coverage: CFLAGS += --coverage -fno-default-inline
coverage: LDFLAGS += --coverage
coverage: test | target/coverage/
	@lcov --output-file target/coverage/coverage.info --directory target --capture --exclude '/usr/include/*'
	@genhtml -o target/coverage target/coverage/coverage.info

-include target/dependencies/*.mk

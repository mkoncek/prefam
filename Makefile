MAKEFLAGS += -r

soversion := 1

.PHONY: all compile test-compile test coverage manpages clean
.PRECIOUS: target/object_files/%.c.o

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

target/lib/libprefam.so.$(soversion): target/object_files/preload.c.o target/object_files/record.c.o target/object_files/util.c.o Makefile | target/lib/
	$(CC) $(CFLAGS) -o $@ $(filter %.o,$^) -shared -Wl,-soname,$(@F) $(LDFLAGS) $(LDLIBS)

target/test/%: target/object_files/%.c.o Makefile | target/test/
	$(CC) $(CFLAGS) -o $@ $(filter %.o,$^) $(LDFLAGS) $(LDLIBS)

target/test/test_derelativize: target/object_files/util.c.o
target/test/test_open: target/object_files/preload.c.o target/object_files/record.c.o target/object_files/util.c.o
target/test/test_exec: target/object_files/preload.c.o target/object_files/record.c.o target/object_files/util.c.o

target/manpages/%: src/%.adoc | target/manpages/
	asciidoctor -D target/manpages $<

manpages: target/manpages/prefam.1 target/manpages/prefam.3

compile: target/lib/libprefam.so.$(soversion)

test-compile: compile target/test/test_derelativize target/test/test_open target/test/test_exec

test: export TARGET_LIB = target/lib/libprefam.so.$(soversion)
test: export TARGET_TEST_BIN_DIR = target/test
test: test-compile
	@./test.sh

coverage: CFLAGS += --coverage -fno-default-inline
coverage: LDFLAGS += --coverage
coverage: test | target/coverage/
	@lcov --output-file target/coverage/coverage.info --directory target --capture --exclude '/usr/include/*'
	@genhtml -o target/coverage target/coverage/coverage.info

-include target/dependencies/*.mk

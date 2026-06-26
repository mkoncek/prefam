.PHONY: all compile test coverage manpage clean

all: compile

CC ?= cc
CFLAGS ?= -Wall -Wextra -Wconversion -Wno-varargs -Og -g
CFLAGS += -std=c99
LDFLAGS ?=

clean:
	@rm -rf -v target

%/:
	@mkdir -p $@

target/object_files/%.c.o: src/%.c Makefile | target/object_files/ target/dependencies/
	$(CC) $(CPPFLAGS) $(CFLAGS) -fpic -MMD -MP -MF target/dependencies/$*.c.mk -c -o $@ $<

target/libprefam.so: target/object_files/preload.c.o target/object_files/record.c.o target/object_files/util.c.o Makefile | target/
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fpic -o $@ $(filter %.o,$^)

target/test_derelativize: target/object_files/test_derelativize.c.o target/object_files/util.c.o Makefile | target/
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(filter %.o,$^)

target/manpages/prefam.1: src/prefam.1.adoc | target/manpages/
	asciidoctor -D target/manpages $<

compile: target/libprefam.so

manpage: target/manpages/prefam.1

test: compile target/test_derelativize
	@./test.sh target/libprefam.so

coverage: CFLAGS += --coverage -fno-default-inline
coverage: LDFLAGS += --coverage
coverage: test | target/coverage/
	@lcov --output-file target/coverage.info --directory target --capture --exclude '/usr/include/*'
	@genhtml -o target/coverage target/coverage.info

-include target/dependencies/*.mk

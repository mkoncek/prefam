.PHONY: all compile test coverage manpage clean

all: compile

CC ?= cc
CXX ?= c++
CFLAGS ?= -Wall -Wextra -Wconversion -Wno-varargs -Og -g
CFLAGS += -std=c99
LDFLAGS ?=

clean:
	@rm -rf -v target

%/:
	@mkdir -p $@

target/object_files/preload.c.o: src/preload.c src/record.h Makefile | target/object_files/
	$(CC) $(CPPFLAGS) $(CFLAGS) -fpic -c -o $@ $<
target/object_files/record.c.o: src/record.c src/record.h Makefile | target/object_files/
	$(CC) $(CPPFLAGS) $(CFLAGS) -fpic -c -o $@ $<
target/object_files/util.c.o: src/util.c Makefile | target/object_files/
	$(CC) $(CPPFLAGS) $(CFLAGS) -fpic -c -o $@ $<
target/object_files/test_derelativize.c.o: src/test_derelativize.c Makefile | target/object_files/
	$(CC) $(CPPFLAGS) $(CFLAGS) -fpic -c -o $@ $<
target/test_derelativize: target/object_files/test_derelativize.c.o target/object_files/util.c.o Makefile | target/
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ target/object_files/test_derelativize.c.o target/object_files/util.c.o

target/libprefam.so: target/object_files/preload.c.o target/object_files/record.c.o Makefile | target/
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fpic -o $@ target/object_files/preload.c.o target/object_files/record.c.o target/object_files/util.c.o

target/manpages/prefam.1: src/prefam.1.adoc | target/manpages/
	asciidoctor -D target/manpages $<

compile: target/libprefam.so

manpage: target/manpages/prefam.1

test: compile target/test_derelativize
	@./test.sh target/libprefam.so

coverage: CFLAGS += --coverage -fno-default-inline
coverage: LDFLAGS += --coverage
coverage: test | target/coverage/
	@lcov --output-file target/coverage.info --directory target/object_files --capture --exclude '/usr/include/*'
	@genhtml -o target/coverage target/coverage.info

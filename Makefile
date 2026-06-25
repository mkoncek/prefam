.PHONY: all compile test manpage clean

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

target/preload.c.o: src/preload.c src/record.h Makefile | target/
	$(CC) $(CPPFLAGS) $(CFLAGS) -fpic -c -o $@ $<
target/record.c.o: src/record.c src/record.h Makefile | target/
	$(CC) $(CPPFLAGS) $(CFLAGS) -fpic -c -o $@ $<

target/libprefam.so: target/preload.c.o target/record.c.o Makefile | target/
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fpic -o $@ target/preload.c.o target/record.c.o

target/manpages/prefam.1: src/prefam.1.adoc | target/manpages/
	asciidoctor -D target/manpages $<

compile: target/libprefam.so

manpage: target/manpages/prefam.1

test: compile
	@./test.sh target/libprefam.so

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=$(STD)
CPPCHECK = cppcheck --enable=warning,style --std=$(STD)
STD = c11
OUTFILES = somefile.txt

TARGETS = bench_sys_call bench_context
SOURCES = $(TARGETS:=.c)

.PHONY: all cppcheck clean

all: $(TARGETS)

cppcheck: $(SOURCES)
	$(CPPCHECK) $(SOURCES)

clean:
	$(RM) $(TARGETS) $(OUTFILES)
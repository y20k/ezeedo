#
# Makefile
#
# Computer Science 50
# Final Project
#


# compiler to use
CC = clang

# flags to pass compiler
CFLAGS = -ggdb3 -O0 -Qunused-arguments -std=c99 -Wall -Werror -rdynamic `pkg-config --cflags  gtk+-3.0` `pkg-config --libs gtk+-3.0`

# name for executable
EXE = ezeedo

# space-separated list of header files
HDRS = ezeedo-core.h ezeedo-callbacks.h

# space-separated list of libraries, if any,
# each of which should be prefixed with -l
LIBS = 

# space-separated list of source files
SRCS = ezeedo.c ezeedo-core.c ezeedo-callbacks.c

# automatically generated list of object files
OBJS = $(SRCS:.c=.o)


# default target
$(EXE): $(OBJS) $(HDRS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# dependencies 
$(OBJS): $(HDRS) Makefile

# housekeeping
clean:
	rm -f core $(EXE) *.o

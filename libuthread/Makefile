# Target library
lib := libuthread.a

all: $(lib)

# Define compilation toolchain
CC	=	gcc

# General gcc options
CFLAGS	:=	-Wall	-Wextra	-Werror

## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif

## Dependency generation
CFLAGS	+= -MMD

# Application objects to compile
OBJS	:=	uthread.o	queue.o	context.o	sem.o	preempt.o

# Include dependencies
DEPS := $(patsubst %.o,%.d,$(OBJS))
-include $(DEPS)

libuthread.a:	$(OBJS)
	ar	rcs	$(lib)	$(OBJS)

# Generic rule for compiling objects
%.o: %.c
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

# Cleaning rule
clean:
	rm	-rf	$(OBJS)	$(DEPS)

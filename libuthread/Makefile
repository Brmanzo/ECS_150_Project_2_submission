# Target library
lib := libuthread.a

all: $(lib)

# Don't print the commands unless explicitly requested with `make V=1`
ifneq ($(V),1)
Q = @
V = 0
endif
# Current directory
CUR_PWD := $(shell pwd)

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
## Include path
CFLAGS 	+= -I$(UTHREADPATH)
## Dependency generation
CFLAGS	+= -MMD

# Linker options
LDFLAGS := -L$(UTHREADPATH) -luthread

# Application objects to compile
OBJS	:=	uthread.o	queue.o	context.o	sem.o

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
	@echo "CLEAN	$(CUR_PWD)"
	$(Q)$(MAKE) V=$(V) D=$(D) -C $(UTHREADPATH) clean
	$(Q)rm -rf $(OBJS) $(DEPS)

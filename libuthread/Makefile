# Target library
lib := libuthread.a

CC	:=	gcc
CFLAGS	:=	-Wall	-Wextra	-Werror	-MMD
OBJS	:=	uthread.o	queue.o	context.o	sem.o

all: $(lib)

deps := $(patsubst %.o,%.d,$(OBJS))
-include $(deps)

libuthread.a:	$(OBJS)
	ar	rcs	$(lib)	$(OBJS)

# Generic rule for compiling objects
%.o: %.c
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

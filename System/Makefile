CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS =
SRC = myFunction.c myShell.c
OBJS = $(SRC:.c=.o)
EXECUTABLE = myShell

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(OBJS)


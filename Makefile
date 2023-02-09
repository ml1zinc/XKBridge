TARGET  = xkbridge
CC      = gcc

SRC     = xkbridge.c keysym_custom.c win_init.c
OBJ     = $(SRC:.c=.o)

LDLIBS  = -lX11
LDFLAGS = -o $(TARGET)_debug $(LDLIBS)
ADDFLAGS= -O2 -fpie -Wl,-pie
CFDEBUG = -Wall -Wextra -g $(LDFLAGS)

all: bin

bin:
	$(CC) -o $(TARGET) $(SRC) $(LDLIBS) $(ADDFLAGS)

debug:
	$(CC) $(CFDEBUG) $(SRC)

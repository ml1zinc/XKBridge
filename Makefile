TARGET  = xkbridge
CC      = gcc

SRC     = xkbridge.c keysym_custom.c win_init.c
OBJ     = $(SRC:.c=.o)

LDLIBS  = -lX11
LDFLAGS = -o $(TARGET)_debug $(LDLIBS)

CFLAGS  = -shared -Wl,-soname,$(TARGET) -fPIC
CFDEBUG = -Wall -Wextra -g $(LDFLAGS)

all: bin

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(CFDEBUG)

bin:
	$(CC) -o $(TARGET) $(SRC) $(LDLIBS)

debug:
	$(CC) $(CFDEBUG) $(SRC)

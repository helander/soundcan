
NAME = setbfreecontrol

# installation path
INSTALL_PATH = /usr/lib/lv2
COMPLETE_INSTALL_PATH = $(DESTDIR)$(INSTALL_PATH)/$(NAME).lv2

# compiler and linker
CC ?= gcc

# flags
CFLAGS += -I. -c -ffast-math -fPIC -DPIC -Wall
LDFLAGS += -shared -lm

# libs
LIBS =

# remove command
RM = rm -f

# sources and objects
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

# plugin name
PLUGIN = plugin.so

$(PLUGIN): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(PLUGIN) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) src/*.o $(PLUGIN)

install:
	mkdir -p $(COMPLETE_INSTALL_PATH)
	cp $(PLUGIN) $(COMPLETE_INSTALL_PATH)
	cp ttl/*.ttl $(COMPLETE_INSTALL_PATH)

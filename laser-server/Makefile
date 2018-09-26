IDIR =./include
ODIR=./obj
SDIR=./src
LDIR =./lib

CC=gcc
CFLAGS=-I$(IDIR)

_DEPS = tim571server.h tim571resource.h tim571protocol.h tim571client.h tim571common.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ_S = tim571server.o tim571resource.o
OBJ_S = $(patsubst %,$(ODIR)/%,$(_OBJ_S))

_OBJ_C = tim571client.o
OBJ_C = $(patsubst %,$(ODIR)/%,$(_OBJ_C))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: clientmake servermake

clientmake: $(OBJ_C)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

servermake: $(OBJ_S)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 

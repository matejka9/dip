IDIR =./include
ODIR=./obj
SDIR=./src
LDIR =./lib

CC=gcc
CFLAGS=-I$(IDIR)

_DEPS = hough_test.h hough.h tim571.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ_H_T = hough_test.o hough.o
OBJ_H_T = $(patsubst %,$(ODIR)/%,$(_OBJ_H_T))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: test

test: $(OBJ_H_T)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

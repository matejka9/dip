IDIR =./include
ODIR=./obj
SDIR=./src
LDIR =./lib

CC=gcc
CFLAGS=-I$(IDIR)

_DEPS = hough_tests.h hough.h math_2d.h tim571.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ_H_T = hough_tests.o hough.o math_2d.o test_hough.o
OBJ_H_T = $(patsubst %,$(ODIR)/%,$(_OBJ_H_T))

_OBJ_M_T = math_2d.o test_math_2d.o
OBJ_M_T = $(patsubst %,$(ODIR)/%,$(_OBJ_M_T))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: test_hough test_math

test_hough: $(OBJ_H_T)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

test_math: $(OBJ_M_T)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

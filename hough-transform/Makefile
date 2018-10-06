IDIR =./include
ODIR=./obj
SDIR=./src
LDIR =./lib

CC=gcc
CFLAGS=-I$(IDIR)

_DEPS = tim571.h math_2d.h hough.h hough_tests.h segment.h corner.h filter.h util.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ_H_T = hough_tests.o hough.o math_2d.o test_hough.o
OBJ_H_T = $(patsubst %,$(ODIR)/%,$(_OBJ_H_T))

_OBJ_M_T = math_2d.o test_math_2d.o
OBJ_M_T = $(patsubst %,$(ODIR)/%,$(_OBJ_M_T))

_OBJ_S_T = math_2d.o hough.o segment.o test_segment.o
OBJ_S_T = $(patsubst %,$(ODIR)/%,$(_OBJ_S_T))

_OBJ_C_T = math_2d.o corner.o test_corner.o
OBJ_C_T = $(patsubst %,$(ODIR)/%,$(_OBJ_C_T))

_OBJ_F_T = util.o math_2d.o hough.o filter.o test_filter.o
OBJ_F_T = $(patsubst %,$(ODIR)/%,$(_OBJ_F_T))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: test_hough test_math test_segment test_corner test_filter

test_hough: $(OBJ_H_T)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

test_math: $(OBJ_M_T)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

test_segment: $(OBJ_S_T)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

test_corner: $(OBJ_C_T)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

test_filter: $(OBJ_F_T)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

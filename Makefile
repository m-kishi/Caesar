CC      = g++
CFLAGS  = -Wall
HDRS    = head.hpp csar.hpp
LDFLAGS =
LIBS    =
OBJS    = main.o proc.o disp.o csar.o
PROGRAM = csar

all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

clean: $(OBJS)
	rm -f *.o

###
*.o: *.cpp *.hpp


CC=clang++
SO=liblle.so

# CFLAGS=
CFLAGS=-g

HEADERS=\
	lle.h

CPP=\
	lle.cpp

OBJ=$(CPP:.cpp=.o)

all: $(SO)

clean:
	rm -f *.o *.so

$(SO): $(OBJ)
	$(CC) -shared $(OBJ) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -O3 $*.cpp -o $@
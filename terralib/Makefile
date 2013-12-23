
CC=clang++
SO=libmanifold.so

# CFLAGS=
CFLAGS=-g -I.. -I../shared

HEADERS=\
	exports.h \
	../shared/Graph.h \
	../shared/KNN.h \
	../shared/lle.h

CPP=\
	exports.cpp \
	../shared/Graph.cpp \
	../shared/KNN.cpp \
	../shared/lle.cpp

OBJ=$(CPP:.cpp=.o)

all: $(SO)

clean:
	rm -f *.o *.so

$(SO): $(OBJ)
	$(CC) -shared $(OBJ) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -O3 $*.cpp -o $@

CC=clang++
SO=libmanifold.so

# CFLAGS=
CFLAGS=-g

HEADERS=\
	exports.h \
	Graph.h \
	KNN.h \
	lle.h

CPP=\
	exports.cpp \
	Graph.cpp \
	KNN.cpp \
	lle.cpp

OBJ=$(CPP:.cpp=.o)

all: $(SO)

clean:
	rm -f *.o *.so

$(SO): $(OBJ)
	$(CC) -shared $(OBJ) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -O3 $*.cpp -o $@

CC=clang++
EXE=app

CFLAGS=-g -I.. -I../shared
LDFLAGS=-framework OpenGL -framework GLUT -lfreeimage

HEADERS=\
	../shared/Graph.h \
	../shared/KNN.h \
	../shared/lle.h

CPP=\
	main.cpp \
	../shared/Graph.cpp \
	../shared/KNN.cpp \
	../shared/lle.cpp

OBJ=$(CPP:.cpp=.o)

all: $(EXE)

clean:
	rm -f *.o *.so

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -O3 $*.cpp -o $@
CC = g++
CFLAGS = -std=c++23 -O3 -ffast-math -g #-DTRACK_SUN

LIBS = -lglfw -lGLEW -lGL -lpugixml -lm -ldl -pthread

SOURCES = main.cpp src/graphics.cpp src/physics.cpp src/utils.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: program

program: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o program

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) program


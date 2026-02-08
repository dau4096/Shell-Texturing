CC = g++
CFLAGS = -std=c++23 -O2 -ffast-math \
         -I/usr/include \
         -I/usr/include/GL \
         -I/usr/include/glm \
	 -I/usr/local/include

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


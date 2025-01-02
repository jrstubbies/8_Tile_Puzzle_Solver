CC := g++
TARGET := search

# Detect the operating system
ifeq ($(OS),Windows_NT)

	CFLAGS := -O2 -std=c++11 -Wall -c
    LFLAGS := -lgdi32

    EXTENSION := .exe
	CLEANUP := del
	CLEANUP_OBJS := del *.o


	# Find all source files (.cpp) and header files (.h)
	SRCS := $(wildcard *.cpp) $(wildcard */*.cpp)
	HDRS := $(wildcard *.h) $(wildcard */*.h)
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		# macOS
		EXTENSION := .out
		CFLAGS := -O2 -std=c++11 -Wall -I/usr/local/include -L/usr/local/lib -c -Wno-write-strings
		LFLAGS := -L/usr/local/lib -lSDL_bgi -lSDL2 
		CLEANUP := rm -f
		CLEANUP_OBJS := rm -f *.o

		# Find all source files (.cpp) and header files (.h)
		SRCS := main.cpp puzzle.cpp algorithm.cpp 
		HDRS := main.h puzzle.h algorithm.h 
	else ifeq ($(UNAME_S),Linux)
		# Linux
		EXTENSION := .out
		CFLAGS := -O2 -std=c++11 -Wall -I/usr/local/include -L/usr/local/lib -c -Wno-write-strings  
		LFLAGS := -lSDL_bgi -lSDL2 
		CLEANUP := rm -f
		CLEANUP_OBJS := rm -f *.o

		# Find all source files (.cpp) and header files (.h)
		SRCS := main.cpp puzzle.cpp algorithm.cpp 
		HDRS := main.h puzzle.h algorithm.h 
	endif
endif



# Create object file names based on source file names
OBJS := $(SRCS:.cpp=.o)


# Rule to build the executable
$(TARGET)$(EXTENSION): $(OBJS)
	$(CC) -O2 -std=c++11 -o $@ $(OBJS) $(LFLAGS)

# Rule to build object files
%.o: %.cpp $(HDRS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(CLEANUP) $(TARGET)$(EXTENSION)
	$(CLEANUP_OBJS)

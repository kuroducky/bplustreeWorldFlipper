# we can define variables in a makefile
# variable CC will specify the compiler; feel free to use clang++
CC:=g++

# this variable contains any extra compiler options that we might
# want to add, like -O3, -march=native, etc.
# -O3 tells g++ to optimize the code for speed
CC_OPTS:=-O3

# this variable will contain the names of all cpp source files
SRCS:=$(wildcard *.cpp)

# variable with all header files
HEADERS:=$(wildcard *.h)

# this will contain the names of all intermediate object files
OBJECTS:=$(patsubst %.cpp,bin/%.o,$(SRCS))

# this rule is fancier now
# $< are the names of all prerequisites (the object files)
# $@ is the name of the target (bin/myprogram in this case)
bin/main: $(OBJECTS)
	$(CC) $^ $(CC_OPTS) -o $@

# but now we have to tell make how to build the object files
# -c option tells g++ to only compile one source file at a tile
#  $< is the name of the first prerequisite (the cpp file in this case)
bin/%.o: %.cpp $(HEADERS)
	$(CC) $< $(CC_OPTS) -c -o $@
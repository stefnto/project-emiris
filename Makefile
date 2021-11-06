CC=g++
INCLUDE=hdr
FLAGZ=-std=c++11
# SOURCES=$(wildcard src/*.cpp)
# BINS=$(patsubst src/%.cpp,bin/%.o,$(SOURCES))


all: LSH cube

# LSH: $(BINS)
# 	$(CC) -o LSH $^
#
# $(BINS) : bin/%.o : src/%.cpp
# 	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

LSH: src/LSHmain.cpp src/LSH.cpp src/utils.cpp
	$(CC) $(FLAGZ) -I $(INCLUDE) -g -o LSH src/LSHmain.cpp src/LSH.cpp src/utils.cpp

cube: src/Cubemain.cpp src/Cube.cpp src/utils.cpp
	$(CC) $(FLAGZ) -I $(INCLUDE) -g -o cube src/Cubemain.cpp src/Cube.cpp src/utils.cpp

# LSHdefault:
# 	./LSH -i input/input_b_id -q input/query_b_id -k 5 -L 5 -o output  -N 3 -R 10000

clean:
	rm -f  bin/*
	rm -f main

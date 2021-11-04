CC=g++
INCLUDE=hdr
FLAGZ=-std=c++11
LSHSOURCES=$(wildcard src/lsh/*.cpp)
# BINS=$(patsubst src/%.cpp,bin/%.o,$(SOURCES))
LSHBINS=$(patsubst src/lsh/%.cpp,bin/lsh/%.o,$(LSHSOURCES))
CUBESOURCES=$(wildcard src/cube/*.cpp)
CUBEBINS=$(patsubst src/cube/%.cpp,bin/cube/%.o,$(CUBESOURCES))


all: lsh cube

lsh: $(LSHBINS) bin/utils/utils.o
	$(CC) -o lsh $^

$(LSHBINS) : bin/lsh/%.o : src/lsh/%.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/utils/utils.o : src/utils/utils.cpp
	$(CC) $(flagz) -c -I $(INCLUDE) $< -o $@

cube: $(CUBEBINS) bin/utils/utils.o
	$(CC) -o cube $^

$(CUBEBINS) : bin/cube/%.o : src/cube/%.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@



#LSH: src/LSHmain.cpp src/LSH.cpp src/utils.cpp
#	$(CC) $(FLAGZ) -I $(INCLUDE) -o LSH src/LSHmain.cpp src/LSH.cpp src/utils.cpp

#cube: src/Cubemain.cpp src/Cube.cpp src/utils.cpp
#	$(CC) $(FLAGZ) -I $(INCLUDE) -g -o cube src/Cubemain.cpp src/Cube.cpp src/utils.cpp

LSHdefault:
	./LSH -i input/input_b_id -q input/query_b_id -k 5 -L 5 -o output  -N 3 -R 10000

clean:
	rm -f  bin/*
	rm -f main

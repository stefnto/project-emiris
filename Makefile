CC=g++
INCLUDE=hdr
FLAGZ=-std=c++11
LSHSOURCES=$(wildcard src/lsh/*.cpp)
# BINS=$(patsubst src/%.cpp,bin/%.o,$(SOURCES))
LSHBINS=$(patsubst src/lsh/%.cpp,bin/lsh/%.o,$(LSHSOURCES))
CUBESOURCES=$(wildcard src/cube/*.cpp)
CUBEBINS=$(patsubst src/cube/%.cpp,bin/cube/%.o,$(CUBESOURCES))
CLUSTERSOURCES = $(wildcard src/clustering/*.cpp)
CLUSTERBINS=$(patsubst src/clustering/%.cpp,bin/clustering/%.o,$(CLUSTERSOURCES))



all: lsh cube clustering


lsh: $(LSHBINS) bin/utils/utils.o bin/mains/LSHmain.o
	$(CC) -o lsh $^

$(LSHBINS) : bin/lsh/%.o : src/lsh/%.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/utils/utils.o : src/utils/utils.cpp
	$(CC) $(flagz) -c -I $(INCLUDE) $< -o $@

bin/mains/LSHmain.o: src/mains/LSHmain.cpp
	$(CC) $(flagz) -c -I $(INCLUDE) $< -o $@

bin/mains/Cubemain.o: src/mains/Cubemain.cpp
	$(CC) $(flagz) -c -I $(INCLUDE) $< -o $@

cube: $(CUBEBINS) bin/utils/utils.o bin/mains/Cubemain.o
	$(CC) -o cube $^

$(CUBEBINS) : bin/cube/%.o : src/cube/%.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

clustering: $(CLUSTERBINS) bin/utils/utils.o $(LSHBINS) $(CUBEBINS) 
	$(CC) -o clustering $^

$(CLUSTERBINS) : bin/clustering/%.o : src/clustering/%.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

#LSH: src/LSHmain.cpp src/LSH.cpp src/utils.cpp
#	$(CC) $(FLAGZ) -I $(INCLUDE) -o LSH src/LSHmain.cpp src/LSH.cpp src/utils.cpp

#cube: src/Cubemain.cpp src/Cube.cpp src/utils.cpp
#	$(CC) $(FLAGZ) -I $(INCLUDE) -g -o cube src/Cubemain.cpp src/Cube.cpp src/utils.cpp

LSHdefault:
	./lsh -i input/input_small_id.txt -q input/query_small_id.txt -k 5 -L 5 -o output2  -N 3 -R 10000

CUBEdefault:
	./cube -i input/input_small_id.txt -q input/query_small_id.txt -k 5 -L 5 -o outputC  -N 3 -R 10000

clean:
	rm -f bin/utils/* bin/cube/* bin/lsh/* bin/clustering/*
	rm -f lsh cube clustering

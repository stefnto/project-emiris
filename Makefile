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

<<<<<<< HEAD
lsh: $(LSHBINS) bin/utils/utils.o
	$(CC) -o lsh $^
=======
LSH: src/LSHmain.cpp src/LSH.cpp src/utils.cpp
	$(CC) $(FLAGZ) -I $(INCLUDE) -g -o LSH src/LSHmain.cpp src/LSH.cpp src/utils.cpp
>>>>>>> 60cc996631977b43558318f5736544b7fcb622ec

$(LSHBINS) : bin/lsh/%.o : src/lsh/%.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/utils/utils.o : src/utils/utils.cpp
	$(CC) $(flagz) -c -I $(INCLUDE) $< -o $@

cube: $(CUBEBINS) bin/utils/utils.o
	$(CC) -o cube $^

$(CUBEBINS) : bin/cube/%.o : src/cube/%.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

clustering: $(CLUSTERBINS) bin/utils/utils.o $(LSHBINS) 
	$(CC) -o clustering $^

$(CLUSTERBINS) : bin/clustering/%.o : src/clustering/%.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

#LSH: src/LSHmain.cpp src/LSH.cpp src/utils.cpp
#	$(CC) $(FLAGZ) -I $(INCLUDE) -o LSH src/LSHmain.cpp src/LSH.cpp src/utils.cpp

#cube: src/Cubemain.cpp src/Cube.cpp src/utils.cpp
#	$(CC) $(FLAGZ) -I $(INCLUDE) -g -o cube src/Cubemain.cpp src/Cube.cpp src/utils.cpp

LSHdefault:
	./LSH -i input/input_small_id.txt -q input/query_small_id.txt -k 5 -L 5 -o output  -N 3 -R 10000

clean:
	rm -f bin/utils/* bin/cube/* bin/lsh/* bin/clustering/*
	rm -f lsh cube clustering 

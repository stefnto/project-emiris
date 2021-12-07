CC=g++
INCLUDE=hdr
FLAGZ=-std=c++11
LSHSOURCES=$(wildcard src/algorithms/LSH.cpp)
# BINS=$(patsubst src/%.cpp,bin/%.o,$(SOURCES))
LSHBINS=$(patsubst src/algorithms/LSH.cpp,bin/LSH.o,$(LSHSOURCES))
CUBESOURCES=$(wildcard src/algorithms/Cube.cpp)
CUBEBINS=$(patsubst src/algorithms/Cube.cpp,bin/Cube.o,$(CUBESOURCES))
CLUSTERSOURCES = $(wildcard src/algorithms/clustering.cpp)
CLUSTERBINS=$(patsubst src/algorithms/clustering.cpp,bin/clustering.o,$(CLUSTERSOURCES))



all: lsh cube clustering


lsh: bin/LSH.o bin/utils/utils.o bin/mains/LSHmain.o
	$(CC) -o lsh $^

cube: bin/Cube.o bin/utils/utils.o bin/mains/Cubemain.o
	$(CC) -o cube $^

clustering: bin/clustering.o bin/utils/utils.o bin/LSH.o bin/Cube.o bin/mains/clustermain.o
	$(CC) -o clustering $^

# $(LSHBINS) : bin/LSH.o

bin/utils/utils.o : src/utils/utils.cpp
	$(CC) $(flagz) -c -I $(INCLUDE) $< -o $@

bin/LSH.o : src/algorithms/LSH.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/Cube.o : src/algorithms/Cube.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/clustering.o : src/algorithms/clustering.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/mains/LSHmain.o: src/mains/LSHmain.cpp
	$(CC) $(flagz) -c -I $(INCLUDE) $< -o $@

bin/mains/Cubemain.o: src/mains/Cubemain.cpp
	$(CC) $(flagz) -c -I $(INCLUDE) $< -o $@

bin/mains/clustermain.o : src/mains/clustermain.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@



# $(CUBEBINS) : bin/Cube.o





# $(CLUSTERBINS) : bin/mains/clustermain.o



#LSH: src/LSHmain.cpp src/LSH.cpp src/utils.cpp
#	$(CC) $(FLAGZ) -I $(INCLUDE) -o LSH src/LSHmain.cpp src/LSH.cpp src/utils.cpp

#cube: src/Cubemain.cpp src/Cube.cpp src/utils.cpp
#	$(CC) $(FLAGZ) -I $(INCLUDE) -g -o cube src/Cubemain.cpp src/Cube.cpp src/utils.cpp

LSHdefault:
	./lsh -i input/nasdaq2015_2017.csv -q input/curve_query_100.txt -k 5 -L 5 -o outputw4  -N 3 -R 10000

CUBEdefault:
	./cube -i input/input_small_id.txt -q input/query_small_id.txt -k 5 -L 5 -o outputC  -N 3 -R 10000

clean:
	rm -f bin/mains/* bin/clustering.i bin/LSH.o bin/Cube.o
	rm -f lsh cube clustering

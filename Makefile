CC=g++
INCLUDE=hdr
FLAGZ= -O2 -std=c++14
# LSHSOURCES=$(wildcard src/algorithms/LSH.cpp)
# # BINS=$(patsubst src/%.cpp,bin/%.o,$(SOURCES))
# LSHBINS=$(patsubst src/algorithms/LSH.cpp,bin/LSH.o,$(LSHSOURCES))
# CUBESOURCES=$(wildcard src/algorithms/Cube.cpp)
# CUBEBINS=$(patsubst src/algorithms/Cube.cpp,bin/Cube.o,$(CUBESOURCES))
# CLUSTERSOURCES = $(wildcard src/algorithms/clustering.cpp)
# CLUSTERBINS=$(patsubst src/algorithms/clustering.cpp,bin/clustering.o,$(CLUSTERSOURCES))



all: lsh cube clustering search cclustering unittest


lsh: bin/LSH.o bin/utils/utils.o bin/mains/LSHmain.o
	$(CC) -o lsh $^

search: bin/Curves.o bin/LSH.o bin/utils/utils.o bin/mains/Curvesmain.o bin/LSH.o bin/Cube.o bin/fred/config.o bin/fred/curve.o bin/fred/frechet.o bin/fred/interval.o bin/fred/point.o bin/fred/simplification.o
	$(CC) -o search $^

cube: bin/Cube.o bin/utils/utils.o bin/mains/Cubemain.o
	$(CC) -o cube $^

clustering: bin/clustering.o bin/utils/utils.o bin/LSH.o bin/Cube.o bin/mains/clustermain.o
	$(CC) -o clustering $^

cclustering: bin/curves_clustering.o bin/utils/utils.o bin/mains/Curvesclstmain.o bin/Curves.o bin/LSH.o bin/Cube.o bin/clustering.o bin/fred/config.o bin/fred/curve.o bin/fred/frechet.o bin/fred/interval.o bin/fred/point.o bin/fred/simplification.o
	$(CC) -o cclustering $^

unittest: unit-testing/unitest.cpp bin/Curves.o bin/LSH.o bin/utils/utils.o bin/Cube.o bin/fred/config.o bin/fred/curve.o bin/fred/frechet.o bin/fred/interval.o bin/fred/point.o bin/fred/simplification.o
	$(CC) -o unittest $^

# $(LSHBINS) : bin/LSH.o

bin/utils/utils.o : src/utils/utils.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o$@

bin/LSH.o : src/algorithms/LSH.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/Curves.o : src/algorithms/Curves.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/Cube.o : src/algorithms/Cube.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/clustering.o : src/algorithms/clustering.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/curves_clustering.o : src/algorithms/curves_clustering.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@


bin/mains/LSHmain.o: src/mains/LSHmain.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/mains/Curvesmain.o: src/mains/Curvesmain.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/mains/Cubemain.o: src/mains/Cubemain.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/mains/clustermain.o : src/mains/clustermain.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/mains/Curvesclstmain.o : src/mains/Curvesclstmain.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

# Fred-Frechet algorithms compile rules

bin/fred/config.o: fred-frechet/src/config.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/fred/curve.o: fred-frechet/src/curve.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/fred/frechet.o: fred-frechet/src/frechet.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/fred/interval.o: fred-frechet/src/interval.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/fred/point.o: fred-frechet/src/point.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

bin/fred/simplification.o: fred-frechet/src/simplification.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@


# $(CUBEBINS) : bin/Cube.o





# $(CLUSTERBINS) : bin/mains/clustermain.o



#LSH: src/LSHmain.cpp src/LSH.cpp src/utils.cpp
#	$(CC) $(FLAGZ) -I $(INCLUDE) -o LSH src/LSHmain.cpp src/LSH.cpp src/utils.cpp

#cube: src/Cubemain.cpp src/Cube.cpp src/utils.cpp
#	$(CC) $(FLAGZ) -I $(INCLUDE) -g -o cube src/Cubemain.cpp src/Cube.cpp src/utils.cpp

LSHcdefault:
	./lshc -i input/nasdaq2015_2017.csv -q input/curve_query_10.txt -k 5 -L 5 -o outputw4  -N 3 -R 10000

LSHdefault:
	./lsh -i input/nasdaq2015_2017.csv -q input/curve_query_100.txt -k 5 -L 5 -o outputw4  -N 3 -R 10000

CUBEdefault:
	./cube -i input/input_small_id.txt -q input/query_small_id.txt -k 5 -L 5 -o outputC  -N 3 -R 10000

clean:
	rm -f bin/*.o
	rm -f bin/*/*
	rm -f lsh cube cclustering search unittest *.o

CC=g++ 
INCLUDE=hdr
FLAGZ=-std=c++11 
SOURCES=$(wildcard src/*.cpp)
BINS=$(patsubst src/%.cpp,bin/%.o,$(SOURCES))


all: LSH
	
LSH: $(BINS)
	$(CC) -o LSH $^	

$(BINS) : bin/%.o : src/%.cpp
	$(CC) $(FLAGZ) -c -I $(INCLUDE) $< -o $@

LSHdefault:
	./LSH -i input/input_small_id.txt -q input/query_small_id.txt -k 5 -L 5 -o output  -N 3 -R 10000

clean:
	rm -f  bin/*
	rm -f main	

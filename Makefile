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
	./LSH -i input/input_b_id -q input/query_b_id -k 2 -L 1 -o output_file.txt -N 3 -R 5

clean:
	rm -f  bin/*
	rm -f main	

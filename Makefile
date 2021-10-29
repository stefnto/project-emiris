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


clean:
	rm -f  *.o
	rm -f travelMonitorClient
	rm -f utilobjs/*
	

#ifndef __LSH__
#define __LSH__

#include <iostream>
#include <vector>
#include <cmath>
#include <list>
#include <random>
#include <chrono>
#include "utils.hpp"



class LSH_Exception{};

class LSH_item{
    private:
        std::string item_id;
        std::vector<int> coordinates;
        long ID;
    public:
        LSH_item(std::string item_id,std::vector<int> coordinates);
        LSH_item(std::string line);
        ~LSH_item() = default;
        LSH_item(LSH_item&) = default;
        void set_id(long id);
        long get_id();

        const std::vector<int>& getCoordinates() const;
        void print_coordinates();
        int get_coordinates_size();
};
class hFunction{                            // floor( (p*v + t)/ w )

    private:
        std::vector<float> v;               //contains vector V
        float t;
        const int w;

    public:
        hFunction(int itemSize,int w = 4);
        int operator()(const LSH_item&);
};
class gFunction{                                                                                        //Σ r_i * h_i          
        private:

        int tableSize;
        int k;
        std::vector<std::pair<int,hFunction>> linearCombinationElements;

    public:
        gFunction() = default;
        void init(int,int,int);
        gFunction(int itemDim,int k,int tableSize);                                                         //itemDim = size of vector that contains the coordinates
        int operator()(LSH_item&);                                                                          //Hashing Function

};
class LSH_HashTable{
private:
    int size;
    int k;                                                                                                  //Number of H functions used by hashingFunction
    gFunction hashingFunction;
    std::list<LSH_item*>* buckets;                                                                          //Array of Lists Aka Hash Table;
    
public:
    LSH_HashTable() = default;
    void init(int,int,int);
    LSH_HashTable(int itemDim, int tableSize, int k);                                                       //Constructs H and G functions;
    ~LSH_HashTable();
    LSH_HashTable(LSH_HashTable&) = default;
    void insert(LSH_item*); 
};

class LSH_solver{
    private:
        LSH_HashTable* hashTables;                                          //Hash tables using the G hash functions
        int n;                                                              //number of Nearest Neighbours we're looking for
        int r;                                                              //the search is made inside the R Radius
        const int L;                                                        //number of HashTables;
        std::vector<LSH_item*> points_coordinates;

        int read_data(std::string data_path);                               //returns lines read

    public:

        //H functions are constructed inside the LSH_solver constructor and picked by the G functions.
        LSH_solver(std::string dataset_path,int k = 4,int L=5,int N = 1,int R = 10000,double (*distanceFunction)(std::vector<int> a, std::vector<int> b) = EuclidianDistance);
        bool solve(std::string query_path, std::string output_path);        //This function is called to solve NN , kNN and Approximate Range Search.
        



};



std::vector<LSH_item*> itemGenerator(int amount,int itemSize);












#endif

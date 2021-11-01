#ifndef __LSH__
#define __LSH__

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <list>
#include <queue>
#include <queue>
#include <random>
#include <chrono>
#include <set>
#include "utils.hpp"



class LSH_Exception{};

class LSH_item{
    private:
        std::string item_id;
        std::vector<int> coordinates;
        long ID;
        double distanceFromQuery = 0;

        static double (*distanceFunction)(std::vector<int> a,std::vector<int> b);
    public:

        LSH_item(std::string item_id, std::vector<int> coordinates);
        LSH_item(std::string line);
        ~LSH_item() = default;
        LSH_item(LSH_item&) = default;
        void set_id(long id);
        long get_id() const;

        const std::vector<int>& getCoordinates() const;
        void print_coordinates();
        int get_coordinates_size();
        void setDistanceFromQuery(LSH_item* query);
        float getDistanceFromQuery() const;
        std::string getItemID() const;

        static void setDistanceFunction(double (*distanceFunction)(std::vector<int> a, std::vector<int>b) );
};

using LSH_Set = std::set<LSH_item *, bool (*)(const LSH_item *a, const LSH_item *b)>;

class hFunction{                            // floor( (p*v + t)/ w )

    private:
        std::vector<float> v;               //contains vector V
        float t;
        const int w;

    public:
        hFunction(int itemSize,int w = 4);
        int operator()(const LSH_item*);
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
        int operator()(LSH_item*);                                                                          //Hashing Function

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
    void NearestNeighbours(LSH_item*,LSH_Set*);
};

class LSH_solver{
    private:
        LSH_HashTable* hashTables;                                          //Hash tables using the G hash functions
        int n;                                                              //number of Nearest Neighbours we're looking for
        int r;                                                              //the search is made inside the R Radius
        const int L;                                                        //number of HashTables;
        std::vector<LSH_item*> points_coordinates;
        std::vector<LSH_item*> queries;

        std::string output_filepath;

        int readItems(std::string data_path,std::vector<LSH_item*>&);                               //returns lines read
        void writeResult(LSH_Set*,LSH_item*);

    public :
        //H functions are constructed inside the LSH_solver constructor and picked by the G functions.
        LSH_solver(std::string dataset_path, std::string query_path, std::string output_file, int k = 4, int L = 5, int N = 1, int R = 10000, double (*distanceFunction)(std::vector<int> a, std::vector<int> b) = EuclidianDistance);
        ~LSH_solver();
        bool solve();                                                                //This function is called to solve NN , kNN and Approximate Range Search.
        LSH_Set* NNandRS(LSH_item *item);       //1-NN , k-NN and Approximate Range Search, returns LSH_Set with nearest neighbours
        void printQueries() const;
};



std::vector<LSH_item*> itemGenerator(int amount,int itemSize);


#endif

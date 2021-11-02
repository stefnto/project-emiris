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

#include <set>
#include "utils.hpp"



class LSH_Exception{};

// class Data_item{
//     private:
//         std::string item_id;                                                    // is id from input_file
//         std::vector<int> coordinates;
//         long ID;                                                                // id computed from (Σ r * h) mod M
//         double distanceFromQuery = 0;
//
//         static double (*distanceFunction)(std::vector<int> a,std::vector<int> b);
//     public:
//
//         Data_item(std::string item_id, std::vector<int> coordinates);
//         Data_item(std::string line);
//         ~Data_item() = default;
//         Data_item(Data_item&) = default;
//         void set_id(long id);
//         long get_id() const;
//
//         const std::vector<int>& getCoordinates() const;
//         void print_coordinates();
//         int get_coordinates_size();
//         void setDistanceFromQuery(Data_item* query);
//         float getDistanceFromQuery() const;
//         std::string getItemID() const;
//
//         static void setDistanceFunction(double (*distanceFunction)(std::vector<int> a, std::vector<int>b) );
// };

using LSH_Set = std::set<Data_item *, bool (*)(const Data_item *a, const Data_item *b)>;

// class hFunction{                            // floor( (p*v + t)/ w )
//
//     private:
//         std::vector<float> v;               //contains vector V
//         float t;
//         const int w;
//
//     public:
//         hFunction(int itemSize,int w = 4);
//         int operator()(const Data_item*);
// };

class gFunction{                                                                                        //Σ r_i * h_i
        private:

        int tableSize;
        int k;
        std::vector<std::pair<int,hFunction>> linearCombinationElements;

    public:
        gFunction() = default;
        void init(int,int,int);
        gFunction(int itemDim,int k,int tableSize);                                                         //itemDim = size of vector that contains the coordinates
        int operator()(Data_item*);                                                                          //Hashing Function

};
class LSH_HashTable{
private:
    int size;
    int k;                                                                                                  //Number of H functions used by hashingFunction
    gFunction hashingFunction;
    std::list<Data_item*>* buckets;                                                                          //Array of Lists Aka Hash Table;

public:
    LSH_HashTable() = default;
    void init(int,int,int);
    LSH_HashTable(int itemDim, int tableSize, int k);                                                       //Constructs H and G functions;
    ~LSH_HashTable();
    LSH_HashTable(LSH_HashTable&) = default;
    void insert(Data_item*);
    void NearestNeighbours(Data_item*,LSH_Set*);
};

class LSH_solver{
    private:
        LSH_HashTable* hashTables;                                          //Hash tables using the G hash functions
        int n;                                                              //number of Nearest Neighbours we're looking for
        int r;                                                              //the search is made inside the R Radius
        const int L;                                                        //number of HashTables;
        std::vector<Data_item*> points_coordinates;
        std::vector<Data_item*> queries;

        std::string output_filepath;

        int readItems(std::string data_path,std::vector<Data_item*>&);                               //reads from data path and inserts to vector
        void writeResult(LSH_Set*,Data_item*);                                                       //given an ordered set,writes items to output path

    public :
        //H functions are constructed inside the LSH_solver constructor and picked by the G functions.
        LSH_solver(std::string dataset_path, std::string query_path, std::string output_file, int k = 4, int L = 5, int N = 1, int R = 10000, double (*distanceFunction)(std::vector<int> a, std::vector<int> b) = EuclidianDistance);
        ~LSH_solver();
        bool solve();                                                                //This function is called to solve NN , kNN and Approximate Range Search.
        LSH_Set* NNandRS(Data_item *item);                                           //1-NN , k-NN and Approximate Range Search, returns LSH_Set with nearest neighbours
        void printQueries() const;
};



std::vector<Data_item*> itemGenerator(int amount,int itemSize);


#endif

#ifndef __LSH__HPP__
#define __LSH__HPP__

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

using LSH_Set = std::set<Data_item *, bool (*)(const Data_item *a, const Data_item *b)>;

class gFunction{                                                                //Σ r_i * h_i
  private:
    int tableSize;
    int k;
    std::vector<std::pair<int,hFunction>> linearCombinationElements;
    int w;
  public:
    gFunction() = default;
    void init(int,int,int, int);
    gFunction(int itemDim,int k,int tableSize, int w);                          //itemDim = size of vector that contains the coordinates
    int operator()(Data_item*);                                                 //Hashing Function
};

class LSH_HashTable{
  private:
    int size;
    int k;                                                                      //Number of H functions used by hashingFunction
    gFunction hashingFunction;
    std::list<Data_item*>* buckets;                                             //Array of Lists Aka Hash Table;

  public:
    LSH_HashTable() = default;
    void init(int,int,int, int);
    LSH_HashTable(int itemDim, int tableSize, int k, int w);                    //Constructs H and G functions;
    ~LSH_HashTable();
    LSH_HashTable(LSH_HashTable&) = default;
    void insert(Data_item*);
    void NearestNeighbours(Data_item*,LSH_Set*);
    int clusteringRangeSearch(Data_item* query,float radius);
};

class LSH_solver{
  private:
    LSH_HashTable* hashTables;                                                  //Hash tables using the G hash functions
    int n;                                                                      //number of Nearest Neighbours we're looking for
    int r;                                                                      //the search is made inside the R Radius
    const int L;                                                                //number of HashTables;
    std::vector<Data_item*> points_coordinates;
    std::vector<Data_item*> queries;
    std::string output_filepath;
    int w;

    //if clustering Mode is on data are used from clusteringData
    // bool clusteringMode = false;
    // std::vector<clustering_data_item*>* clusteringData;


    void writeResult(LSH_Set* , Data_item* , std::set<double>&);                                //given an ordered set,writes items to output path

  public :
    //H functions are constructed inside the LSH_solver constructor and picked by the G functions.
    LSH_solver(std::string dataset_path, std::string query_path, std::string output_file, int k = 4, int L = 5, int N = 1, int R = 10000, double (*distanceFunction)(const std::vector<int>& a,const std::vector<int>& b) = EuclidianDistance);
    // LSH_solver(std::vector<clustering_data_item*>*clusteringData, int k = 4, int L = 5, int N = 1, int R = 10000, double (*distanceFunction)(const std::vector<int>& a,const std::vector<int>& b) = EuclidianDistance);
    ~LSH_solver();
    bool solve();                                                                //This function is called to solve NN , kNN and Approximate Range Search.
    // int clusteringRangeSearch(float radius, Data_item* cent, int id);            //We use this function to rangeSearch for points near the centroid given
    LSH_Set* NNandRS(Data_item *item);                                           //1-NN , k-NN and Approximate Range Search, returns LSH_Set with nearest neighbours
    void printQueries() const;
};



std::vector<Data_item*> itemGenerator(int amount,int itemSize);


#endif

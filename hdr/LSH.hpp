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

using LSH_Set = std::set<Data_point* , bool (*)(const Data_point *a, const Data_point *b)>;

class gFunction{                                                                // Σ r_i * h_i
  private:
    unsigned long long tableSize;
    int k;
    std::vector<std::pair<int,hFunction>> linearCombinationElements;
    int w;
  public:
    gFunction() = default;
    void init(int itemDim, int k, unsigned long long tableSize, int w);
    gFunction(int itemDim, int k, unsigned long long tableSize, int w);         // itemDim = size of vector that contains the coordinates

    template <typename T>
    int operator()(T* item, const std::vector<double>* altered_coordinates = nullptr){                                                    // Hashing Function
      long M = 0xFFFFFFFF - 4;                                                  // 2^32 - 1 - 4
      long sum = 0 ;

      for (std::pair<int,hFunction> elem : linearCombinationElements)
        sum += mod( ( elem.first * elem.second(item, altered_coordinates) ), M );


      sum = mod(sum, M);
      // std::cout << "Point : " << item->get_item_id();
      // std::cout << ", sum = " << sum << std::endl;
      item->ID_push_back(sum);

      return mod(sum, tableSize);
    }
};

class LSH_HashTable: public HashTable {
  protected:
    gFunction hashingFunction;
    std::list<Data_point*>* buckets;                                            // Array of Lists Aka Hash Table;

  public:
    LSH_HashTable(){};
    LSH_HashTable(int itemDim, unsigned long long tableSize, int k, int w);     // Constructs H and G functions;
    virtual ~LSH_HashTable();

    void init(int itemDim, unsigned long long tableSize, int k, int w);

    void insert(Data_point* item);
    void NearestNeighbours(Data_query* query, LSH_Set* ordSet,int index);

};

class LSH_solver: public Solver {
  private:
    const int l;                                                                // number of HashTables;
    int w;

    std::vector<Data_point*> points_coordinates;
    std::vector<Data_query*> queries;
    LSH_HashTable* hashTables;                                                  // Hash the tables using the G hash functions


    void writeResult(LSH_Set* result, Data_query* query, std::set<double>& true_nn);                                //given an ordered set,writes items to output path

  public :

    //H functions are constructed inside the LSH_solver constructor and picked by the G functions.
    LSH_solver(std::string dataset_path, std::string query_path, std::string output_filepath, int k, int l, int n, int r, double (*distanceFunction)(const std::vector<double>& a,const std::vector<double>& b) = EuclidianDistance);
    ~LSH_solver();

    bool solve();                                                               // This function is called to solve NN , kNN and Approximate Range Search.

    LSH_Set* NNandRS(Data_query *query);                                        // 1-NN , k-NN and Approximate Range Search, returns LSH_Set with nearest neighbors
    void printQueries() const;
};



std::vector<Data_item*> itemGenerator(int amount,int itemSize);


#endif

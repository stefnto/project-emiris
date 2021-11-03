#ifndef __CUBE__HPP__
#define __CUBE__HPP__

#include <list>
#include <unordered_map>
#include "utils.hpp"

class Vertex_point {
  private:
    unsigned long long binary_hash;                                             // string of bits represented as int  --- [ f_1( h_1(p) ) , ... , f_k( h_k(p) ) ], can store up to 64 bits
    std::unordered_map<int, int> set;                                           // first int represents h_i(p) and second int represents f_i( h_i(p) )
                                                                                // f_i( h_i(p) ) = { 0 , 1 } randomly generated
                                                                                // if h_k(p) = h_i(p), k > i, then f_k( h_k(p) ) = f_i( h_i(p) ), for the already randomly generated f_i( h_i(p) )

    std::vector<hFunction> hFunc;                                               // vector that holds each h_i for the specific point
  public:
    Vertex_point(){};
    Vertex_point(int k, int itemDim);
    void init(int k, int itemDim);
    ~Vertex_point(){};
    unsigned long long operator()(Data_item*, int);
    void bit_concat(int);
};


class Cube_HashTable {
  private:
    const int size;
    int k;
    hFunction *hFunc;

    std::list<Data_item*>* buckets;

  public:
    Cube_HashTable();
};


class Cube_Solver {
  private:
    Cube_HashTable* hashTable;
    int k;
    int m;
    int probes;
    int n;
    int r;
    std::vector<Data_item*> points_coordinates;
    std::vector<Data_item*> queries;

    std::string output_filepath;

  public:
    Cube_Solver(std::string dataset_path, std::string query_path, std::string output_file, int k, int m, int probes, int n, int r, double (*distanceFunction)(std::vector<int> a, std::vector<int> b) = EuclidianDistance);
    ~Cube_Solver();
    bool solve();
};


#endif

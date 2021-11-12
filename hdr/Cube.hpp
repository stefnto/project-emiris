#ifndef __CUBE__HPP__
#define __CUBE__HPP__

#include <list>
#include <fstream>
#include <unordered_map>
#include "utils.hpp"


using Cube_Set = std::set<Data_point *, bool (*)(const Data_point *a, const Data_point *b)>;


class Vertex_point {
  private:
    unsigned long long binary_hash;                                             // string of bits represented as int  --- [ f_1( h_1(p) ) , ... , f_k( h_k(p) ) ], can store up to 64 bits
  public:
    Vertex_point(){};
    Vertex_point(int itemDim);
    ~Vertex_point(){};

    void bit_concat(int);
    int getBH();

    template <typename T>
    unsigned long long operator()(T* item, std::vector<hFunction>& hFunc, std::unordered_map<int, int>*& sets){
      srand(time(NULL));

      int count = 0;
      for (hFunction h_i : hFunc){
        int tmp = h_i(item);                                                    // h_i(p)
        auto search = sets[count].find(tmp);                                    // search in the set for the specific h_i

        if ( search == sets[count].end() ){                                     // an h_i(p) with value 'tmp' doesn't exist
          int bit = mod( rand()*tmp, 2 );                                       // randomly generate 0 or 1
          sets[count].insert( {tmp, bit} );                                     // and then map it to the h_i(p) value
          bit_concat(bit);
        }
        else {                                                                  // h_i(p) with value 'tmp' exists
          bit_concat(search->second);                                           // concat the already generated f_i( h_i(p) ) to the binary_hash
        }

        count++;
      }
      return binary_hash;
    }

};


class Cube_HashTable: public HashTable {
  protected:
    int itemDim;
    int w;
    std::vector<Vertex_point> hcube_points;                                     // will be size of number of points from input
    std::list<Data_point*>* buckets;

    std::vector<hFunction> hFunc;                                               // vector that holds each h_i for the specific point, k h functions will be generated

    std::unordered_map<int, int>* sets;                                         // there are k unordered_maps, each for an h_i, i = 0, ... ,k
                                                                                // first int represents h_i(p) and second int represents f_i( h_i(p) )
                                                                                // f_i( h_i(p) ) = { 0 , 1 } randomly generated
                                                                                // if h_i(p_x) = h_i(p_y), j ≠ x, then f_k( h_k(p_x) ) = f_i( h_i(p_y) ), for the already randomly generated f_i( h_i(p_x) )

  public:
    Cube_HashTable(int k, int dim, unsigned long long buckets_no, int w);
    ~Cube_HashTable();

    template <typename T>
    void insertV_points(std::vector<T*>& points_coordinates){
      int counter = 0;
      for (int i = 0; i < points_coordinates.size(); i++){

        hcube_points.emplace_back(Vertex_point(this->itemDim));

        unsigned long long index = this->hcube_points[i](points_coordinates[i], this->hFunc, this->sets);

        this->buckets[index].emplace_back(points_coordinates[i]);
      }

    };

    void empty_buckets(int );

    Cube_Set* NN(Data_query* query, int m, int probes);                         // m is the numebr of NNs that will be checked for the query


};

class Cube_Solver: public Solver {
  private:
    int k;
    int m;
    int probes;
    std::vector<Data_point*> points_coordinates;
    std::vector<Data_query*> queries;
    Cube_HashTable* hashTable;

  public:
    Cube_Solver(std::string dataset_path, std::string query_path, std::string output_file, int k, int m, int probes, int n, int r, double (*distanceFunction)(const std::vector<int>& a, const std::vector<int>& b) = EuclidianDistance);
    ~Cube_Solver();
    bool solve();
    void writeResult(Cube_Set* result, Data_query* query, std::set<double>& true_nn);
    
};


#endif

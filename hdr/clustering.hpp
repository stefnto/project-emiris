
#ifndef __clustering__
#define __clustering__

#include <iostream>
#include "utils.hpp"
#include "LSH.hpp"
#include "Cube.hpp"


enum method {classic = 0,lsh,hypercube};

class clustering{
  private:
    int k_lsh, l_lsh;
    int n, r;                                                                   // number of NN and value r for rangeSearch
    int k_medians;                                                              //for k means++
    int m_cube, k_cube, probes_cube;
    std::string output_filepath;
    std::vector<clustering_data_item*> input_data;

    double (*distanceFunction)(const std::vector<int>& a,const std::vector<int>& b);
    void lloyd();
    float silhouette(centroid* centroids);
    void reverseAssignmentLSH();
    void reverseAssignmentCube();
    float minCentroidDistance(centroid* );
    centroid* initpp();
  public:
    clustering(std::string input_file, std::string output_file, int k_lsh, int l_lsh, int n, int r, int k_medians, int m_cube,
              int k_cube, int probes_cube, double (*distanceFunction)(const std::vector<int>& a,const std::vector<int>& b) = EuclidianDistance);
    ~clustering() = default;
    clustering(clustering&) = default;
    void solve(method m);



};

class Cube_Solver_Clustering: public Solver {
  private:
    int k;
    int m;
    int probes;

    Cube_HashTable* hashTable;

  public:
    Cube_Solver_Clustering(std::vector<clustering_data_item*>& clusteringData, int k, int m, int probes, int n, int r, double (*distanceFunction)(const std::vector<int>& a, const std::vector<int>& b) = EuclidianDistance);
    ~Cube_Solver_Clustering(){};
};

#endif

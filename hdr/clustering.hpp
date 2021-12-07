
#ifndef __clustering__
#define __clustering__

#include <iostream>
#include "utils.hpp"
#include "LSH.hpp"
#include "Cube.hpp"


enum method {classic = 0,lsh,hypercube};

using Centroid = std::vector<double>;                                              // for each centroid it's coordinates are stored

class Clustering_data_item;

void checkRadiusOfItem(Clustering_data_item* centroid, double radius, Clustering_data_item* point, int& sum);


class Clustering_data_item : public Data_point {
  private:
    int cluster;
    double radius = 0 ;
    double silhouette = 0;
    double distanceFromNearestCentroid = 0;

  public:
    Clustering_data_item(std::string line):Data_point(line),cluster(-1){}
    Clustering_data_item(std::string item_id, std::vector<double> coordinates): Data_point(item_id, coordinates), cluster(-1){}

    void setCluster(int cluster){this->cluster = cluster;}
    int getCluster() const {return cluster;}

    void setRadius(double radius) { this->radius = radius;}
    double getRadius() const  {return radius;}

    void setSilhouette(double silhouette){this->silhouette = silhouette;}
    double getSilhouette() const {return this->silhouette;}

    void setDistanceFromNearestCentroid(Clustering_data_item* item);
    void setDistanceFromNearestCentroid(double dist){this->distanceFromNearestCentroid = dist;}
    double getDistanceFromNearestCentroid() const {return this->distanceFromNearestCentroid;}


    double calculateDistance(Data_item *item) const {return distanceFunction(item->getCoordinates(), this->coordinates); }
    double calculateDistance(const Centroid &cent) const { return distanceFunction(coordinates, cent); }

    void findNearestCentroid(Centroid* centroids, int size);



};

class Clustering_Solver: public Solver {
  private:
    int k_lsh, l_lsh;
    int k_medians;                                                              //for k means++
    int m_cube, k_cube, probes_cube;
    int complete_flag;

    std::vector<Clustering_data_item*> input_data;

    double (*distanceFunction)(const std::vector<double>& a,const std::vector<double>& b);

    Centroid* initpp();

    void lloyd();
    void reverseAssignmentLSH();
    void reverseAssignmentCube();

    double silhouette(Centroid* centroids);

    double minCentroidDistance(Centroid* centroids);


  public:
    Clustering_Solver(std::string input_file, std::string output_filepath, int k_lsh, int l_lsh, int n, int r, int k_medians, int m_cube,
              int k_cube, int probes_cube, int complete_flag , double (*distanceFunction)(const std::vector<double>& a,const std::vector<double>& b) = EuclidianDistance);
    ~Clustering_Solver();
    Clustering_Solver(Clustering_Solver&) = default;
    void solve(method m);

    void writeResult(method m, Centroid* centroids);

};

class LSH_HashTable_Clustering: public LSH_HashTable {
  private:

  public:
    LSH_HashTable_Clustering(){};
    LSH_HashTable_Clustering(int itemDim, unsigned long long tableSize, int k, int w):
    LSH_HashTable(itemDim, tableSize, k, w){};
    ~LSH_HashTable_Clustering(){};

    void initHT(int itemDim, unsigned long long tableSize, int k, int w);

    int clusteringRangeSearch(Clustering_data_item* centroid, double radius,int index);

};

class Cube_HashTable_Clustering: public Cube_HashTable {
  private:

  public:
    Cube_HashTable_Clustering(int k, int dim, unsigned long long buckets_no, int w):
        Cube_HashTable(k, dim, buckets_no, w){};

    int clusteringRangeSearch(Clustering_data_item* centroid, double radius, int m, int probes);
};


class LSH_Solver_Clustering: public Solver {
  private:
    int k;
    int l;
    int w;

    LSH_HashTable_Clustering* hashTables;

  public:
    LSH_Solver_Clustering(std::vector<Clustering_data_item *>& clusteringData, int k, int l, int n, int r, double (*distanceFunction)(const std::vector<double>& a, const std::vector<double>& b) = EuclidianDistance);
    ~LSH_Solver_Clustering();

    int clusteringRangeSearch(Clustering_data_item* centroid, double radius);               // this function is used to rangeSearch for points near the centroid given

};

class Cube_Solver_Clustering: public Solver {
  private:
    int k;
    int m;
    int probes;

    Cube_HashTable_Clustering* hashTable;

  public:
    Cube_Solver_Clustering(std::vector<Clustering_data_item*>& clusteringData, int k, int m, int probes, int n, int r, double (*distanceFunction)(const std::vector<double>& a, const std::vector<double>& b) = EuclidianDistance);
    ~Cube_Solver_Clustering();

    int clusteringRangeSearch(Clustering_data_item* centroid, double radius);               // this function is used to rangeSearch for points near the centroid given
};




#endif

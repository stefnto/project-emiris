#ifndef __cclustering__
#define __cclustering__

#include <iostream>
#include "utils.hpp"
#include "Curves.hpp"
#include "clustering.hpp"

// enum method {classic = 0, lsh, hypercube };

enum updateMethods {meanFrechet = 0, meanVector};

enum assignmentMethods {lloydCurve, lloydV , lshV, hypercubeV, lsh_frechet};

using Centroid = std::vector<double>; // for each centroid it's coordinates are stored



class Clustering_curve_data_item : public Frechet_point{
private:
    int cluster;
    double radius = 0;
    double silhouette = 0;
    double distanceFromNearestCentroid = 0;

public:
    Clustering_curve_data_item(): Frechet_point() {}
    Clustering_curve_data_item(std::string line) : Frechet_point(line), cluster(-1) {}
    Clustering_curve_data_item(std::string id,const std::vector<double>& c) : Frechet_point(id,c),cluster(-1){}

    void setCluster(int cluster) { this->cluster = cluster; }
    int getCluster() const { return cluster; }

    void setRadius(double radius) { this->radius = radius; }
    double getRadius() const { return radius; }

    void setSilhouette(double silhouette) { this->silhouette = silhouette; }
    double getSilhouette() const { return this->silhouette; }

    void setDistanceFromNearestCentroid(Clustering_curve_data_item *item);
    void setDistanceFromNearestCentroid(double dist) { this->distanceFromNearestCentroid = dist; }
    double getDistanceFromNearestCentroid() const { return this->distanceFromNearestCentroid; }

    double& operator[](size_t pos){ return this->refAt(pos);}

    double calculateDistance(Data_item *item) const { return distanceFunction(item->getCoordinates(), this->coordinates); }
    double calculateDistance(const Centroid &cent) const { return distanceFunction(coordinates, cent); }

    void findNearestCentroid(Centroid *centroids, int size);
};


class Clustering_curve_solver : public Solver{
  private:
    int k_lsh, l_lsh;
    int k_medians;                                                              // for k means++
    int m_cube, k_cube, probes_cube;
    int complete_flag;

    updateMethods updateStep;

    assignmentMethods assignmentStep;

    std::vector<std::vector<Clustering_curve_data_item*>> clusters;             //vector that contains elements of each cluster

    std::vector<Clustering_curve_data_item *> input_data;

    double (*distanceFunction)(const Frechet_point* a,const Frechet_point* b);

    std::vector<Clustering_curve_data_item>* initpp();

    void genericClustering();

    int lloydAssignment(std::vector<Clustering_curve_data_item> *centroids,std::vector<Clustering_curve_data_item>* nextCentroids); // returns number of changes made to the clusters

    void meanFrechetUpdate(std::vector<Clustering_curve_data_item>* nextCentroids);
    /*
    void reverseAssignmentLSH();
    void reverseAssignmentCube();
    double silhouette(Centroid *centroids);
    double minCentroidDistance(Centroid *centroids);
 */
  public:
    Clustering_curve_solver(std::string input_file, std::string output_filepath, int k_lsh, int l_lsh, int n, int r, int k_medians, int m_cube, int k_cube, int probes_cube, int complete_flag, updateMethods um, assignmentMethods am);
    ~Clustering_curve_solver() = default;

    Clustering_curve_solver(Clustering_curve_solver &) = default;

    void solve();

    void writeResult(updateMethods um, assignmentMethods am, std::vector<Clustering_curve_data_item> *centroids, std::vector<std::vector<Clustering_curve_data_item*>> clusters);
};


class Frechet_Clustering_HashTable: public HashTable {
  protected:
    gFunction hashingFunction;
    std::list<Data_point*>* buckets;                                            // Array of Lists Aka Hash Table;
    snapping* snapper;
    size_t itemDim;

    template <typename T>
    void padding(T* item, double num = 0){
      if (item->get_altered_coordinates_size() > this->itemDim){
          std::cout << "item->altered_coordinates.size() > this->itemDim" << std::endl;
          exit(0);
      }
      while (item->get_altered_coordinates_size() < this->itemDim) item->altered_coordinates_emlace_back(num);
    }



  public:
    Frechet_Clustering_HashTable() = default;
    Frechet_Clustering_HashTable(int itemDim, unsigned long long tableSize, int k, int w,snapping* snapper);     // Constructs H and G functions;
    virtual ~Frechet_Clustering_HashTable();

    void init(size_t itemDim, size_t tableSize, int k, int w, snapping *snapper);
    int clusteringRangeSearch(Clustering_curve_data_item *centroid, double radius, int index);
};


class Frechet_Clustering_solver: public Solver {                                              // Class is used for both Discrete and Continuous Frechet
  private:
    const int l;                                                                              // number of HashTables, in Continuous l = 1
    int w;

    std::vector<Frechet_point*> points_coordinates;                                           //Here we store the elements read from the file
    std::vector<Frechet_query*> queries;
    Frechet_Clustering_HashTable* hashTables;                                                 // Hash the tables using the G hash functions


    void writeResult(LSH_Set* result, Data_query* query, std::set<double>& true_nn);                                //given an ordered set,writes items to output path

  public :

    Frechet_Clustering_solver(std::vector<Clustering_curve_data_item *> &clusteringData, int k, int l, int n, int r, double (*distanceFunction)(const std::vector<double> &a, const std::vector<double> &b) = EuclidianDistance);

    ~Frechet_Clustering_solver();
    int clusteringRangeSearch(Clustering_curve_data_item *centroid, double radius);

    void printQueries() const;
};


void checkRadiusOfItem(Clustering_curve_data_item *centroid, double radius, Clustering_curve_data_item *point, int &sum);


Clustering_curve_data_item* meanFrechetCurve(Clustering_curve_data_item* item1, Clustering_curve_data_item* item2);


double discreteFrechetBT(const Frechet_point *a, const Frechet_point *b, std::vector<std::vector<std::pair<size_t,size_t>>>& bt);


double discreteFrechetRecursionBT(const Frechet_point *a, const Frechet_point *b, int i, int j, std::vector<std::vector<double>>&maxLength, std::vector<std::vector<std::pair<size_t,size_t>>>& bt/* vector2d &maxLength */);

#endif

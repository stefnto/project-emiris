#ifndef __UTILS__HPP__
#define __UTILS__HPP__

#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <chrono>
#include <fstream>
#include <set>

class item_Exception{};

using centroid = std::vector<int>;

double EuclidianDistance(const std::vector<int>& a,const std::vector<int>& b);

class Solver {
  protected:
    int n;
    int r;
    std::string output_filepath;

  public:
    Solver(int n, int r, std::string output_filepath, double (*distanceFunction)(const std::vector<int>& a, const std::vector<int>& b) = EuclidianDistance);
    Solver(int n, int r, double (*distanceFunction)(const std::vector<int>& a, const std::vector<int>& b) = EuclidianDistance);
    virtual ~Solver(){};
};

class HashTable {
  protected:
    int k;
    unsigned long long size;

  public:
    HashTable(int k, int size) :k(k), size(size){};
    virtual ~HashTable(){};
};

class Data_item{
    private:
        std::string name;                                                       // is id from input_file
        std::vector<int> coordinates;
        long ID;                                                                // id computed from (Σ r * h) mod M
        double distanceFromQuery = 0;                                           // distance of item from each query checked at a time
        double algorithmTime = 0;
        double bruteforceTime = 0;
        double shorterDistance = 0;                                             // used in brute force method

        static double (*distanceFunction)(const std::vector<int> &a, const std::vector<int> &b);

    public:
        Data_item(){};
        Data_item(std::string item_id, std::vector<int> coordinates);
        Data_item(std::string line);
        virtual ~Data_item() = default;
        Data_item(Data_item &) = default;
        void set_id(long id);
        long get_id() const;

        const std::vector<int> &getCoordinates() const;
        void print_coordinates();
        int get_coordinates_size();
        void setDistanceFromQuery(Data_item *query);
        void setDistanceFromQuery(float dist);
        float getDistanceFromQuery() const;
        std::string getItemID() const;
        void setAlgorithmTime(double time);
        void setBruteForcetime(double time);
        void setShorterDistance(double value);
        double getAlgorithmTime();
        double getBruteForceTime();
        double getShorterDistance();
        std::string getName() const;

        float calculateDistance(Data_item *item) const {return distanceFunction(item->coordinates, coordinates); }
        float calculateDistance(const centroid &cent) const { return distanceFunction(coordinates, cent); }
        static void setDistanceFunction(double (*distanceFunction)(const std::vector<int> &a, const std::vector<int> &b));
        double (*getDistanceFunction())(const std::vector<int>& a,const std::vector<int>& b);
};

class clustering_data_item : public Data_item {
    public:
        clustering_data_item(std::string line):Data_item(line),cluster(-1){}
        void setCluster(int cluster){this->cluster = cluster;}
        int getCluster() const {return cluster;}
        float getRadius() const  {return radius;}
        void setRadius(float radius) { this->radius = radius;}
        void findNearestCentroid(centroid* centroids,int size);
        void setSilhouette(float silhouette){this->silhouette = silhouette;}
        float getSilhouette() const {return this->silhouette;}

    private:
        int cluster;
        float radius= 0 ;
        float silhouette = 0;
};


class hFunction{                                                                // floor( (p*v + t)/ w )

    private:
        std::vector<float> v;                                                   //contains vector V
        float t;
        const int w;

    public:
        hFunction(int itemSize,int w = 4);
        int operator()(const Data_item*);
        std::vector<float>& getv();
};

long mod(long x, long y);



float minDist(centroid* centroids,int size);

template <typename T>
int readItems(std::string dataset_path, std::vector<T *> &container){
    std::ifstream datafile;
    int counter = 0;
    datafile.open(dataset_path);
    if (datafile.is_open())
    {
        std::string line;
        double sttime, endtime; // to compute total run time
        sttime = ((double)clock()) / CLOCKS_PER_SEC;
        while (getline(datafile, line)){
            counter++;
            container.emplace_back(new T(line)); // creates a 'Data_item' and puts it at the end of the vector 'points_coordinates'
        }
        endtime = ((double)clock()) / CLOCKS_PER_SEC;
        datafile.close();
        std::cout << "time needed to read elements : " << endtime - sttime << std::endl;
    }
    return counter;
}

    int rGenerator();

void getNumbersWithHammingDistance(int k, unsigned long long number, int probes, std::set<unsigned long long>& set);

void bruteForceSearch(Data_item *item, std::vector<Data_item*>& points_coordinates, int n, std::set<double>& true_nn_distances);

void checkRadiusOfItem(Data_item* centroid, float radius, clustering_data_item* c_d_item, int& sum);

template<typename T>
int avgDistance(std::vector<T*>& points_coordinates){
  srand(time(NULL));
  std::set<int> set;
  double dist = 0;
  int num = ( 5 * points_coordinates.size() ) / 1000;                           // for 1000 points check 5 vectors to find mean distance
                                                                                // so for y points in the data set, x = ( 5 * y ) / 1000 number of vectors will be checked for mean distance
  for (int i = 0; i < num; i++){
    int tmp = rand() % points_coordinates.size();

    auto search = set.find(tmp);
    if (search == set.end()){
      set.insert(tmp);
    }

    for (std::set<int>::iterator it = std::next(set.begin(), 1); it!=set.end(); ++it){  // compute each distance of first point to the other 'set.size() - 1' points
      dist += EuclidianDistance(points_coordinates[*set.begin()]->getCoordinates(), points_coordinates[*it]->getCoordinates());
    }
    dist = dist / set.size();                                                   // get the mean distance
  }
  return dist;
}

void readConfig(std::string config_file, int& k_lsh, int& l_lsh, int& k_medians, int& m_cube, int& k_cube, int& probes_cube);
#endif

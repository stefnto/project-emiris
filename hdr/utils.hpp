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
        ~Data_item() = default;
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

        float calculateDistance(Data_item *item) const { return distanceFunction(item->getCoordinates(), coordinates); }
        float calculateDistance(const centroid &cent) const { return distanceFunction(coordinates, cent); }
        static void setDistanceFunction(double (*distanceFunction)(const std::vector<int> &a, const std::vector<int> &b));
        double (*getDistanceFunction())(const std::vector<int>& a,const std::vector<int>& b);
};

class clustering_data_item : public Data_item {
    public:
        clustering_data_item(std::string line):Data_item(line),cluster(-1){}
        void setCluster(int cluster){
            this->cluster = cluster;
        };
        int getCluster() const {return cluster;}
        void setDistance1st(float dist){this->dist1 = dist;}
        void setDistance2nd(float dist){this->dist2 = dist;}
        float getDistance1st() const {  return this->dist1;}
        float getDistance2nd() const { return this->dist2;}
        void setRadius(float rad){this->radius = radius; radiusIsSet = true;}
        float getRadius() const { return this->radius;}
        bool isSetRadius() const { return radiusIsSet;}
        void unsetRadius() { radiusIsSet = false;}
        void findNearestCentroid(centroid* centroids,int size);
        void setSilhouette(float silhouette){this->silhouette = silhouette;}
        float getSilhouette() const {return this->silhouette;}

    private:
        int cluster;
        float radius;                   //what was the radius of the range-search when the cluster was assigned 
        float dist1;                    //distance from first closest cluster
        float dist2;                    //distance from second closest cluster
        bool radiusIsSet = false;
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

double EuclidianDistance(const std::vector<int>& a,const std::vector<int>& b);
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

int avgDistance(std::vector<Data_item*>& points_coordinates);
#endif

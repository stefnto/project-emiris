#ifndef __UTILS__HPP__
#define __UTILS__HPP__

#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <chrono>
#include <fstream>

class item_Exception{};

class Data_item{
    private:
        std::string name;                                                       // is id from input_file
        std::vector<int> coordinates;
        long ID;                                                                // id computed from (Σ r * h) mod M
        double distanceFromQuery = 0;

        static double (*distanceFunction)(std::vector<int> a,std::vector<int> b);
    public:
        Data_item(){};
        Data_item(std::string item_id, std::vector<int> coordinates);
        Data_item(std::string line);
        virtual ~Data_item() = default;
        Data_item(Data_item&) = default;
        void set_id(long id);
        long get_id() const;

        const std::vector<int>& getCoordinates() const;
        void print_coordinates();
        int get_coordinates_size();
        void setDistanceFromQuery(Data_item* query);
        float getDistanceFromQuery() const;
        std::string getName() const;

        static void setDistanceFunction(double (*distanceFunction)(std::vector<int> a, std::vector<int>b) );
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
    private:
        int cluster;
        float dist1;                    //distance from first closest cluster
        float dist2;                    //distance from second closest cluster
       
};


class hFunction{                            // floor( (p*v + t)/ w )

    private:
        std::vector<float> v;               //contains vector V
        float t;
        const int w;

    public:
        hFunction(int itemSize,int w = 4);
        int operator()(const Data_item*);
};

long mod(long x, long y);

double EuclidianDistance(std::vector<int> a, std::vector<int> b);
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

#endif

#ifndef __UTILS__HPP__
#define __UTILS__HPP__

#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <chrono>
#include <set>

class item_Exception{};

class Data_item{
    private:
        std::string item_id;                                                    // is id from input_file
        std::vector<int> coordinates;
        long ID;                                                                // id computed from (Σ r * h) mod M
        double distanceFromQuery = 0;                                           // distance of item from each query checked at a time
        double algorithmTime = 0;
        double bruteforceTime = 0;
        double shorterDistance = 0;                                             // used in brute force method

        static double (*distanceFunction)(std::vector<int> a,std::vector<int> b);
    public:
        Data_item(){};
        Data_item(std::string item_id, std::vector<int> coordinates);
        Data_item(std::string line);
        ~Data_item() = default;
        Data_item(Data_item&) = default;
        void set_id(long id);
        long get_id() const;

        const std::vector<int>& getCoordinates() const;
        void print_coordinates();
        int get_coordinates_size();
        void setDistanceFromQuery(Data_item* query);
        float getDistanceFromQuery() const;
        std::string getItemID() const;
        void setAlgorithmTime(double time);
        void setBruteForcetime(double time);
        void setShorterDistance(double value);
        double getAlgorithmTime();
        double getBruteForceTime();
        double getShorterDistance();

        static void setDistanceFunction(double (*distanceFunction)(std::vector<int> a, std::vector<int>b) );
        double (*getDistanceFunction())(std::vector<int> a,std::vector<int> b);
};


class hFunction{                                                                // floor( (p*v + t)/ w )

    private:
        std::vector<float> v;                                                   //contains vector V
        float t;
        const int w;

    public:
        hFunction(int itemSize,int w = 4);
        int operator()(const Data_item*);
};

long mod(long x, long y);

double EuclidianDistance(std::vector<int> a, std::vector<int> b);

int rGenerator();

void getNumbersWithHammingDistance(int k, unsigned long long number, int probes, std::set<unsigned long long>& set);

void bruteForceSearch(Data_item *item, std::vector<Data_item*>& points_coordinates, int n, std::set<double>& true_nn_distances);

int avgDistance(std::vector<Data_item*>& points_coordinates);
#endif

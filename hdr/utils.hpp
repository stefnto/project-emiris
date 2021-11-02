#ifndef __LSH__UTILS__
#define __LSH__UTILS__

#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <chrono>

class Exception{};

class Data_item{
    private:
        std::string item_id;                                                    // is id from input_file
        std::vector<int> coordinates;
        long ID;                                                                // id computed from (Σ r * h) mod M
        double distanceFromQuery = 0;

        static double (*distanceFunction)(std::vector<int> a,std::vector<int> b);
    public:

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

        static void setDistanceFunction(double (*distanceFunction)(std::vector<int> a, std::vector<int>b) );
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

int rGenerator();

#endif


#ifndef __clustering__
#define __clustering__

#include <iostream>
#include <LSH.hpp>
#include <utils.hpp>


enum method {classic = 0,lsh,hypercube};

class clustering{
    public:
        clustering(std::string input_file, std::string config_file, std::string output_file, char method, double (*distanceFunction)(const std::vector<int>& a,const std::vector<int>& b) = EuclidianDistance);
        ~clustering() = default;
        clustering(clustering&) = default;
        void solve();
    private:
        std::string output_file;
        std::vector<clustering_data_item*> input_data;
        int k = 10;                                                                             //for k means++
        double (*distanceFunction)(const std::vector<int>& a,const std::vector<int>& b);
        void lloyd();
        float silhouette(centroid* centroids);
        void reverseAssignment();
        float minCentroidDistance(centroid* );




        centroid* initpp();


};

#endif
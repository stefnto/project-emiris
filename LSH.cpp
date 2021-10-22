#include "LSH.hpp"

//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path, int k = 4, int L = 5, int N = 1, int R = 10000, double (*distanceFunction)(std::vector<double> a, std::vector<double> b) = EuclidianDistance){

}

bool LSH_solver::solve(std::string query_path, std::string output_path){

}

//LSH_item Methods;

LSH_item::LSH_item(std::string item_id, std::vector<double> coordinates){

}

void LSH_item::set_id(int ID){

}

//LSH_HashTable Methods

LSH_HashTable::LSH_HashTable(int size, int k) : size(size), k(k){

    //Create k h functions
}

void LSH_HashTable::insert(LSH_item){

}

//gFunction Methods

gFunction::gFunction(){

}

int gFunction::operator()(LSH_item&){

}

//general methods

double EuclidianDistance(std::vector<double> a , std::vector<double> b){

}

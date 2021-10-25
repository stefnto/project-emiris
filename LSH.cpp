#include "LSH.hpp"

//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path, int k = 4, int L = 5, int N = 1, int R = 10000, double (*distanceFunction)(std::vector<int> a, std::vector<int> b) = EuclidianDistance){

}

bool LSH_solver::solve(std::string query_path, std::string output_path){

}

//LSH_item Methods;

LSH_item::LSH_item(std::string item_id, std::vector<int> coordinates){

}

void LSH_item::set_id(int ID){

}

//LSH_HashTable Methods

LSH_HashTable::LSH_HashTable(int size, int k) : size(size), k(k){

    //Create k h functions
}

LSH_HashTable::~LSH_HashTable(){

    delete[] this->buckets;

}


void LSH_HashTable::insert(LSH_item){

}

//gFunction Methods

gFunction::gFunction(int itemSize){
    // for (int i = 0 ; i < itemSize; i++) linearCombinationElements.push_back(std::pair())

}

int gFunction::operator()(LSH_item&){

}

//hFunction Methods

hFunction::hFunction(int itemSize):w(4){
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0,w*1.0);

    t = distribution(generator);

    std::normal_distribution<float> distributionN(0.0,4.5);

    for (int i = 0 ; i < itemSize; i++) v.push_back(distributionN(generator));

}

//general methods

double EuclidianDistance(std::vector<double> a , std::vector<double> b){
        if (a.size() != b.size()) throw LSH_Exception();

        double sum = 0;

        for (int i = 0 ; i < a.size(); i++){
            sum += pow((a.at(i) - b.at(i)),2); 
        }

        return sqrt(sum);

}

#include "LSH.hpp"

//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path, int k , int L , int N , int R , double (*distanceFunction)(std::vector<int> a, std::vector<int> b) ){

}

bool LSH_solver::solve(std::string query_path, std::string output_path){
    return true;
}

//LSH_item Methods;

LSH_item::LSH_item(std::string item_id, std::vector<int> coordinates){

}

void LSH_item::set_id(int ID){

}

const std::vector<int>& LSH_item::getCoordinates() const {
    return this->coordinates;
}

    //LSH_HashTable Methods

LSH_HashTable::LSH_HashTable(int size, int k) : size(size), k(k){
    this->hashingFunction = gFunction(10,k,size);
}

LSH_HashTable::~LSH_HashTable(){

    delete[] this->buckets;

}


void LSH_HashTable::insert(LSH_item item){

}

//gFunction Methods

gFunction::gFunction(int itemDim,int k,int tableSize): k(k), tableSize(tableSize) {
    for (int i = 0 ; i < k; i++) linearCombinationElements.push_back(std::pair<int,hFunction>(rGenerator(),hFunction(itemDim)));

}

int gFunction::operator()(const LSH_item& item){

    long M = 0xFFFFFFFF - 4;
    long sum = 0 ;

    for (std::pair<int,hFunction> elem : linearCombinationElements) sum +=( elem.first*elem.second(item) )%M;
    sum %= M;
        

    return sum % this->tableSize;
    

}

//hFunction Methods

hFunction::hFunction(int itemSize):w(4){
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0,w*1.0);

    t = distribution(generator);

    std::normal_distribution<float> distributionN(0.0,1.0);

    for (int i = 0 ; i < itemSize; i++) v.push_back(distributionN(generator));

}

int hFunction::operator()(const LSH_item& item){

    std::vector<int>::const_iterator it1 = item.getCoordinates().begin();
    std::vector<float>::const_iterator it2 = v.begin();

    if( item.getCoordinates().size() != v.size() ) throw LSH_Exception();

    float sum = 0;
    while ( it2 != v.end() ){
        sum += (*it1)*(*it2);
        it1++;
        it2++;
    }

    sum /= this->w;

    return sum;

}

//general methods


#include "LSH.hpp"
#include <sstream>

//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path, int k , int L , int N , int R , double (*distanceFunction)(std::vector<int> a, std::vector<int> b) ){

}

bool LSH_solver::solve(std::string query_path, std::string output_path){
    return true;
}

//LSH_item Methods;

LSH_item::LSH_item(std::string item_id, std::vector<int> coordinates):item_id(item_id),coordinates(coordinates){}

void LSH_item::set_id(int ID){

    this->item_id = ID;
}
LSH_item::LSH_item(std::string line){
  std::stringstream ss(line);
  int number;
  while (ss >> number){
    this->coordinates.push_back(number);           // push each coordinate
  }
}
  const std::vector<int>& LSH_item::getCoordinates() const {
    return this->coordinates;
}

    //LSH_HashTable Methods

LSH_HashTable::LSH_HashTable(int itemDim, int k,int tableSize) : size(tableSize), k(k),hashingFunction(itemDim,k,tableSize){

    this->buckets = new std::list<LSH_item*>[tableSize];
}

LSH_HashTable::~LSH_HashTable(){
  delete[] this->buckets;
}

void LSH_item::print_coordinates(){
  for (int i = 0; i < this->coordinates.size(); i++){
    std::cout << this->coordinates[i] << " " ;
  }
  std::cout << std::endl;
}

int LSH_item::get_coordinates_size(){
  return this->coordinates.size();
}

//LSH_HashTable Methods

 


void LSH_HashTable::insert(LSH_item* item){

    int index = this->hashingFunction(*item);

    this->buckets[index].push_back(item);

}

//gFunction Methods

gFunction::gFunction(int itemDim,int k,int tableSize): k(k), tableSize(tableSize) {
    for (int i = 0 ; i < k; i++) linearCombinationElements.push_back(std::pair<int,hFunction>(rGenerator(),hFunction(itemDim)));

}

int gFunction::operator()(LSH_item& item){

    long M = 0xFFFFFFFF - 4;
    long sum = 0 ;

    for (std::pair<int,hFunction> elem : linearCombinationElements) sum +=( elem.first*elem.second(item) )%M;
    sum %= M;
    
    item.set_id(sum);   //setting id of the item 

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

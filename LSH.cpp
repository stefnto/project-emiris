#include "LSH.hpp"
#include <sstream>
#include <fstream>

//LSH_solver Methods

LSH_solver::LSH_solver(std::string input_file, int k , int L , int N , int R , double (*distanceFunction)(std::vector<int> a, std::vector<int> b) ){
  std::ifstream datafile;
  datafile.open(input_file);
  if (datafile.is_open()){
    std::string line;
    while (getline(datafile, line)){
      this->points_coordinates.emplace_back(line);                                    // creates a 'LSH_item' and puts it at the end of the vector 'points_coordinates'
    }
    datafile.close();
  }
  this->points_coordinates[0].print_coordinates();
  std::cout << "points_coordinates size = " << this->points_coordinates[0].get_coordinates_size() << std::endl;
  std::cout << "points_coordinates size = " << this->points_coordinates[points_coordinates.size() - 1].get_coordinates_size() << std::endl;
  this->points_coordinates[points_coordinates.size() - 1].print_coordinates();
  std::cout << "size = " << points_coordinates.size() << std::endl;
}

bool LSH_solver::solve(std::string query_path, std::string output_path){
    return true;
}

//LSH_item Methods;

// LSH_item::LSH_item(std::string item_id, std::vector<int> coordinates):item_id(item_id),coordinates(coordinates){}

void LSH_item::set_id(long id){

    this->item_id = id;
}

long LSH_item::get_id(){
  return this->item_id;
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

LSH_HashTable::LSH_HashTable(int itemDim, int k,int tableSize) : size(tableSize), k(k),hashingFunction(itemDim,k,tableSize){

    this->buckets = new std::list<LSH_item*>[tableSize];
}


LSH_HashTable::~LSH_HashTable(){
  delete[] this->buckets;
}


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

    for (std::pair<int,hFunction> elem : linearCombinationElements) {
      sum += mod((elem.first * elem.second(item)), M);
    }
    sum = mod(sum, M);

    item.set_id(sum);   //setting id of the item
    // std::cout << "id = " << item.get_id() << std::endl;
    return sum % this->tableSize;


}

//hFunction Methods

hFunction::hFunction(int itemSize):w(4){
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> distribution(0.0,w*1.0);

    t = distribution(generator);

    std::normal_distribution<float> distributionN(0.0,1.0);

    for (int i = 0 ; i < itemSize; i++)
      v.push_back(distributionN(generator));

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

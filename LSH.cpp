#include "LSH.hpp"
#include <sstream>

//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path, int k, int , int l, int r, double (*distanceFunction)(std::vector<double> a, std::vector<double> b)){

}

bool LSH_solver::solve(std::string query_path, std::string output_path){
  return true;
}

//LSH_item Methods;

LSH_item::LSH_item(std::string line){
  std::stringstream ss(line);
  int number;
  while (ss >> number){
    this->coordinates.push_back(number);           // push each coordinate
  }

}

void LSH_item::set_id(int ID){

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

LSH_HashTable::LSH_HashTable(int size, int k) : size(size), k(k){

    //Create k h functions
}

void LSH_HashTable::insert(LSH_item){

}

//gFunction Methods

gFunction::gFunction(int itemSize){
    // for (int i = 0 ; i < itemSize; i++) linearCombinationElements.push_back(std::pair())

}

int gFunction::operator()(LSH_item&){
  return 0;
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
            //sum += pow((a.at(i) - b.at(i)),k);                    // initialize k for other metrics
        }

        return sqrt(sum);
        // return pow(sum, 1/k)                                     // return when using other metrics

}

#include "utils.hpp"
#include <sstream>


//Data_item Methods

double (*Data_item::distanceFunction)(std::vector<int> a,std::vector<int> b ) = nullptr;

Data_item::Data_item(std::string name, std::vector<int> coordinates):name(name),coordinates(coordinates){}

void Data_item::set_id(long id){

    this->ID = id;
}

long Data_item::get_id() const{
  return this->ID;
}

Data_item::Data_item(std::string line){
  std::stringstream ss(line);
  int number;
  std::string itmID;
  ss >> itmID;
  this->name = itmID;
  while (ss >> number){
    this->coordinates.emplace_back(number);           // push each coordinate
  }
}

void Data_item::setDistanceFromQuery(Data_item* query){
  this->distanceFromQuery = Data_item::distanceFunction(this->coordinates,query->coordinates);
}

float Data_item::getDistanceFromQuery() const{
  return this->distanceFromQuery;
}

void Data_item::setDistanceFunction(double (*dFunction)(std::vector<int> a, std::vector<int> b)){
      distanceFunction = dFunction;
}

std::string Data_item::getName()const{return this->name;}


void Data_item::print_coordinates(){
  for (int i = 0; i < this->coordinates.size(); i++){
    std::cout << this->coordinates[i] << " " ;
  }
  std::cout << std::endl;
}

int Data_item::get_coordinates_size(){
  return this->coordinates.size();
}

const std::vector<int>& Data_item::getCoordinates() const {
  return this->coordinates;
}


//hFunction Methods

hFunction::hFunction(int itemSize,int w):w(33){
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine generator(seed);
  std::uniform_real_distribution<float> distribution(0.0, w * 1.0);

  t = distribution(generator);

  std::normal_distribution<float> distributionN(0.0, 1.0);

  for (int i = 0; i < itemSize; i++) v.push_back(distributionN(generator));
}

int hFunction::operator()(const Data_item* item){
    std::vector<int>::const_iterator it1 = item->getCoordinates().begin();
    std::vector<float>::const_iterator it2 = v.begin();

    if( item->getCoordinates().size() != v.size() ) throw item_Exception();

    float sum = t;
    while ( it2 != v.end() ){
        sum += (*it1)*(*it2);
        it1++;
        it2++;
    }
    sum /= this->w;

    return sum;

}

//          General functions

// a % n = a – ( n * trunc( a/n ) ).

 long mod(long x, long y){
    if (x < 0) {
        long modulo = (-x) % y;
        return modulo == 0 ? 0 : y - modulo ;
    }
    return x % y;
}

 double EuclidianDistance(std::vector<int> a, std::vector<int> b){
    if (a.size() != b.size()) return 0;

    double sum = 0;

    for (int i = 0; i < a.size(); i++)
    {
        sum += pow((a.at(i) - b.at(i)), 2);
    }

    return sqrt(sum);
}

 int rGenerator(){


      return rand() % 2000 - 999;

}

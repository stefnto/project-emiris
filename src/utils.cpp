#include "utils.hpp"
#include <sstream>


//Data_item Methods

double (*Data_item::distanceFunction)(std::vector<int> a,std::vector<int> b ) = nullptr;

Data_item::Data_item(std::string item_id, std::vector<int> coordinates):item_id(item_id),coordinates(coordinates){}

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
  this->item_id = itmID;
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

std::string Data_item::getItemID()const{return this->item_id;}


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

double Data_item::getAlgorithmTime(){
  return this->algorithmTime;
}

double Data_item::getBruteForceTime(){
  return this->bruteforceTime;
}

void Data_item::setAlgorithmTime(double time){
  this->algorithmTime = time;
}

void Data_item::setBruteForcetime(double time){
  this->bruteforceTime = time;
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

// fills the referenced set with numbers that have 'hamming_distance' from 'number' that was given
void getNumbersWithHammingDistance(int k, unsigned long long number, int hamming_distance, std::set<unsigned long long>& set){
  //set.clear()

  for (int i = 0; i < k; i++){
    // cout << i+1 << " enapalhpsh" << endl;
    unsigned long long mask = 1 << i;

    if (hamming_distance > 1){
      // cout <<"klhsh ksana"<< endl;
      // cout << (number & ~mask) << endl;
      // cout << std::bitset<9>(number & ~mask) << endl;
      getNumbersWithHammingDistance(k, (number & ~mask), hamming_distance-1, set);

      continue;
    }

    else if (hamming_distance == 1)

      // cout <<"    " << std::bitset<9>(~mask) << endl;
      // cout <<"    " << std::bitset<9>(number) << endl;
      // cout <<"-------------- (AND)" << endl;
      //
      // cout <<"    " << std::bitset<9>(number & ~mask) << " = " << (number & ~mask) << endl;
      set.insert(number & ~mask);

  }

  // cout <<"set size = " << set.size() << endl;
  // for (auto it = set.begin(); it !=
  //                              set.end(); ++it)
  //         cout << ' ' << *it;
}

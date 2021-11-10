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

double (* Data_item::getDistanceFunction())(std::vector<int> a,std::vector<int> b){
  return this->distanceFunction;
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

void Data_item::setShorterDistance(double value){
  this->shorterDistance = value;
}


double Data_item::getShorterDistance(){
  return this->shorterDistance;
}

//hFunction Methods

hFunction::hFunction(int itemSize,int w):w(w){

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine generator(seed);
  std::uniform_real_distribution<float> distribution(0.0, w * 1.0);

  t = distribution(generator);

  std::normal_distribution<float> distributionN(0.0, 1.0);

  for (int i = 0; i < itemSize; i++)
    v.push_back(distributionN(generator));
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

std::vector<float>& hFunction::getv(){
  return this->v;
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
void getNumbersWithHammingDistance(int k, unsigned long long number, int probes, std::set<unsigned long long>& set){

  for (int i = 0; i < k; i++){
    unsigned long long mask = 1 << i;

    int hamming_distance = 1 + ((probes-1) / k);                                // cieling of probes/k

    if (hamming_distance > 1){
      getNumbersWithHammingDistance(k, (number & ~mask), hamming_distance-1, set);

      continue;
    }

    else if (hamming_distance == 1)
      set.insert(number & ~mask);

  }
}




void bruteForceSearch(Data_item *query, std::vector<Data_item*>& points_coordinates, int n, std::set<double>& true_nn_distances){
  double sttime, endtime;                                                       // to compute total run time

  sttime=((double) clock())/CLOCKS_PER_SEC;
  for (Data_item* point: points_coordinates){
    double (* ptr) (std::vector<int> a,std::vector<int> b) = query->getDistanceFunction();
    double distance = ptr(query->getCoordinates(), point->getCoordinates());

    if (true_nn_distances.size() < n){                                          // if size of set less than n, add the distance
      true_nn_distances.insert(distance);
    }
    else if (true_nn_distances.size() >= n){                                    // if distance is less than biggest distance in set
      if (distance < *true_nn_distances.rbegin() ){
        true_nn_distances.erase( *true_nn_distances.rbegin() );                 // erase the biggest distance that is saved on set
        true_nn_distances.insert(distance);                                     // insert new distance
      }
    }
  }

  endtime=((double) clock())/CLOCKS_PER_SEC;
  query->setBruteForcetime( endtime - sttime);
}


int avgDistance(std::vector<Data_item*>& points_coordinates){
  srand(time(NULL));
  std::set<int> set;
  double dist = 0;
  int num = ( 5 * points_coordinates.size() ) / 1000;                           // for 1000 points check 5 vectors to find mean distance
                                                                                // so for y points in the data set, x = ( 5 * y ) / 1000 number of vectors will be checked for mean distance
  for (int i = 0; i < num; i++){
    int tmp = rand() % points_coordinates.size();
    auto search = set.find(tmp);
    if (search == set.end()){
      set.insert(tmp);
    }
  }
  for (std::set<int>::iterator it = std::next(set.begin(), 1); it!=set.end(); ++it){  // compute each distance of first point to the other 'set.size() - 1' points
    dist += EuclidianDistance(points_coordinates[*set.begin()]->getCoordinates(), points_coordinates[*it]->getCoordinates());
  }
    dist = dist / set.size();                                                   // get the mean distance

  return dist;
}

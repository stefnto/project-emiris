#include "utils.hpp"
#include <sstream>




//Data_item Methods

double (*Data_item::distanceFunction)(const std::vector<int>& a,const std::vector<int>& b ) = nullptr;

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

void Data_item::setDistanceFunction(double (*dFunction)(const std::vector<int>& a, const std::vector<int>& b)){
      distanceFunction = dFunction;
}

std::string Data_item::getName()const{return this->name;}
double (* Data_item::getDistanceFunction())(const std::vector<int>& a,const std::vector<int>& b){
  return this->distanceFunction;
}



void Data_item::print_coordinates(){
  for (int i = 0; i < this->coordinates.size(); i++){
    std::cout << this->coordinates[i] << " " ;
  }
  std::cout << std::endl;
}
void Data_item::setDistanceFromQuery(float distanceFromQuery){
  this->distanceFromQuery = distanceFromQuery;
}

    int Data_item::get_coordinates_size()
{
  return this->coordinates.size();
}

const std::vector<int>& Data_item::getCoordinates() const {
  return this->coordinates;
}
//clustering_data_item
void clustering_data_item::findNearestCentroid(centroid* centroids,int size){
  float minD = this->calculateDistance(centroids[0]);
  int cent = 0;
  for (int i = 0; i < size; i++ ){
    float dist = this->calculateDistance(centroids[i]);
    if (dist < minD){
        cent = i;
        minD = dist;
    }
  }
  this->setCluster(cent);
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

// Solver Methods

Solver::Solver(int n, int r, std::string output_filepath, double (*distanceFunction)(const std::vector<int>& a, const std::vector<int>& b) )
  : n(n), r(r), output_filepath(output_filepath){

  Data_item::setDistanceFunction(distanceFunction);                           // computing distance between Data_items is handled by the class Data_item

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

 double EuclidianDistance(const std::vector<int>& a,const std::vector<int>& b){
    if (a.size() != b.size())return 0;

    double sum = 0;
    for (int i = 0; i < a.size(); i++)
    {
        sum += pow((a[i] - b[i]), 2);
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

    double (* ptr) (const std::vector<int>& a,const std::vector<int>& b) = query->getDistanceFunction();
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

    for (std::set<int>::iterator it = std::next(set.begin(), 1); it!=set.end(); ++it){  // compute each distance of first point to the other 'set.size() - 1' points
      dist += EuclidianDistance(points_coordinates[*set.begin()]->getCoordinates(), points_coordinates[*it]->getCoordinates());
    }
    dist = dist / set.size();                                                   // get the mean distance
  }
  return dist;
}

void readConfig(std::string config_file, int& k_lsh, int& l_lsh, int& k_medians, int& m_cube, int& k_cube, int& probes_cube){

  std::string num_clust = "number_of_clusters", num_vector_ht = "number_of_vector_hash_tables",
         num_vector_hf = "number_of_vector_hash_functions", m_hcube = "max_number_M_hypercube",
         num_hcube_dim = "number_of_hypercube_dimensions", num_probes = "number_of_probes";

  std::ifstream config;
  std::string tmp;
  int number;

  config.open(config_file);
  if (config.is_open()){
    std::string line;

    while (getline(config, line)){

      if (line.compare(0, 18, num_clust) == 0){
        std::stringstream ss(line);
        while (ss >> tmp){
          if (std::stringstream(tmp) >> number)
            k_medians = number;
        }
      }
      else if (line.compare(0, 28, num_vector_ht) == 0){
        std::stringstream ss(line);
        while (ss >> tmp){
          if (std::stringstream(tmp) >> number)
            l_lsh = number;
        }
      }
      else if(line.compare(0, 31, num_vector_hf) == 0){
        std::stringstream ss(line);
        while (ss >> tmp){
          if (std::stringstream(tmp) >> number)
            k_lsh = number;
        }
      }
      else if(line.compare(0, 22, m_hcube) == 0){
        std::stringstream ss(line);
        while (ss >> tmp){
          if (std::stringstream(tmp) >> number)
            m_cube = number;
        }
      }
      else if(line.compare(0, 30, num_hcube_dim) == 0){
        std::stringstream ss(line);
        while (ss >> tmp){
          if (std::stringstream(tmp) >> number)
            k_cube = number;
        }
      }
      else if (line.compare(0, 16, num_probes) == 0){
        std::stringstream ss(line);
        while (ss >> tmp){
          if (std::stringstream(tmp) >> number)
            probes_cube = number;
        }
      }
    }
  }

}

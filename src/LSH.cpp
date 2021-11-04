#include "LSH.hpp"
#include <sstream>
#include <fstream>


//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path,std::string query_path,std::string output_filepath, int k, int L, int N, int r, double (*distanceFunction)(std::vector<int> a, std::vector<int> b)) : n(N), r(r), L(L),output_filepath(output_filepath){

  this->hashTables = new LSH_HashTable[L];

  LSH_item::setDistanceFunction(distanceFunction);                                               //computing distance between LSH_items is handled by the class LSH_item

  int itemsRead = this->readItems(dataset_path,points_coordinates);                              //reads and inserts items to points_coordinates
  int queriesRead = this->readItems(query_path,queries);
  if ( itemsRead ) {
    int itemDim = points_coordinates.at(0)->get_coordinates_size();
    for (int i = 0 ; i < L ; i++) hashTables[i].init(itemDim,k,itemsRead/4);                    //initializing each hash table
    for (LSH_item* item : points_coordinates){
       for (int i = 0 ; i < L ; i ++) hashTables[i].insert(item);                               //insert a pointer pointing to the item on "points_coordinates" in each hashtable, in a bucket based on hashing with gFunction
    }
  }else std::cout << "dataset_path is empty" << std::endl;
}

bool LSH_solver::solve(){
    LSH_Set* result;
    for (LSH_item* query : queries) {
      result = NNandRS(query);          //returns sorted set with the nearest elements to the query
      writeResult(result,query);        //filters & writes elements to the output file 
      delete result;
    }
    return true;
}

int LSH_solver::readItems(std::string dataset_path,std::vector<LSH_item*>& container){
    std::ifstream datafile;
    int counter = 0;
    datafile.open(dataset_path);
    if (datafile.is_open()){
    std::string line;
    double sttime, endtime; // to compute total run time
    sttime = ((double)clock()) / CLOCKS_PER_SEC;
    while (getline(datafile, line)){
      counter++;
      container.emplace_back(new LSH_item(line));                                                                     // creates a 'LSH_item' and puts it at the end of the vector 'points_coordinates'
    }
    endtime = ((double)clock()) / CLOCKS_PER_SEC;
    datafile.close();
    std::cout << "time needed to read elements : " << endtime - sttime << std::endl;
    }
  return counter;
}

LSH_Set* LSH_solver::NNandRS(LSH_item* query){    //
  auto comp = [](const LSH_item* a,const LSH_item* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  LSH_Set*  nn = new LSH_Set(comp);

  for (int i = 0; i < L; i++) this->hashTables[i].NearestNeighbours(query, nn);

  LSH_Set::iterator it = nn->begin();
  for (int i = 0; i < n; i++){
    if (it == nn->end()) break;
    it++;
  }

  return nn;
}

void LSH_solver::printQueries() const {
  int i = 1;
  for (LSH_item* item : queries) std::cout << "query" << i++ << " : " << item->getName() << std::endl;
}

void LSH_solver::writeResult(LSH_Set* result,LSH_item* item){
  std::ofstream output_file;
  output_file.open(output_filepath,std::ofstream::out | std::ofstream::app);
  output_file << "Query : " << item->getName() << std::endl;
  if (result->size() == 0 ) output_file << "No elements were found near this query" << std::endl;
  else{
      output_file << "Nearest neighbour : " << (*(result->begin()))->getName() << " Distance from query : " << (*(result->begin()))->getDistanceFromQuery() << std::endl << std::endl;
      int counter = 0;
      output_file << "Nearest Neighbours : " << std::endl;
      for (LSH_item* elem : *result){
        if (counter == this->n) break;
        output_file << elem->getName() << " Distance from query : " << elem->getDistanceFromQuery() << std::endl;
        counter++;
      }
      output_file << std::endl;
      output_file << "Elements in radius " << this->r << " from query :" <<std::endl << std::endl;
      for (LSH_item* elem : *result){
        float dFromQuery = elem->getDistanceFromQuery();
        if (dFromQuery < this->r) output_file << elem->getName() << " Distance from query : " << elem->getDistanceFromQuery() << std::endl;
        else break;
      }

  }
  output_file << std::endl;
}

LSH_solver::~LSH_solver(){
  delete[] this->hashTables;
  for (LSH_item* item : this->points_coordinates) delete item;
  std::cout << "deleted points_coordinates" << std::endl;
  for (LSH_item* item : this->queries) delete item;
  std::cout << "deleted queries" << std::endl;
}
//LSH_item Methods;

double (*LSH_item::distanceFunction)(std::vector<int> a,std::vector<int> b ) = nullptr;

LSH_item::LSH_item(std::string name, std::vector<int> coordinates):name(name),coordinates(coordinates){}

void LSH_item::set_id(long id){

    this->ID = id;
}

long LSH_item::get_id() const{
  return this->ID;
}

LSH_item::LSH_item(std::string line){
  std::stringstream ss(line);
  int number;
  ss >> this->name;
  while (ss >> number){
    this->coordinates.push_back(number);           // push each coordinate
  }
}

void LSH_item::setDistanceFromQuery(LSH_item* query){
  this->distanceFromQuery = LSH_item::distanceFunction(this->coordinates,query->coordinates);
}

float LSH_item::getDistanceFromQuery() const{
  return this->distanceFromQuery;
}

void LSH_item::setDistanceFunction(double (*dFunction)(std::vector<int> a, std::vector<int> b)){
      distanceFunction = dFunction;
}

std::string LSH_item::getName()const{return this->name;}


//LSH_HashTable Methods

    LSH_HashTable::LSH_HashTable(int itemDim, int k, int tableSize) : size(tableSize), k(k), hashingFunction(itemDim, k, tableSize)
{

  this->buckets = new std::list<LSH_item *>[tableSize];
}

void LSH_HashTable::init(int itemDim,int k,int tableSize){
  this->size = tableSize;
  this->buckets = new std::list<LSH_item*>[tableSize];
  this->k = k;
  this->hashingFunction.init(itemDim,k,tableSize);
}

LSH_HashTable::~LSH_HashTable(){
  delete[] this->buckets;
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

void LSH_HashTable::insert(LSH_item* item){

    int index = this->hashingFunction(item);
    this->buckets[index].push_back(item);

}
void LSH_HashTable::NearestNeighbours(LSH_item* query,LSH_Set* ordSet){

  int index = this->hashingFunction(query);
  for (LSH_item* item : this->buckets[index]){
       if (query->get_id() == item->get_id()){
        item->setDistanceFromQuery(query);
        ordSet->insert(item);
       }
  }

}

//gFunction Methods

gFunction::gFunction(int itemDim,int k,int tableSize): k(k), tableSize(tableSize) {
    for (int i = 0 ; i < k; i++) linearCombinationElements.push_back(std::pair<int,hFunction>(rGenerator(),hFunction(itemDim,4)));

}

void gFunction::init(int itemDim,int k,int tableSize){
    this->k = k;
    this->tableSize = tableSize;

    for (int i = 0; i < k; i++) linearCombinationElements.push_back(std::pair<int, hFunction>(rGenerator(), hFunction(itemDim,4)));
}

int gFunction::operator()(LSH_item* item){

    long M = 0xFFFFFFFF - 4;    // 2^32 - 1 - 4
    long sum = 0 ;

    for (std::pair<int,hFunction> elem : linearCombinationElements) sum += mod((elem.first*elem.second(item)),M);
    sum = mod(sum,M);
    item->set_id(sum);   //setting id of the item

    return mod(sum,tableSize);


}

//hFunction Methods



hFunction::hFunction(int itemSize,int w):w(23){
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine generator(seed);
  std::uniform_real_distribution<float> distribution(0.0, w * 1.0);

  t = distribution(generator);

  std::normal_distribution<float> distributionN(0.0, 1.0);

  for (int i = 0; i < itemSize; i++) v.push_back(distributionN(generator));
}

int hFunction::operator()(const LSH_item* item){
    std::vector<int>::const_iterator it1 = item->getCoordinates().begin();
    std::vector<float>::const_iterator it2 = v.begin();

    if( item->getCoordinates().size() != v.size() ) throw LSH_Exception();

    float sum = t;
    while ( it2 != v.end() ){
        sum += (*it1)*(*it2);
        it1++;
        it2++;
    }
    sum /= this->w;

    return sum;

}
//general methods

std::vector<LSH_item *> itemGenerator(int amount,int itemSize){

  std::vector<LSH_item*> items;

  for (int i = 0 ; i < amount; i++){
    std::vector<int> co;
    for (int j = 0; j < itemSize; j++) co.push_back(rand() % 201);
    items.push_back( new LSH_item("x"+i,co));
  }
  return items;

}

bool LSH_comperator(LSH_item* a, LSH_item* b){
  return a->getDistanceFromQuery() < b->getDistanceFromQuery();
}

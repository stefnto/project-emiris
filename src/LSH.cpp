#include "LSH.hpp"
#include <sstream>
#include <fstream>


//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path,std::string query_path, int k, int L, int N, int r, double (*distanceFunction)(std::vector<int> a, std::vector<int> b)) : n(N), r(r), L(L){
  this->hashTables = new LSH_HashTable[L];

  LSH_item::setDistanceFunction(distanceFunction);

  int itemsRead = this->getItems(dataset_path,points_coordinates);                              //  reads and inserts items to points_coordinates
  int queriesRead = this->getItems(query_path,this->queries);
  if ( itemsRead ) {                                                                           
    int itemDim = points_coordinates.at(0)->get_coordinates_size();                             
    for (int i = 0 ; i < L ; i++) hashTables[i].init(itemDim,k,itemsRead/4);                    //initializing each hash table
    for (LSH_item* item : points_coordinates){
       for (int i = 0 ; i < L ; i ++) hashTables[i].insert(item);                         
    }
  }else std::cout << "dataset_path is empty" << std::endl;
}

bool LSH_solver::solve( std::string output_path){
    std::cout << "trying to solve..." << std::endl;
    NearestNeighbours(*queries.at(0));
    return true;
}

int LSH_solver::getItems(std::string dataset_path,std::vector<LSH_item*>& container){
    std::ifstream datafile;
    int counter = 0;
    datafile.open(dataset_path);
    if (datafile.is_open()){
    std::string line;
    while (getline(datafile, line)){
      counter++;
      container.emplace_back(new LSH_item(line)); // creates a 'LSH_item' and puts it at the end of the vector 'points_coordinates'
    }
    datafile.close();
    }
  return counter;
}

void LSH_solver::NearestNeighbours(LSH_item& query){
  auto comp = [](const LSH_item* a,const LSH_item* b) -> int {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  std::cout << "lambda created" << std::endl;
  std::set<LSH_item*,decltype(comp)> nn(comp);
  std::cout << "priority_queue created" << std::endl;
  for (int i = 0; i < L; i++)
  {
    this->hashTables[i].NearestNeighbours(query, nn);
    std::cout << "found NNs in hashtable " << i << std::endl;
  }

  std::cout << "N is " << n << "and nn size is " << nn.size() << std::endl;
  std::set<LSH_item*,decltype(comp)>::iterator it = nn.begin();
  for (int i = 0; i < n; i++){
    if (it == nn.end()) break;
    std::cout <<"Nearest neighbour " << i << "  ID: " << (*it)->getItemID() << " Approximate distance from query: " << (*it)->getDistanceFromQuery() << std::endl;
    it++;
  }
}

void LSH_solver::printQueries() const {
  int i = 1;
  for (LSH_item* item : queries) std::cout << "query" << i++ << " : " << item->getItemID() << std::endl;
}
//LSH_item Methods;

double (*LSH_item::distanceFunction)(std::vector<int> a,std::vector<int> b ) = nullptr;

LSH_item::LSH_item(std::string item_id, std::vector<int> coordinates):item_id(item_id),coordinates(coordinates){}

void LSH_item::set_id(long id){

    this->ID = id;
}

long LSH_item::get_id() const{
  return this->ID;
}

LSH_item::LSH_item(std::string line){
  std::stringstream ss(line);
  int number;
  std::string itmID;
  ss >> itmID;
  this->item_id = itmID;
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

std::string LSH_item::getItemID()const{return this->item_id;}
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

    int index = this->hashingFunction(*item);
    this->buckets[index].push_back(item);

}
template <typename comp>
void LSH_HashTable::NearestNeighbours(LSH_item& query,std::set<LSH_item*,comp>& ordSet){

  int index = this->hashingFunction(query);
  std::cout << "index of query : " << query.getItemID() << " is " << index << std::endl;
  for (LSH_item* item : this->buckets[index]){
       if (query.get_id() == item->get_id()){
        item->setDistanceFromQuery(&query);
        ordSet.insert(item);
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

int gFunction::operator()(LSH_item& item){

    long M = 0xFFFFFFFF - 4;    // 2^32 - 1 - 4
    long sum = 0 ;

    for (std::pair<int,hFunction> elem : linearCombinationElements) sum += mod((elem.first*elem.second(item)),M);
    sum = mod(sum,M);
    item.set_id(sum);   //setting id of the item

    return mod(sum,tableSize);
    

}

//hFunction Methods



hFunction::hFunction(int itemSize,int w):w(w){  
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  static std::default_random_engine generator(seed);
  std::uniform_real_distribution<float> distribution(0.0, w * 1.0);

  t = distribution(generator);

  std::normal_distribution<float> distributionN(0.0, 1.0);

  for (int i = 0; i < itemSize; i++) v.push_back(distributionN(generator));
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

std::vector<LSH_item *> itemGenerator(int amount,int itemSize){

  std::vector<LSH_item*> items;

  for (int i = 0 ; i < amount; i++){
    std::vector<int> co;
    for (int j = 0; j < itemSize; j++) co.push_back(rand() % 201);
    items.push_back( new LSH_item("x"+i,co));     
  }
  return items;

}

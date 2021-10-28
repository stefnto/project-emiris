#include "LSH.hpp"
#include <sstream>
#include <fstream>

//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path, int k , int L , int N , int r , double (*distanceFunction)(std::vector<int> a, std::vector<int> b) ):n(n),r(r),L(L){

  this->hashTables = new LSH_HashTable[L];
  
   int itemsRead = this->read_data(dataset_path);                                               //  reads and inserts items to points_coordinates
   if ( itemsRead ) {                                                                           
    int itemDim = points_coordinates.at(0)->get_coordinates_size();                             
    for (int i = 0 ; i < L ; i++) hashTables[i].init(itemDim,k,itemsRead/4);                    //initializing each hash table
    for (LSH_item* item : points_coordinates){
       for (int i = 0 ; i < L ; i ++) hashTables[i].insert(item);                         
     }
   }else std::cout << "dataset_path is empty" << std::endl;
  
}

bool LSH_solver::solve(std::string query_path, std::string output_path){
    return true;
}

int LSH_solver::read_data(std::string dataset_path){
    std::ifstream datafile;
    int counter = 0;
    datafile.open(dataset_path);
    if (datafile.is_open()){
    std::string line;
    while (getline(datafile, line)){
      counter++;
      this->points_coordinates.emplace_back(new LSH_item(line)); // creates a 'LSH_item' and puts it at the end of the vector 'points_coordinates'
    }
    datafile.close();
   }
  this->points_coordinates[0]->print_coordinates();
  std::cout << "points_coordinates size = " << this->points_coordinates[0]->get_coordinates_size() << std::endl;
  std::cout << "points_coordinates size = " << this->points_coordinates[points_coordinates.size() - 1]->get_coordinates_size() << std::endl;
  this->points_coordinates[points_coordinates.size() - 1]->print_coordinates();
  std::cout << "size = " << points_coordinates.size() << std::endl;
  return counter;
}
//LSH_item Methods;

 LSH_item::LSH_item(std::string item_id, std::vector<int> coordinates):item_id(item_id),coordinates(coordinates){}

void LSH_item::set_id(long id){

    this->ID = id;
}

long LSH_item::get_id(){
  return this->ID;
}

LSH_item::LSH_item(std::string line){
  std::stringstream ss(line);
  int number;
  while (ss >> number){
    this->coordinates.push_back(number);           // push each coordinate
  }
}

    //LSH_HashTable Methods
LSH_HashTable::LSH_HashTable(int itemDim, int k,int tableSize) : size(tableSize), k(k),hashingFunction(itemDim,k,tableSize){

    this->buckets = new std::list<LSH_item*>[tableSize];
}

void LSH_HashTable::init(int itemDim,int k,int tableSize){
  std::cout << "I'm init " << std::endl;
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
    std::cout << "gFunction stats : " << std::endl;
    sum = mod(sum,M);
    std::cout << "sum is : " << sum << std::endl;

    item.set_id(sum);   //setting id of the item

    std::cout << "sum MOD tableSize is : " << mod(sum,this->tableSize) << std::endl;

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
  
  std::cout << "Randomly generated h is : " << std::endl;

  for (float el : this->v) std::cout << el << " ";
  std::cout << std::endl;
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
    std::cout <<"H sum is : " << sum << std::endl;
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

#include "LSH.hpp"



//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path,std::string query_path,std::string output_filepath, int k, int L, int N, int r, double (*distanceFunction)(std::vector<int> a, std::vector<int> b)) : n(N), r(r), L(L),output_filepath(output_filepath){
  double sttime, endtime;                                                       // to compute total run time
  this->hashTables = new LSH_HashTable[L];

  Data_item::setDistanceFunction(distanceFunction);                                               //computing distance between Data_items is handled by the class Data_item

  sttime=((double) clock())/CLOCKS_PER_SEC;
  int itemsRead = this->readItems(dataset_path,points_coordinates);                              //reads and inserts items to points_coordinates
  endtime=((double) clock())/CLOCKS_PER_SEC;
  std::cout << "time: " << endtime - sttime << std::endl;
  int queriesRead = this->readItems(query_path,queries);
  if ( itemsRead ) {
    int itemDim = points_coordinates.at(0)->get_coordinates_size();
    for (int i = 0 ; i < L ; i++) hashTables[i].init(itemDim,k,itemsRead/4);                    //initializing each hash table
    for (Data_item* item : points_coordinates){
       for (int i = 0 ; i < L ; i ++) hashTables[i].insert(item);                               //insert a pointer pointing to the item on "points_coordinates" in each hashtable, in a bucket based on hashing with gFunction
    }
  }else std::cout << "dataset_path is empty" << std::endl;
}

bool LSH_solver::solve(){
    LSH_Set* result;
    for (Data_item* query : queries) {
      result = NNandRS(query);
      writeResult(result,query);
      delete result;
    }
    return true;
}

int LSH_solver::readItems(std::string dataset_path,std::vector<Data_item*>& container){
    std::ifstream datafile;
    int counter = 0;
    datafile.open(dataset_path);
    if (datafile.is_open()){
    std::string line;
    while (getline(datafile, line)){
      counter++;
      container.emplace_back(new Data_item(line)); // creates a 'Data_item' and puts it at the end of the vector 'points_coordinates'
    }
    datafile.close();
    }
  return counter;
}

LSH_Set* LSH_solver::NNandRS(Data_item* query){    //
  auto comp = [](const Data_item* a,const Data_item* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  LSH_Set*  nn = new LSH_Set(comp);

  for (int i = 0; i < L; i++) this->hashTables[i].NearestNeighbours(query, nn);


  // std::cout << "N is " << n << "and nn size is " << nn->size() << std::endl;
  LSH_Set::iterator it = nn->begin();
  // std::cout << "Nearest neighbours of query : " << query->getItemID() << std::endl;
  for (int i = 0; i < n; i++){
    if (it == nn->end()) break;
    // std::cout <<"Nearest neighbour " << i << "  ID: " << (*it)->getItemID() << " Approximate distance from query: " << (*it)->getDistanceFromQuery() << std::endl;
    it++;
  }

  return nn;
}

void LSH_solver::printQueries() const {
  int i = 1;
  for (Data_item* item : queries) std::cout << "query" << i++ << " : " << item->getItemID() << std::endl;
}

void LSH_solver::writeResult(LSH_Set* result,Data_item* item){
  std::ofstream output_file;
  output_file.open(output_filepath,std::ofstream::out | std::ofstream::app);
  output_file << "Query : " << item->getItemID() << std::endl;
  if (result->size() == 0 ) output_file << "No elements were found near this query" << std::endl;
  else{
      output_file << "Nearest neighbour : " << (*(result->begin()))->getItemID() << " Distance from query : " << (*(result->begin()))->getDistanceFromQuery() << std::endl << std::endl;
      int counter = 0;
      output_file << "Nearest Neighbours : " << std::endl;
      for (Data_item* elem : *result){
        if (counter == this->n) break;
        output_file << elem->getItemID() << " Distance from query : " << elem->getDistanceFromQuery() << std::endl;
        counter++;
      }
      output_file << std::endl;
      output_file << "Elements in radius " << this->r << " from query :" <<std::endl << std::endl;
      for (Data_item* elem : *result){
        float dFromQuery = elem->getDistanceFromQuery();
        if (dFromQuery < this->r) output_file << elem->getItemID() << " Distance from query : " << elem->getDistanceFromQuery() << std::endl;
        else break;
      }

  }
  output_file << std::endl;
}

LSH_solver::~LSH_solver(){
  delete[] this->hashTables;
  for (Data_item* item : this->points_coordinates) delete item;
  std::cout << "deleted points_coordinates" << std::endl;
  for (Data_item* item : this->queries) delete item;
  std::cout << "deleted queries" << std::endl;
}



//LSH_HashTable Methods

LSH_HashTable::LSH_HashTable(int itemDim, int k, int tableSize) : size(tableSize), k(k), hashingFunction(itemDim, k, tableSize){
  this->buckets = new std::list<Data_item *>[tableSize];
}

void LSH_HashTable::init(int itemDim,int k,int tableSize){
  this->size = tableSize;
  this->buckets = new std::list<Data_item*>[tableSize];
  this->k = k;
  this->hashingFunction.init(itemDim,k,tableSize);
}

LSH_HashTable::~LSH_HashTable(){
  delete[] this->buckets;
}

const std::vector<int>& Data_item::getCoordinates() const {
  return this->coordinates;
}

void Data_item::print_coordinates(){
  for (int i = 0; i < this->coordinates.size(); i++){
    std::cout << this->coordinates[i] << " " ;
  }
  std::cout << std::endl;
}

int Data_item::get_coordinates_size(){
  return this->coordinates.size();
}

void LSH_HashTable::insert(Data_item* item){
  int index = this->hashingFunction(item);
  this->buckets[index].push_back(item);
}

void LSH_HashTable::NearestNeighbours(Data_item* query,LSH_Set* ordSet){

  int index = this->hashingFunction(query);
  for (Data_item* item : this->buckets[index]){
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
    for (int i = 0; i < k; i++)
      linearCombinationElements.push_back(std::pair<int, hFunction>(rGenerator(), hFunction(itemDim,4)));
}

int gFunction::operator()(Data_item* item){

    long M = 0xFFFFFFFF - 4;    // 2^32 - 1 - 4
    long sum = 0 ;

    for (std::pair<int,hFunction> elem : linearCombinationElements) sum += mod((elem.first*elem.second(item)),M);
    sum = mod(sum,M);
    item->set_id(sum);   //setting id of the item

    return mod(sum,tableSize);
}


//general methods

std::vector<Data_item *> itemGenerator(int amount,int itemSize){

  std::vector<Data_item*> items;

  for (int i = 0 ; i < amount; i++){
    std::vector<int> co;
    for (int j = 0; j < itemSize; j++) co.push_back(rand() % 201);
    items.push_back( new Data_item("x"+i,co));
  }
  return items;

}

bool LSH_comperator(Data_item* a, Data_item* b){
  return a->getDistanceFromQuery() < b->getDistanceFromQuery();
}

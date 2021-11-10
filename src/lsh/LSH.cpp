#include "LSH.hpp"



//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path,std::string query_path,std::string output_filepath, int k, int L, int N, int r, double (*distanceFunction)(const std::vector<int>& a, const std::vector<int>& b)) : n(N), r(r), L(L),output_filepath(output_filepath){
  double sttime, endtime;                                                       // to compute total run time
  this->hashTables = new LSH_HashTable[L];

  Data_item::setDistanceFunction(distanceFunction); // computing distance between Data_items is handled by the class Data_item

  int itemsRead = readItems(dataset_path, points_coordinates); // reads and inserts items to points_coordinates

  int queriesRead = readItems(query_path, queries);

  this->w = avgDistance(this->points_coordinates); // use avgDistance() to generate a 'w' for the 'h' functions

  std::cout << "time: " << endtime - sttime << std::endl;
  if ( itemsRead ) {
    int itemDim = points_coordinates.at(0)->get_coordinates_size();
    for (int i = 0 ; i < L ; i++) hashTables[i].init(itemDim,k,itemsRead/4,w);                    //initializing each hash table
    for (Data_item* item : points_coordinates){
       for (int i = 0 ; i < L ; i ++) hashTables[i].insert(item);                               //insert a pointer pointing to the item on "points_coordinates" in each hashtable, in a 
    }
  }else std::cout << "dataset_path is empty" << std::endl;
}

LSH_solver::LSH_solver(std::vector<clustering_data_item *> *clusteringData,int k,int L, int N ,int R , double (*distanceFunction)(const std::vector<int>& a, const std::vector<int>& b) ):n(N),r(r),L(L),clusteringData(clusteringData),clusteringMode(1){ 

  this->hashTables = new LSH_HashTable[L];
  Data_item::setDistanceFunction(distanceFunction);
  int itemDim = clusteringData->at(0)->get_coordinates_size();
  for (int i = 0 ; i < L ; i++) hashTables[i].init(itemDim,k,clusteringData->size()/4,33);
  for (clustering_data_item* item : *clusteringData){
      for (int i = 0 ; i < L; i++) hashTables[i].insert(item);
  }
}

int LSH_solver::clusteringRangeSearch(float radius,Data_item* cent,int id){
  int sum = 0;
  for (int i = 0 ; i < L; i++)sum+= hashTables[i].clusteringRangeSearch(cent,radius);
  return sum;
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


LSH_Set* LSH_solver::NNandRS(Data_item* query){    //
  auto comp = [](const Data_item* a,const Data_item* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
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
  for (Data_item* item : queries) std::cout << "query" << i++ << " : " << item->getName() << std::endl;
}

void LSH_solver::writeResult(LSH_Set* result,Data_item* item){
  std::ofstream output_file;
  output_file.open(output_filepath,std::ofstream::out | std::ofstream::app);
  output_file << "Query : " << item->getName() << std::endl;
  if (result->size() == 0 ) output_file << "No elements were found near this query" << std::endl;
  else{
      output_file << "Nearest neighbour : " << (*(result->begin()))->getName() << " Distance from query : " << (*(result->begin()))->getDistanceFromQuery() << std::endl << std::endl;
      int counter = 0;
      output_file << "Nearest Neighbours : " << std::endl;
      for (Data_item* elem : *result){
        if (counter == this->n) break;
        output_file << elem->getName() << " Distance from query : " << elem->getDistanceFromQuery() << std::endl;
        counter++;
      }
      output_file << std::endl;
      output_file << "Elements in radius " << this->r << " from query :" <<std::endl << std::endl;
      for (Data_item* elem : *result){
        float dFromQuery = elem->getDistanceFromQuery();
        if (dFromQuery < this->r) output_file << elem->getName() << " Distance from query : " << elem->getDistanceFromQuery() << std::endl;
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

LSH_HashTable::LSH_HashTable(int itemDim, int k, int tableSize,int w) : size(tableSize), k(k), hashingFunction(itemDim, k, tableSize,w){
  this->buckets = new std::list<Data_item *>[tableSize];
}

void LSH_HashTable::init(int itemDim,int k,int tableSize,int w){
  this->size = tableSize;
  this->buckets = new std::list<Data_item*>[tableSize];
  this->k = k;
  this->hashingFunction.init(itemDim,k,tableSize,w);
}

LSH_HashTable::~LSH_HashTable(){
  delete[] this->buckets;
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

int LSH_HashTable::clusteringRangeSearch(Data_item* query,float radius){
  int index = this->hashingFunction(query);
  int sum = 0;
  for (Data_item* item : this->buckets[index]){
    if ( query->get_id() == item->get_id() ){
      float distanceFromQuery;
      clustering_data_item *c_d_item = dynamic_cast<clustering_data_item *>(item);
      if ( c_d_item->isSetRadius() ){
        if ( radius < c_d_item->getRadius() ){
          distanceFromQuery = item->calculateDistance(query);
          c_d_item->setCluster(std::stoi(query->getName()));
          c_d_item->setRadius(radius);
          sum++;
        }else if (radius == c_d_item->getRadius() && std::stoi(query->getName()) != c_d_item->getCluster() ){
          distanceFromQuery = item->calculateDistance(query);
          if (distanceFromQuery < c_d_item->getDistanceFromQuery()) {c_d_item->setDistanceFromQuery(distanceFromQuery); c_d_item->setCluster(std::stoi(query->getName()));sum++;}
        }
      }else{
        sum++;
        c_d_item->setCluster(std::stoi(query->getName()));
        c_d_item->setRadius(radius);
        c_d_item->setDistanceFromQuery(query);
      }
    }
  }
  return sum;
}

//gFunction Methods

gFunction::gFunction(int itemDim,int k,int tableSize,int w): k(k), tableSize(tableSize) {
    for (int i = 0 ; i < k; i++) linearCombinationElements.push_back(std::pair<int,hFunction>(rGenerator(),hFunction(itemDim,w)));
}

void gFunction::init(int itemDim,int k,int tableSize,int w){
    this->k = k;
    this->tableSize = tableSize;
    for (int i = 0; i < k; i++) linearCombinationElements.push_back(std::pair<int, hFunction>(rGenerator(), hFunction(itemDim,w)));
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

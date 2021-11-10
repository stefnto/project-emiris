#include "LSH.hpp"



//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path,std::string query_path,std::string output_filepath, int k, int L, int N, int r, double (*distanceFunction)(std::vector<int> a, std::vector<int> b)) : n(N), r(r), L(L),output_filepath(output_filepath){
  this->hashTables = new LSH_HashTable[L];

  Data_item::setDistanceFunction(distanceFunction);                                              // computing distance between Data_items is handled by the class Data_item
  int itemsRead = this->readItems(dataset_path,points_coordinates);                              // reads and inserts items to points_coordinates

  int queriesRead = this->readItems(query_path,queries);

  this->w = avgDistance(this->points_coordinates) / 2;                                           // use avgDistance() to generate a 'w' for the 'h' functions

  if ( itemsRead ) {
    int itemDim = points_coordinates.at(0)->get_coordinates_size();
    for (int i = 0 ; i < L ; i++) hashTables[i].init(itemDim,k,itemsRead/4, w);                  // initializing each hash table
    for (Data_item* item : points_coordinates){
       for (int i = 0 ; i < L ; i ++) hashTables[i].insert(item);                                // insert a pointer pointing to the item on "points_coordinates" in each hashtable,
                                                                                                 // in a bucket based on hashing with gFunction
    }
  }
  else
    std::cout << "dataset_path is empty" << std::endl;


}

bool LSH_solver::solve(){
    LSH_Set* result_approx_NN;
    std::set<double> result_true_NN;

    for (Data_item* query : queries){
      result_approx_NN = NNandRS(query);
      bruteForceSearch(query, this->points_coordinates, this->n, result_true_NN);
      writeResult(result_approx_NN ,query, result_true_NN);
      delete result_approx_NN;
      result_true_NN.clear();
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
      container.emplace_back(new Data_item(line));                              // creates a 'Data_item' and puts it at the end of the vector 'points_coordinates'
    }
    datafile.close();
    }
  return counter;
}

LSH_Set* LSH_solver::NNandRS(Data_item* query){
  auto comp = [](const Data_item* a,const Data_item* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  LSH_Set*  nn = new LSH_Set(comp);                                             // ordered_set that contains NNs

  double sttime, endtime;                                                       // to compute total run time

  sttime=((double) clock())/CLOCKS_PER_SEC;

  for (int i = 0; i < L; i++)
    this->hashTables[i].NearestNeighbours(query, nn);                           // search NN of query for each hashTable

  endtime=((double) clock())/CLOCKS_PER_SEC;

  query->setAlgorithmTime( endtime - sttime );

  return nn;
}

void LSH_solver::printQueries() const {
  int i = 1;
  for (Data_item* item : queries) std::cout << "query" << i++ << " : " << item->getItemID() << std::endl;
}

void LSH_solver::writeResult(LSH_Set* result,Data_item* item, std::set<double>& true_nn){
  std::ofstream output_file;
  output_file.open(output_filepath,std::ofstream::out | std::ofstream::app);
  output_file << "Query : " << item->getItemID() << std::endl;
  if (result->size() == 0 ){
    output_file << "No elements were found near this query using LSH" << std::endl;
    output_file << "distanceTrue : " << *std::next(true_nn.begin(), 0) <<std::endl;
    output_file << "tTrue : " << item->getBruteForceTime() << std::endl;
  }
  else {
      auto it = true_nn.begin();
      int counter = 0;
      for (Data_item* elem : *result){
        if (counter == this->n)
          break;

        output_file << "Nearest neighbor-" << counter << " : " << elem->getItemID() << std::endl;
        output_file << "distanceLSH : " << elem->getDistanceFromQuery() << std::endl;
        output_file << "distanceTrue : " << *std::next(true_nn.begin(), counter) <<std::endl;
        output_file << "tLSH : " << item->getAlgorithmTime() << std::endl;
        output_file << "tTrue : " << item->getBruteForceTime() << std::endl << std::endl;
        counter++;
      }

      output_file << std::endl;

      output_file << "Checking in radius " << this->r << " from query :" << std::endl << std::endl;
      for (Data_item* elem : *result){
        float dFromQuery = elem->getDistanceFromQuery();
        if (dFromQuery < this->r)
        output_file << "  Element : " << elem->getItemID() << ", distance from query : " << elem->getDistanceFromQuery() << std::endl;
      }

  }
  output_file << std::endl << "===============================================================================" << std::endl << std::endl;
}

LSH_solver::~LSH_solver(){
  delete[] this->hashTables;
  for (Data_item* item : this->points_coordinates) delete item;
  // std::cout << "deleted points_coordinates" << std::endl;
  for (Data_item* item : this->queries) delete item;
  // std::cout << "deleted queries" << std::endl;
}



//LSH_HashTable Methods

LSH_HashTable::LSH_HashTable(int itemDim, int tableSize, int k, int w) : size(tableSize), k(k), hashingFunction(itemDim, k, tableSize, w){
  this->buckets = new std::list<Data_item *>[tableSize];
}

void LSH_HashTable::init(int itemDim,int k,int tableSize, int w){
  this->size = tableSize;
  this->buckets = new std::list<Data_item*>[tableSize];
  this->k = k;
  this->hashingFunction.init(itemDim,k,tableSize, w);
}

LSH_HashTable::~LSH_HashTable(){
  delete[] this->buckets;
}

void LSH_HashTable::insert(Data_item* item){
  int index = this->hashingFunction(item);
  this->buckets[index].emplace_back(item);
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

gFunction::gFunction(int itemDim,int k,int tableSize, int w): k(k), tableSize(tableSize) {
    for (int i = 0 ; i < k; i++)
      linearCombinationElements.emplace_back(std::pair<int,hFunction>(rGenerator(),hFunction(itemDim,w)));
}

void gFunction::init(int itemDim,int k,int tableSize, int w){
    this->k = k;
    this->tableSize = tableSize;
    for (int i = 0; i < k; i++)
      linearCombinationElements.emplace_back(std::pair<int, hFunction>(rGenerator(), hFunction(itemDim,w)));
}

int gFunction::operator()(Data_item* item){

    long M = 0xFFFFFFFF - 4;                                                    // 2^32 - 1 - 4
    long sum = 0 ;

    for (std::pair<int,hFunction> elem : linearCombinationElements){
      sum += mod((elem.first*elem.second(item)),M);
    }
    sum = mod(sum,M);
    item->set_id(sum);                                                          //setting id of the item

    return mod(sum,tableSize);
}


//general methods

std::vector<Data_item *> itemGenerator(int amount,int itemSize){

  std::vector<Data_item*> items;

  for (int i = 0 ; i < amount; i++){
    std::vector<int> co;
    for (int j = 0; j < itemSize; j++)
      co.emplace_back(rand() % 201);

    items.emplace_back( new Data_item("x"+i,co));
  }
  return items;

}

bool LSH_comperator(Data_item* a, Data_item* b){
  return a->getDistanceFromQuery() < b->getDistanceFromQuery();
}

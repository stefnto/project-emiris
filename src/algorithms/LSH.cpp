#include "LSH.hpp"


//LSH_solver Methods

LSH_solver::LSH_solver(std::string dataset_path, std::string query_path, std::string output_filepath, int k, int l, int n, int r, double (*distanceFunction)(const std::vector<double>& a, const std::vector<double>& b))
  : Solver(n, r, output_filepath), l(l)
  {
  double sttime, endtime;                                                       // to compute total run time
  this->hashTables = new LSH_HashTable[l];

  int itemsRead = readItems(dataset_path, points_coordinates);                  // reads and inserts items to points_coordinates

  int queriesRead = readItems(query_path, queries);

  this->w =  avgDistance(this->points_coordinates) / 2;                          // use avgDistance() to generate a 'w' for the 'h' functions

  if ( itemsRead ) {
    int itemDim = points_coordinates[0]->get_coordinates_size();

    for (int i = 0 ; i < l ; i++)
      hashTables[i].init(itemDim, itemsRead/4, k, w);                           // initialize each hash table

    for (Data_point* item : points_coordinates){
      for (int i = 0 ; i < l ; i ++)
        hashTables[i].insert(item);                                             // insert a pointer pointing to the item on "points_coordinates" in each hashtable
    }
  }else std::cout << "dataset_path is empty" << std::endl;
}


bool LSH_solver::solve(){
  LSH_Set *result_approx_NN;                                                    // ordered set of ANNs for each query

  std::set<double> result_true_NN;                                              // ordered set of true NNs for each query

  for (Data_query *query : queries)
  {
    result_approx_NN = NNandRS(query);

    bruteForceSearch(query, this->points_coordinates, this->n, result_true_NN);

    writeResult(result_approx_NN, query, result_true_NN);

    delete result_approx_NN;
    result_true_NN.clear();
  }
  return true;
}


LSH_Set* LSH_solver::NNandRS(Data_query* query){

  auto comp = [](const Data_point* a,const Data_point* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  LSH_Set*  nn = new LSH_Set(comp);                                             // ordered_set that contains NNs

  double sttime, endtime;                                                       // to compute total run time

  sttime=((double) clock())/CLOCKS_PER_SEC;

  for (int i = 0; i < l; i++)
    this->hashTables[i].NearestNeighbours(query, nn,i);                           // search NN of query for each hashTable

  endtime=((double) clock())/CLOCKS_PER_SEC;

  query->setAlgorithmTime( endtime - sttime );

  return nn;
}


void LSH_solver::printQueries() const {
  int i = 1;
  for (Data_query* query : queries)
    std::cout << "query " << i++ << " : " << query->get_item_id() << std::endl;
}


void LSH_solver::writeResult(LSH_Set* result, Data_query* query, std::set<double> &true_nn){
  std::ofstream output_file;
  output_file.open(output_filepath, std::ofstream::out | std::ofstream::app);


  output_file << "Query : " << query->get_item_id() << std::endl;
  output_file << "Algorithm: " << "LSH_Vector" << std::endl;

  if (result->size() == 0){
    output_file << "No elements were found near this query using LSH" << std::endl;
    output_file << "distanceTrue : " << *std::next(true_nn.begin(), 0) << std::endl;
    output_file << "tTrue : " << query->getBruteForceTime() << std::endl;
  }
  else {
    auto it = true_nn.begin();
    int counter = 0;

    for (Data_point* point : *result){
      if (counter == this->n)
        break;

      output_file << "Nearest neighbor-" << counter << " : " << point->get_item_id() << std::endl;
      output_file << "distanceLSH : " << point->getDistanceFromQuery() << std::endl;
      output_file << "distanceTrue : " << *std::next(true_nn.begin(), counter) << std::endl;
      output_file << "tLSH : " << query->getAlgorithmTime() << std::endl;
      output_file << "tTrue : " << query->getBruteForceTime() << std::endl << std::endl;
      counter++;
    }

    output_file << std::endl;

    output_file << "Checking in radius " << this->r << " from query :" << std::endl << std::endl;

    for (Data_point *point : *result){
      float dFromQuery = point->getDistanceFromQuery();
      if (dFromQuery < this->r)
        output_file << "  Element : " << point->get_item_id() << ", distance from query : " << point->getDistanceFromQuery() << std::endl;
    }
  }
  output_file << std::endl
              << "===============================================================================" << std::endl
              << std::endl;
}


LSH_solver::~LSH_solver(){
  delete[] this->hashTables;
  for (Data_item* item : this->points_coordinates) delete item;
  // std::cout << "deleted points_coordinates" << std::endl;
  for (Data_item* item : this->queries) delete item;
  // std::cout << "deleted queries" << std::endl;
}



// LSH_HashTable Methods

LSH_HashTable::LSH_HashTable(int itemDim, unsigned long long tableSize, int k, int w)
  : HashTable(k, tableSize), hashingFunction(itemDim, k, tableSize, w){
  this->buckets = new std::list<Data_point*>[tableSize];
}


void LSH_HashTable::init(int itemDim, unsigned long long tableSize, int k, int w){
  this->k = k;
  this->size = tableSize;
  this->buckets = new std::list<Data_point*>[tableSize];

  this->hashingFunction.init(itemDim, k, tableSize, w);
}


LSH_HashTable::~LSH_HashTable(){
  delete[] this->buckets;
}


void LSH_HashTable::insert(Data_point* item){
  int index = this->hashingFunction(item);
  this->buckets[index].push_back(item);
}


void LSH_HashTable::NearestNeighbours(Data_query* query,LSH_Set* ordSet,int index){

  int pos = this->hashingFunction(query);

  for (Data_point* point : this->buckets[pos]){
    if ( query->getID(index) == point->getID(index) ){

      point->setDistanceFromQuery(query);
      ordSet->insert(point);
    }
  }

}



// gFunction Methods

gFunction::gFunction(int itemDim, int k, unsigned long long tableSize, int w): tableSize(tableSize), k(k) {
    for (int i = 0 ; i < k; i++)
      linearCombinationElements.push_back( std::pair<int,hFunction>( rGenerator(),hFunction(itemDim, w) ) );
}


void gFunction::init(int itemDim, int k, unsigned long long tableSize, int w){
    this->k = k;
    this->tableSize = tableSize;
    for (int i = 0; i < k; i++)
      linearCombinationElements.push_back( std::pair<int, hFunction>( rGenerator(), hFunction(itemDim, w) ) );
}



// Gneral Methods

std::vector<Data_item *> itemGenerator(int amount,int itemSize){

  std::vector<Data_item*> items;

  for (int i = 0 ; i < amount; i++){
    std::vector<double> co;
    for (int j = 0; j < itemSize; j++) co.push_back(rand() % 201);
    items.push_back( new Data_item("x"+i,co));
  }
  return items;

}

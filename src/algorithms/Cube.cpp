#include "Cube.hpp"

// Vertex_point methods

Vertex_point::Vertex_point(int itemDim): binary_hash(0) {}


void Vertex_point::bit_concat(int value){                                       // adds a bit { 0,1 } at the end of the number
  if (value == 1){
    this->binary_hash = this->binary_hash << 1;
    this->binary_hash = this->binary_hash | 1;
  }
  else
    this->binary_hash = this->binary_hash << 1;
}


int Vertex_point::getBH(){
  return this->binary_hash;
}



// Cube_HashTable Methods

Cube_HashTable::Cube_HashTable(int k, int dim, unsigned long long buckets_no, int w)
  : HashTable(k, buckets_no), itemDim(dim), w(w){

  this->sets = new std::unordered_map<int, int>[this->k];                       // initialize k unordered_map sets

  this->buckets = new std::list<Data_point *>[this->size];

  for (int i = 0; i < k; i++)
    hFunc.emplace_back( hFunction(this->itemDim, this->w) );                    // generate k hFunctions to use on hashing process

}


Cube_HashTable::~Cube_HashTable(){
  delete[] this->buckets;
  delete[] this->sets;
}


void Cube_HashTable::empty_buckets(int buckets_no){
  int k=0,j=0;
  for (int i = 0; i < buckets_no; i++){
    if (buckets[i].size() != 0)
      k++;
    else
      j++;
  }
  std::cout << "empty buckets = " << j << std::endl;
  std::cout << "non empty buckets = " << k << std::endl;
}


Cube_Set* Cube_HashTable::NN(Data_query* query, int m, int probes){

  auto comp = [](const Data_point* a,const Data_point* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  Cube_Set *ordered_set = new Cube_Set(comp);                                   // ordered_set that contains NNs

  double sttime, endtime;                                                       // to compute total run time


  int counter = 0;                                                              // if counter == M, NN search is stopped
  bool m_points_checked = false;                                                // flag to see if m points where checked

  std::set<unsigned long long> ham_dist_numbers;                                // holds the numbers with hamming_distance x from 'index'

  Vertex_point v(this->itemDim);                                                // initialize Vertex_point for the query
  unsigned long long index = v(query, this->hFunc, this->sets);                 // get binary_hash of query

  sttime=((double) clock())/CLOCKS_PER_SEC;

  for (Data_point* point: this->buckets[index]){                                // check all items in bucket that query was projected into
    if (counter > m){                                                           // if you checked more than M points for NN stop
      m_points_checked = true;
      break;
    }
    point->setDistanceFromQuery(query);
    ordered_set->insert(point);
    counter++;
  }

  if (m_points_checked){                                                        // checked M points in total, so stop
    endtime=((double) clock())/CLOCKS_PER_SEC;
    query->setAlgorithmTime( endtime - sttime );                                // set time that algorithm run for the specific query
    return ordered_set;
  }
  else {                                                                        // bucket that query was projected into had less than M points
    getNumbersWithHammingDistance(this->k, index, probes, ham_dist_numbers);    // determine buckets to check using Hamming Distance

    for (int i = 0; i < probes-1; i++){                                         // check 'probes-1' vertices because 1 vertex has already been checked
      int ham_dist_numbers_index = ( rand() % ham_dist_numbers.size() ) + 1;
      unsigned long long tmp_index = *std::next(ham_dist_numbers.begin(), ham_dist_numbers_index);  // takes a random index that exists in 'ham_dist_numbers' set

      for (Data_point* point: this->buckets[tmp_index]){                        // hashes buckets with 'tmp_index' and check all items in said bucket
        if (counter > m){                                                       // if you checked more than M points for NN stop
          m_points_checked = true;
          break;
        }
        point->setDistanceFromQuery(query);
        ordered_set->insert(point);
        counter++;
      }
      ham_dist_numbers.erase(tmp_index);                                        // erase hash from set, so a bucket is not checked twice

      if (m_points_checked){
        endtime=((double) clock())/CLOCKS_PER_SEC;
        query->setAlgorithmTime( endtime - sttime );
        return ordered_set;
      }
    }
  }

  endtime=((double) clock())/CLOCKS_PER_SEC;
  query->setAlgorithmTime( endtime - sttime );

  return ordered_set;
}



// Cube_Solver Methods

Cube_Solver::Cube_Solver(std::string dataset_path, std::string query_path, std::string output_filepath, int k, int m, int probes, int n, int r, double (*distanceFunction)(const std::vector<double>& a,const std::vector<double>& b))
  : Solver(n, r, output_filepath), k(k), m(m), probes(probes)
  {
    int itemsRead = readItems(dataset_path,points_coordinates);                 // reads and inserts items to points_coordinates

    int queriesRead = readItems(query_path,queries);

    int w = avgDistance(this->points_coordinates);

    this->hashTable = new Cube_HashTable(k, points_coordinates[0]->get_coordinates_size(), pow(2,k), w);

    hashTable->insertV_points(this->points_coordinates);
}


Cube_Solver::~Cube_Solver(){
  delete this->hashTable;
  for (Data_item* item : this->points_coordinates) delete item;
  // std::cout << "deleted points_coordinates" << std::endl;
  for (Data_item* item : this->queries) delete item;
  // std::cout << "deleted queries" << std::endl;
}


bool Cube_Solver::solve(){
    Cube_Set* result_approx_NN;                                                 // ordered set of ANNs for each query

  std::set<double> result_true_NN;                                              // ordered set of true NNs for each query

  for (Data_query* query: queries){

    result_approx_NN = this->hashTable->NN(query, this->m, this->probes);
    bruteForceSearch(query, this->points_coordinates, this->n, result_true_NN);
    //write result
    writeResult(result_approx_NN, query, result_true_NN);
    delete result_approx_NN;
    result_true_NN.clear();
  }
  return true;
}


void Cube_Solver::writeResult(Cube_Set* result, Data_query* query, std::set<double>& true_nn){
  std::ofstream output_file;
  output_file.open(output_filepath, std::ofstream::out | std::ofstream::app);

  output_file << "Query : " << query->get_item_id() << std::endl;
  output_file << "Algorithm: " << "Hypercube" << std::endl;

  if (result->size() == 0){
    output_file << "Nο elements were found near this query using hyperCube" << std::endl;
    output_file << "distanceTrue : " << *std::next(true_nn.begin(), 0) <<std::endl;
    output_file << "tTrue : " << query->getBruteForceTime() << std::endl;
  }
  else {
    int counter = 0;

    for (Data_point* point : *result){
      if (counter == this->n)
        break;

      output_file << "Nearest neighbor-" << counter << " : " << point->get_item_id() << std::endl;
      output_file << "distanceHypercube : " << point->getDistanceFromQuery() << std::endl;
      output_file << "distanceTrue : " << *std::next(true_nn.begin(), counter) << std::endl;
      output_file << "tHypercube : " << query->getAlgorithmTime() << std::endl;
      output_file << "tTrue : " << query->getBruteForceTime() << std::endl << std::endl;
      counter++;
    }

    output_file << std::endl;

    output_file << "Checking in radius " << this->r << " from query :" << std::endl << std::endl;

    for (Data_point* point : *result){
      float dFromQuery = point->getDistanceFromQuery();
      if (dFromQuery < this->r)
      output_file << "  Element : " << point->get_item_id() << ", distance from query : " << point->getDistanceFromQuery() << std::endl;
    }
  }

  output_file << std::endl << "===============================================================================" << std::endl << std::endl;
}

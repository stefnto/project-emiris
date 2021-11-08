#include "Cube.hpp"

// Vertex_point methods

Vertex_point::Vertex_point(int k, int itemDim): binary_hash(0) {

  for (int i = 0; i < k; i++)
    hFunc.emplace_back(hFunction(itemDim, 100));
}

void Vertex_point::bit_concat(int value){                                       // adds a bit { 0,1 } at the end of the number
  if (value == 1){
    this->binary_hash = this->binary_hash << 1;
    this->binary_hash = this->binary_hash | 1;
  }
  else
    this->binary_hash = this->binary_hash << 1;
}

unsigned long long Vertex_point::operator()(Data_item* item, int k) {
  srand(time(NULL));

  int count = 0;
  for (hFunction h_i : hFunc){

    int tmp = h_i(item);                                                        // h_i(p)
    auto search = set.find(tmp);

    if (search == set.end()){                                                   // an h_i(p) with value 'tmp' doesn't exist
      int bit = mod(rand()*tmp, 2);                                             // randomly generate 0 or 1
      set.insert( {tmp, bit} );                                                 // and then map it to the h_i(p) value
      bit_concat(bit);
    }
    else {
      bit_concat(search->second);
    }

  }

  return binary_hash;
}

int Vertex_point::getBH(){
  return this->binary_hash;
}


// Cube_HashTable Methods

Cube_HashTable::Cube_HashTable(int k, int dim, unsigned long long buckets_no, int points_no, std::vector<Data_item*>* coordinates): size(buckets_no), k(k), itemDim(dim){
  pts_coordinates = coordinates;

  this->buckets = new std::list<Data_item *>[this->size];

}

Cube_HashTable::~Cube_HashTable(){
  delete[] this->buckets;
}

void Cube_HashTable::insertV_points(std::vector<Data_item*> & points_coordinates, int k){
  int counter = 0;
  for (int i = 0; i < points_coordinates.size(); i++){

    hcube_points.emplace_back(Vertex_point(k, itemDim));

    unsigned long long index = this->hcube_points[i]( points_coordinates[i], k );

    this->buckets[index].emplace_back(points_coordinates[i]);
  }
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

Cube_Set* Cube_HashTable::NN(Data_item* query, int m, int probes){
  auto comp = [](const Data_item* a,const Data_item* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  Cube_Set *ordered_set = new Cube_Set(comp);                                   // ordered_set that contains NNs

  double sttime, endtime;                                                       // to compute total run time


  int counter = 0;                                                              // if counter == M, NN search is stopped
  bool m_points_checked = false;                                                // flag to see if m points where checked

  std::set<unsigned long long> ham_dist_numbers;                                // holds the numbers with hamming_distance x from 'index'

  Vertex_point v(this->k, this->itemDim);                                       // initialize Vertex_point for the query
  unsigned long long index = v(query, this->k);                                 // get binary_hash of query

  sttime=((double) clock())/CLOCKS_PER_SEC;
  for (Data_item* item: buckets[index]){                                        // check all items in bucket that query was projected into
    if (counter > m){                                                           // if you checked more than M points for NN stop
      m_points_checked = true;
      break;
    }
    item->setDistanceFromQuery(query);
    ordered_set->insert(item);
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

      unsigned long long tmp_index = ( rand() % ham_dist_numbers.size() ) + 1;  // takes a random index that exists in 'ham_dist_numbers' set

      for (Data_item* item: buckets[tmp_index]){                                // hashes buckets with 'tmp_index' and check all items in said bucket
        if (counter > m){                                                       // if you checked more than M points for NN stop
          m_points_checked = true;
          break;
        }
        item->setDistanceFromQuery(query);
        ordered_set->insert(item);
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
Cube_Solver::Cube_Solver(std::string dataset_path, std::string query_path, std::string output_file, int k, int m, int probes, int n, int r, double (*distanceFunction)(std::vector<int> a, std::vector<int> b))
  : k(k), m(m), probes(probes), n(n), r(r), output_filepath(output_file)
  {

    Data_item::setDistanceFunction(distanceFunction);                           // computing distance between Data_items is handled by the class Data_item

    int itemsRead = this->readItems(dataset_path,points_coordinates);           // reads and inserts items to points_coordinates

    int queriesRead = this->readItems(query_path,queries);

    this->hashTable = new Cube_HashTable(k, points_coordinates[0]->get_coordinates_size(), pow(2,k), points_coordinates.size(), &points_coordinates);

    hashTable->insertV_points(points_coordinates, k);


}

Cube_Solver::~Cube_Solver(){
  delete this->hashTable;
  for (Data_item* item : this->points_coordinates) delete item;
  // std::cout << "deleted points_coordinates" << std::endl;
  for (Data_item* item : this->queries) delete item;
  // std::cout << "deleted queries" << std::endl;
}

int Cube_Solver::readItems(std::string dataset_path,std::vector<Data_item*>& container){

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

bool Cube_Solver::solve(){
  Cube_Set* result_approx_NN;
  std::set<double> result_true_NN;

  for (Data_item* query: queries){
    result_approx_NN = this->hashTable->NN(query, this->m, this->probes);
    bruteForceSearch(query, this->points_coordinates, this->n, result_true_NN);
    //write result
    writeResult(result_approx_NN, query, result_true_NN);
    delete result_approx_NN;
    result_true_NN.clear();
  }
  return true;
}

void Cube_Solver::writeResult(Cube_Set* result, Data_item* item, std::set<double>& true_nn){
  std::ofstream output_file;
  output_file.open(output_filepath, std::ofstream::out | std::ofstream::app);
  output_file << "Query : " << item->getItemID() << std::endl;
  if (result->size() == 0){
    output_file << "Nο elements were found near this query using hyperCube" << std::endl;
    output_file << "distanceTrue : " << *std::next(true_nn.begin(), 0) <<std::endl;
    output_file << "tTrue : " << item->getBruteForceTime() << std::endl;
  }
  else {

    int counter = 0;
    for (Data_item* elem : *result){
      if (counter == this->n)
        break;

      output_file << "Nearest neighbor-" << counter << " : " << elem->getItemID() << std::endl;
      output_file << "distanceHypercube : " << elem->getDistanceFromQuery() << std::endl;
      output_file << "distanceTrue : " << *std::next(true_nn.begin(), counter) << std::endl;
      output_file << "tHypercube : " << item->getAlgorithmTime() << std::endl;
      output_file << "tTrue : " << item->getBruteForceTime() << std::endl;
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

#include "Cube.hpp"

// Vertex_point methods

Vertex_point::Vertex_point(int k, int itemDim): binary_hash(0) {
  for (int i = 0; i < k; i++)
    hFunc.emplace_back(hFunction(itemDim));

     std::cout << "Done!" << std::endl;
}

void Vertex_point::init(int k, int itemDim){
  std::cout << "inited" << std::endl;
  this->binary_hash = 0;
  for (int i = 0; i < k; i++)
    hFunc.emplace_back(hFunction(itemDim));
}

void Vertex_point::bit_concat(int value){
  if (value == 1){
    this->binary_hash = this->binary_hash << 1;
    this->binary_hash = this->binary_hash | 1;
  }
  else
    this->binary_hash = this->binary_hash << 1;
}

unsigned long long Vertex_point::operator()(Data_item* item, int k) {
  srand(time(NULL));

  // char buf[k+1];
  // int index = k;
  // buf[index] = '\0';

  int count = 0;
  for (hFunction h_i : hFunc){
    int tmp = h_i(item);                                                        // h_i(p)
    // std::cout << "tmp = " << tmp << std::endl;
    auto search = set.find(tmp);
    if (search == set.end()){                                                   // an h_i(p) with value 'tmp' doesn't exist
      int bit = mod(rand()*tmp, 2);                                             // randomly generate 0 or 1
      // std::cout << "1-------bit = " << bit << std::endl;
      set.insert( {tmp, bit} );                                                 // and then map it to the h_i(p) value
      bit_concat(bit);
      // std::cout << "hash =  " << this->binary_hash << std::endl;

    }
    else {
      // std::cout << "2-------bit = " << search->second << std::endl;
      bit_concat(search->second);
      // std::cout << "hash =  " << this->binary_hash << std::endl;
    }

  }

  return binary_hash;
}


// Cube_HashTable Methods

Cube_HashTable::Cube_HashTable(int k, int dim, int tableSize, int points_no): size(tableSize), k(k), itemDim(dim){
  std::cout << "size = " << tableSize << std::endl;
  this->buckets = new std::list<Data_item *>[this->size];
  this->hcube_points = new std::vector<Vertex_point>[points_no];
  for (int i = 0; i < this->hcube_points->size(); i++)
    (*hcube_points)[i].init(k, dim);
  std::cout << "buckets size = " << buckets->size() << std::endl;
  std::cout << "done!" << std::endl;
}

void Cube_HashTable::init(int k, int dim, int tableSize, int points_no){
  this->size = tableSize;
  this->k = k;
  this->buckets = new std::list<Data_item *>[tableSize];
  this->buckets[0].push_back(new Data_item());
  this->buckets[0].push_back(new Data_item());
  std::cout << "buckets size = " << buckets[1].size() << std::endl;
  std::cout << "buckets size = " << buckets[0].size() << std::endl;
  this->hcube_points = new std::vector<Vertex_point>[points_no];
  for (int i = 0; i < this->hcube_points->size(); i++)
    (*hcube_points)[i].init(k, dim);
}

Cube_HashTable::~Cube_HashTable(){
  delete[] this->buckets;
  delete[] this->hcube_points;
}

void Cube_HashTable::insert(Data_item* item){
  // unsigned long long index = this->hcube_points;
}

void Cube_HashTable::search(){

}


// Cube_Solver Methods
Cube_Solver::Cube_Solver(std::string dataset_path, std::string query_path, std::string output_file, int k, int m, int probes, int n, int r, double (*distanceFunction)(std::vector<int> a, std::vector<int> b))
  : k(k), m(m), probes(probes), n(n), r(r), output_filepath(output_file)
  {
    double sttime, endtime;                                                       // to compute total run time

    Data_item::setDistanceFunction(distanceFunction);                                               //computing distance between Data_items is handled by the class Data_item

    sttime=((double) clock())/CLOCKS_PER_SEC;
    int itemsRead = this->readItems(dataset_path,points_coordinates);                              //reads and inserts items to points_coordinates

    endtime=((double) clock())/CLOCKS_PER_SEC;
    std::cout << "time: " << endtime - sttime << std::endl;
    int queriesRead = this->readItems(query_path,queries);
    std::cout << "i = " << itemsRead << std::endl << "q = " << queriesRead << std::endl;
    this->hashTable = new Cube_HashTable();
    this->hashTable->init(k, points_coordinates[0]->get_coordinates_size(), pow(2,k), points_coordinates.size());

}

Cube_Solver::~Cube_Solver(){
  delete[] this->hashTable;
  for (Data_item* item : this->points_coordinates) delete item;
  std::cout << "deleted points_coordinates" << std::endl;
  for (Data_item* item : this->queries) delete item;
  std::cout << "deleted queries" << std::endl;
}

int Cube_Solver::readItems(std::string dataset_path,std::vector<Data_item*>& container){
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

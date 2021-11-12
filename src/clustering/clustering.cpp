#include <clustering.hpp>

// Clustering_data_item Methods

void Clustering_data_item::findNearestCentroid(Centroid* centroids, int size){
  double minD = this->calculateDistance(centroids[0]);
  int cent = 0;
  for (int i = 0; i < size; i++ ){
    double dist = this->calculateDistance(centroids[i]);
    if (dist < minD){
        cent = i;
        minD = dist;
    }
  }
  this->setCluster(cent);
}

void Clustering_data_item::setDistanceFromNearestCentroid(Clustering_data_item* item){
  this->distanceFromNearestCentroid = Data_item::distanceFunction(this->coordinates, item->getCoordinates());
}



// Clustering_Solver Methods

Clustering_Solver::Clustering_Solver(std::string input_file, std::string output_filepath, int k_lsh, int l_lsh,
              int n, int r, int k_medians, int m_cube, int k_cube, int probes_cube, double (*distanceFunction)(const std::vector<int>& a,const std::vector<int>& b))
  : Solver(n, r, output_filepath), k_lsh(k_lsh), l_lsh(l_lsh), k_medians(k_medians), m_cube(m_cube), k_cube(k_cube), probes_cube(probes_cube)
{
    readItems(input_file, input_data);
    this->distanceFunction = distanceFunction;
}


Clustering_Solver::~Clustering_Solver(){
  // for (std::vector<Clustering_data_item*> iterator i = input_data.being(); i != input_data.end(); ++i)
  //   delete *i;
  //
  // input_data.clear();
}


void Clustering_Solver::solve(method m){
    if (m == classic){
      std::cout << "Running Lloyd algorithm" << std::endl << std::endl;
      this->lloyd();
    }
    else if (m == lsh){
      std::cout << "Running LSH range search algorithm" << std::endl << std::endl;
      this->reverseAssignmentLSH();
    }
    else if (m == hypercube){
      std::cout << "Running Hypercube range search algorithm" << std::endl << std::endl;
      this->reverseAssignmentCube();
    }
}


Centroid* Clustering_Solver::initpp(){
  unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator1(seed1);
  std::uniform_int_distribution<int> distribution(0,input_data.size());

  int t = 1;
  int pos = distribution(generator1);                                           // choosing starting centroid

  std::set<std::string> centroid_ids;                                           // set containing ids of already chosen points

  Centroid* centroids = new Centroid[k_medians];

  centroid_ids.emplace(input_data[pos]->get_item_id());                         // placing first elements id to the set of the centroid ids

  centroids[0] = input_data[pos]->getCoordinates();                             // placing first centroid to the set of the centroids

  while (t != k_medians){

    double prev_sum = 0;

    using sumAndKey = std::pair<double, Clustering_data_item *>;
    using sumAndKeySet = std::set<sumAndKey, bool (*)(const sumAndKey &a, const sumAndKey &b)>;

    auto comp = [](const sumAndKey &a, const sumAndKey &b) -> bool{ return a.first < b.first; };

    sumAndKeySet partial_sums(comp);                                                            //contains elements and partial sums assigned to each element

    for (Clustering_data_item* item : input_data){

      if ( centroid_ids.find(item->get_item_id()) == centroid_ids.end() ){                      // if current point has not been chosen as centroid
        int it = 0;

        const std::vector<int> coordinates = item->getCoordinates();
        double minD = distanceFunction(centroids[0], coordinates);

        while ( ++it < t ) {
          double dist = distanceFunction(centroids[it],coordinates);
          if (dist < minD)
            minD = dist;
        }

        prev_sum += minD*minD;
        partial_sums.emplace(prev_sum, item);
      }
    }

    unsigned seed2 = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator2(seed2);
    std::uniform_real_distribution<double> pickDouble(0, prev_sum);

    double x = pickDouble(generator2);

    sumAndKeySet::iterator current = partial_sums.begin();                                      // checking where the picked float belongs p(r-1) < x <= p(r)

    while (current != partial_sums.end()){

      if ( (*current).first >= x ){                                                             // new centroid found

        centroid_ids.emplace( ((*current).second)->get_item_id() );                             // adding new centroid's id to the set of the centroid_ids
        centroids[t]= ((*current).second)->getCoordinates();                                    // adding new centroid to the set of centroids
        break;

      }
      current++;
    }

    t++;
  }

  return centroids;
}


void Clustering_Solver::lloyd(){
  int elems[k_medians];                                                         // each elem[i] holds the number of elements the cluster_i has
  for (int i = 0; i < k_medians; i++)
    elems[i] = 0;

  Centroid* centroids = this->initpp();

  int iterations = 0 ;
  int limit = 20;
  int num_of_centroids = centroids[0].size();
  int changes = 0;

  while (true){
    Centroid* nextCentroids = new Centroid[k_medians];                          // next iteration will use different centroids

    for (int i = 0; i < k_medians; i++)
      nextCentroids[i].assign(num_of_centroids,0);                              // fill each position of vector with int = 0

    for (Clustering_data_item* item : input_data){
      int c = 0;

      double minD = distanceFunction(item->getCoordinates(),centroids[0]);

      for (int i = 1 ; i < k_medians ; i++){                                    // find the cluster that point belongs to

        double dist = distanceFunction(item->getCoordinates(), centroids[i]);

        if (dist < minD){                                                       // distance from nearest
          minD = dist;
          c = i;                                                                // centroid with least distance from current element
        }

      }

      if (item->getCluster() != c)
        changes++;

      item->setCluster(c);
      item->setDistanceFromNearestCentroid(minD);
      elems[c]++;

      // create generation of new centroids
      for (int j = 0 ; j < num_of_centroids; j++)
        nextCentroids[c][j] += item->getCoordinates()[j];
    }

    // std::cout <<"changes " << changes << std::endl;
    if (changes < input_data.size()/500)
      break;

    changes = 0;

    for (int i = 0; i < k_medians; i++){                                        // creating next gen of centroids
      for (int j = 0 ; j < num_of_centroids; j++){
        nextCentroids[i][j] /= elems[i];
      }
      elems[i] = 0;
    }

    delete[] centroids;
    centroids = nextCentroids;
  }

  // float total = silhouette(centroids);

  delete[] centroids;

  std::ofstream output_file;
  output_file.open(this->output_filepath, std::ofstream::out | std::ofstream::app);

  for (Clustering_data_item* item : input_data)
    output_file << item->get_item_id() << " belongs to cluster : " << item->getCluster() /*<< " silhouette : " << item->getSilhouette()*/ << std::endl;
  // output_file << "total silhouette is : " << total << std::endl;
  output_file.close();
}


double Clustering_Solver::silhouette(Centroid* centroids){
  std::cout << "in silhouette" << std::endl;
  std::list<Clustering_data_item*> clusters[k_medians];                                          // list-k contains the 'Clustering_data_item's that are on k-Cluster

  double total = 0;
  for (Clustering_data_item* item : input_data)
    clusters[item->getCluster()].emplace_back(item);                                             //  put each element to its cluster

  for (int i = 0 ; i < k_medians; i++){                                                          // we're doing this for all elements but we're iterating through clusters
    int counter1 = 0 ;
    std::list<Clustering_data_item*>::iterator it1 = clusters[i].begin();

    while ( it1 != clusters[i].end() ){

      std::list<Clustering_data_item*>::iterator it2 = clusters[i].begin();
      double ai = 0;                                                                             // sum of distances from same cluster

      while ( it2 != clusters[i].end() ){
        double dist = (*it1)->calculateDistance(*it2);
        ai += dist;
        it2++;
      }

      ai /= clusters[i].size();                                                                  // average distance from his neighborhood


      double minD;
      int sec;

      // finding second nearest centroid
      if (i == 0 ) {
        minD = (*it1)->calculateDistance(centroids[1]);
        sec=1;
      }
      else{
        minD = (*it1)->calculateDistance(centroids[i-1]);
        sec = i-1;
      }

      for (int j = 0 ; j < k_medians; j++){
        if (j != i) {
          float dist = (*it1)->calculateDistance(centroids[j]);
          if (dist < minD) {
            minD = dist;
            sec = j;
          }
        }
      }

      double bi = 0;                                                                             // sum of distances from closest cluster
      for (Clustering_data_item* item : clusters[sec])
        bi += (*it1)->calculateDistance(item);

      bi /= clusters[sec].size();
      double max = bi;

      if (ai > bi)
        max = ai;

      double sil = (bi-ai)/max;
      (*it1)->setSilhouette(sil);

      total +=sil;
      it1++;
      counter1++;
      }
    }

  total /= input_data.size();

  return total;
}


void Clustering_Solver::reverseAssignmentLSH(){

  LSH_Solver_Clustering solver(input_data, k_lsh, l_lsh, n, r);                 // initialize LSH_Solver_Clustering
  int population[k_medians];

  Centroid* centroids = this->initpp();                                         // get first generation of centroids

  for (int i = 0; i < k_medians; i++)
    population[i] = 0;

  int num_of_centroids = centroids->size();
  int iterations = 0;

  std::list<Clustering_data_item*> clusters[k_medians];
  Centroid *nextCentroids = new Centroid[k_medians];

  while (true){
    double radius = minCentroidDistance(centroids)/2;
    int changes = 0;
    while (true){
      for (int i = 0 ; i < k_medians; i++) {
        Clustering_data_item centroid(std::to_string(i), centroids[i]);         // make the centroid a 'Clustering_data_item'
        changes += solver.clusteringRangeSearch(&centroid, radius);             // for each centroid do range search
      }
      if (changes < input_data.size() / 1000)
        break;
      changes = 0;
      radius *= 2;
    }

    for (int i = 0; i < k_medians; i++)                                         // fill each position of vector with int = 0
      nextCentroids[i].assign(num_of_centroids, 0);

    for (Clustering_data_item* item : input_data){
      if (item->getRadius() == 0)
        item->findNearestCentroid(centroids, k_medians);
      else
        item->setRadius(0);

      int index = item->getCluster();
      population[index]++;
      for (int j = 0; j < num_of_centroids; j++) {
        const std::vector<int>& coors = item->getCoordinates();
        nextCentroids[index][j] += coors[j];
      }
    }


    if (++iterations == 10 )
      break;

    for (int i = 0 ; i < k_medians ; i++){
      for (int j = 0 ; j < num_of_centroids; j++)
        nextCentroids[i][j] /= population[i];
      population[i] = 0;
    }
    delete[] centroids;
    centroids = nextCentroids;
    nextCentroids = new Centroid[k_medians];
  }
  // float totalSil = silhouette(centroids);

  delete[] nextCentroids;
  delete[] centroids;

  std::ofstream output_file;
  output_file.open(this->output_filepath, std::ofstream::out | std::ofstream::app);
  for (Clustering_data_item *item : input_data)
    output_file << item->get_item_id() << " belongs to cluster : " << item->getCluster() /*<< " silhouette : " << item->getSilhouette()*/ << std::endl;
  // output_file << "total silhouette " << totalSil << std::endl;
  output_file.close();
}


void Clustering_Solver::reverseAssignmentCube(){

  Cube_Solver_Clustering solver(input_data, k_cube, m_cube, probes_cube, n, r);             // initialize Cube_Solver_Clustering
  int population[k_medians];

  Centroid* centroids = this->initpp();                                                     // get first generation of centroids

  for (int i = 0; i < k_medians; i++)
    population[i] = 0;

  int num_of_centroids = centroids->size();
  int iterations = 0;

  std::list<Clustering_data_item*> clusters[k_medians];
  Centroid* nextCentroids = new Centroid[k_medians];

  while (true){
    double radius = minCentroidDistance(centroids)/2;
    int changes = 0;
    while (true){
      for (int i = 0 ; i < k_medians; i++) {
        Clustering_data_item centroid(std::to_string(i), centroids[i]);         // make the centroid a 'Clustering_data_item'
        changes += solver.clusteringRangeSearch(&centroid, radius);             // for each centroid do range search
      }
      if (changes < input_data.size() / 1000)                                   // if changes to centroids were less than a number
        break;                                                                  // representing a floor, stop loop
      changes = 0;
      radius *= 2;
    }

    for (int i = 0; i < k_medians; i++)                                         // fill each position of vector with int = 0
      nextCentroids[i].assign(num_of_centroids, 0);

    for (Clustering_data_item* item : input_data){
      if (item->getRadius() == 0)
        item->findNearestCentroid(centroids, k_medians);
      else
        item->setRadius(0);

      int index = item->getCluster();
      population[index]++;
      for (int j = 0; j < num_of_centroids; j++) {
        const std::vector<int>& coors = item->getCoordinates();
        nextCentroids[index][j] += coors[j];
      }
    }

    if (++iterations == 10 )
      break;

    for (int i = 0 ; i < k_medians ; i++){
      for (int j = 0 ; j < num_of_centroids; j++)
        nextCentroids[i][j] /= population[i];
      population[i] = 0;
    }
    delete[] centroids;
    centroids = nextCentroids;
    nextCentroids = new Centroid[k_medians];
  }
  // float totalSil = silhouette(centroids);

  delete[] nextCentroids;
  delete[] centroids;

  std::ofstream output_file;
  output_file.open(this->output_filepath, std::ofstream::out | std::ofstream::app);
  for (Clustering_data_item *item : input_data)
    output_file << item->get_item_id() << " belongs to cluster : " << item->getCluster() /*<< " silhouette : " << item->getSilhouette()*/ << std::endl;
  // output_file << "total silhouette " << totalSil << std::endl;
  output_file.close();

}


double Clustering_Solver::minCentroidDistance(Centroid* centroids){
  double minDist = this->distanceFunction(centroids[0],centroids[1]);

  for (int i = 0 ; i < k_medians; i++){
    for (int j = i+1 ; j < k_medians; j++){
      double dist = distanceFunction(centroids[i], centroids[j]);
      if (dist < minDist)
        minDist = dist;
    }
  }
  return minDist;
}



// LSH_HashTable_Clustering Methods

void LSH_HashTable_Clustering::initHT(int itemDim, unsigned long long tableSize, int k, int w){

  this->init(itemDim, tableSize, k, w);                                         // init corresponding LSH_HashTable

}


int LSH_HashTable_Clustering::clusteringRangeSearch(Clustering_data_item* centroid, double radius){

  int index = this->hashingFunction(centroid);
  int sum = 0;

  for (Data_point* item : this->buckets[index]){

    if ( centroid->get_ID() == item->get_ID() ){

      double distanceFromCentroid;
      Clustering_data_item* c_d_item = dynamic_cast<Clustering_data_item *>(item);                    // typecast Data_point* to Clustering_data_item*

      checkRadiusOfItem(centroid, radius, c_d_item, sum);
    }
  }
  return sum;
}



// Cube_HashTable_Clustering Methods

int Cube_HashTable_Clustering::clusteringRangeSearch(Clustering_data_item* centroid, double radius, int m, int probes){

    int sum = 0;
    int counter = 0;                                                                  // if counter == m, NN search is stopped
    bool m_points_checked = false;                                                    // flag to see if m points where checked

    std::set<unsigned long long> ham_dist_numbers;                                    // holds the numbers with hamming_distance x from 'index'

    Vertex_point v(this->itemDim);                                                    // initialize Vertex_point for the query
    unsigned long long index = v(centroid, this->hFunc, this->sets);                  // get binary_hash of query

    for (Data_point* item: this->buckets[index]){

      if (counter > m){                                                               // if m points were checked stop
        m_points_checked = true;
        break;
      }

      double distanceFromCentroid;
      Clustering_data_item* c_d_item = dynamic_cast<Clustering_data_item *>(item);    // typecast Data_point* to Clustering_data_item*

      checkRadiusOfItem(centroid, radius, c_d_item, sum);

      counter++;
    }

    if (m_points_checked)
      return sum;
    else {

      getNumbersWithHammingDistance(this->k, index, probes, ham_dist_numbers);

      for (int i = 0; i < probes-1; i++){                                              // check 'probes-1' vertices because 1 vertex has already been checked

        int ham_dist_numbers_index = ( rand() % ham_dist_numbers.size() ) + 1;
        unsigned long long tmp_index = *std::next(ham_dist_numbers.begin(), ham_dist_numbers_index);  // takes a random index that exists in 'ham_dist_numbers' set

        for (Data_point* item: this->buckets[tmp_index]){                              // hashes buckets with 'tmp_index' and check all items in said bucket

          if (counter > m){                                                            // if you checked more than M points for NN stop
            m_points_checked = true;
            break;
          }

          double distanceFromCentroid;
          Clustering_data_item* c_d_item = dynamic_cast<Clustering_data_item *>(item);

          checkRadiusOfItem(centroid, radius, c_d_item, sum);

          counter++;
        }
        ham_dist_numbers.erase(tmp_index);                                        // erase hash from set, so a bucket is not checked twice

        if (m_points_checked)
          return sum;
      }
    }

    return sum;
}



// LSH_Solver_Clustering Methods

LSH_Solver_Clustering::LSH_Solver_Clustering(std::vector<Clustering_data_item *>& clusteringData, int k, int l, int n, int r, double (*distanceFunction)(const std::vector<int>& a, const std::vector<int>& b) )
  : Solver(n, r), k(k), l(l)
  {

  this->hashTables = new LSH_HashTable_Clustering[l];

  int itemDim = clusteringData[0]->get_coordinates_size();

  this->w = avgDistance(clusteringData) / 2;

  for (int i = 0 ; i < l ; i++)
    hashTables[i].initHT(itemDim, clusteringData.size()/8, k, w);

  for (Clustering_data_item* item : clusteringData){
    for (int i = 0 ; i < l; i++)
      hashTables[i].insert(item);
  }
}


LSH_Solver_Clustering::~LSH_Solver_Clustering(){
  delete[] this->hashTables;
  std::cout << "ht deleted" << std::endl;
}


int LSH_Solver_Clustering::clusteringRangeSearch(Clustering_data_item* centroid, double radius){
  int sum = 0;
  for (int i = 0 ; i < l; i++){
    // std::cout << "checking ht " << i+1 << " with radius = " << radius << std::endl;
    sum += hashTables[i].clusteringRangeSearch(centroid, radius);
  }

  return sum;
}



// Cube_Solver_Clustering Methods

Cube_Solver_Clustering::Cube_Solver_Clustering(std::vector<Clustering_data_item*>& clusteringData, int k, int m, int probes, int n, int r, double (*distanceFunction)(const std::vector<int>& a, const std::vector<int>& b))
  : Solver(n, r), k(k), m(m), probes(probes)
  {
    int w = avgDistance(clusteringData);

    this->hashTable = new Cube_HashTable_Clustering(k, clusteringData[0]->get_coordinates_size(), pow(2,k), w);

    hashTable->insertV_points(clusteringData);
  }


Cube_Solver_Clustering::~Cube_Solver_Clustering(){
  delete this->hashTable;
  std::cout << "ht deleted" << std::endl;
}


int Cube_Solver_Clustering::clusteringRangeSearch(Clustering_data_item* centroid, double radius){
  int sum = 0;

  sum = hashTable->clusteringRangeSearch(centroid, radius, this->m, this->probes);

  return sum;

}



// Universal Methods

void checkRadiusOfItem(Clustering_data_item* centroid, double radius, Clustering_data_item* point, int& sum){

  double distanceFromCentroid;

  if ( point->getRadius() ){                                                                                // item was previously checked for a specific radius

    // if point has been checked for the same radius but for a different cluster
    if (radius == point->getRadius() && std::stoi(centroid->get_item_id()) != point->getCluster() ){

      distanceFromCentroid = point->calculateDistance(centroid);                                            // calculate distance from current centroid to item


      if ( distanceFromCentroid < point->getDistanceFromNearestCentroid() ) {
        point->setDistanceFromNearestCentroid(distanceFromCentroid);
        point->setCluster( std::stoi(centroid->get_item_id()) );
        sum++;                                                                                              //new change made
      }
    }
  }
  else{
    sum++;
    point->setCluster( std::stoi(centroid->get_item_id()) );
    point->setRadius(radius);
    point->setDistanceFromNearestCentroid(centroid);
  }
}

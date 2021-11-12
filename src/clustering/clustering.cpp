#include <clustering.hpp>

clustering::clustering(std::string input_file, std::string output_file, int k_lsh, int l_lsh, int n, int r, int k_medians, int m_cube, int k_cube, int probes_cube, double (*distanceFunction)(const std::vector<int>& a,const std::vector<int>& b))
  :k_lsh(k_lsh), l_lsh(l_lsh), n(n), r(r), k_medians(k_medians), m_cube(m_cube), k_cube(k_cube), probes_cube(probes_cube), output_filepath(output_file)
{
    readItems(input_file, input_data);
    this->distanceFunction = distanceFunction;
}

void clustering::solve(method m){
    Data_item::setDistanceFunction(this->distanceFunction);

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

centroid* clustering::initpp(){
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator1(seed1);
    std::uniform_int_distribution<int> distribution(0,input_data.size());
    // std::cout << "input_data.size()" << input_data.size() << std::endl;

    int t = 1;
    int pos = distribution(generator1);                                         //choosing starting centroid

    // std::cout << "position of first centroid chosen : " << pos << std::endl;

    std::set<std::string> centroid_ids;                                         //set containing ids of already chosens points

    centroid* centroids = new centroid[k_medians];

    centroid_ids.emplace(input_data.at(pos)->getName());                        //placing first elements id to the set of the centroid ids

    centroids[0]= input_data.at(pos)->getCoordinates();                         //placing first centroid to the set of the centroids

    while (t != k_medians){

        float prev_sum = 0;

        using sumAndKey = std::pair<float, clustering_data_item *>;
        using sumAndKeySet = std::set<sumAndKey, bool (*)(const sumAndKey &a, const sumAndKey &b)>;

        auto comp = [](const sumAndKey &a, const sumAndKey &b) -> bool{ return a.first < b.first; };

        sumAndKeySet partial_sums(comp);                                                           //contains elements and partial sums assigned to each element
        for (clustering_data_item* item : input_data){

            if ( centroid_ids.find(item->getName()) == centroid_ids.end() ){                      // if current point has not been chosen as centroid
                int it = 0;
                const std::vector<int> coordinates = item->getCoordinates();
                float minD = distanceFunction(centroids[0],coordinates);
                while ( ++it < t ) {
                    float dist = distanceFunction(centroids[it],coordinates);
                    if (dist < minD) minD = dist;
                }
                prev_sum += minD*minD;
                partial_sums.emplace(prev_sum,item);
            }
        }
        unsigned seed2 = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator2(seed2);
        std::uniform_real_distribution<float> pickFloat(0,prev_sum);

        float x = pickFloat(generator2);

        // std::cout << "float picked : " << x << std::endl;

        sumAndKeySet::iterator current = partial_sums.begin();                            //checking where the picked float belongs p(r-1) < x <= p(r)

        while (current != partial_sums.end()){
          if ( (*current).first >= x ){                                                 //new centroid found
            centroid_ids.emplace(((*current).second)->getName());                    //adding new centroid's id to the set of the centroid_ids
            centroids[t]= ((*current).second)->getCoordinates();                     //adding new centroid to the set of centroids
            break;
          }
          current++;
        }
        t++;
    }
    // for (std::string name : centroid_ids) std::cout << name << std::endl;
    return centroids;
}

void clustering::lloyd(){
  int elems[k_medians];                                                       // each elem[i] holds the number of elements the cluster_i has
  for (int i = 0; i < k_medians; i++)
    elems[i] = 0;

  centroid* centroids = this->initpp();
  int iterations = 0 ;
  int limit = 20;
  int num_of_centroids = centroids[0].size();
  int changes = 0;
  while (true){
    centroid* nextCentroids = new centroid[k_medians];                            //next iteration will use different centroids

    for (int i = 0; i < k_medians; i++)                                           // fill each position of vector with int = 0
      nextCentroids[i].assign(num_of_centroids,0);

    for (clustering_data_item* item : input_data){
      float minD = distanceFunction(item->getCoordinates(),centroids[0]);
      int c = 0;
      for (int i = 1 ; i < k_medians ; i++){                                      // find the cluster that point belongs to
        float dist = distanceFunction(item->getCoordinates(), centroids[i]);
        if (dist < minD){                                                         //distance from nearest
          minD = dist;
          c = i;                                                                  //centroid with least distance from current element
        }
      }
      if (item->getCluster() != c) changes++;
      item->setCluster(c);
      item->setDistanceFromQuery(minD);
      elems[c]++;
      // create generation of new centroids
      for (int j = 0 ; j < num_of_centroids; j++)
        nextCentroids[c][j] += item->getCoordinates()[j];
    }

    // std::cout <<"changes " << changes << std::endl;
    if (changes < input_data.size()/500)
      break;
    changes = 0;

    for (int i = 0; i < k_medians; i++){                                             //creating next gen of centroids
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
  for (clustering_data_item* item : input_data)
    output_file << item->getName() << " belongs to cluster : " << item->getCluster() /*<< " silhouette : " << item->getSilhouette()*/ << std::endl;
  // output_file << "total silhouette is : " << total << std::endl;
  output_file.close();
}

float clustering::silhouette(centroid* centroids){
  std::cout << "in silhouette" << std::endl;
  std::list<clustering_data_item*> clusters[k_medians];
  float total = 0;
  for (clustering_data_item* item : input_data) clusters[item->getCluster()].push_back(item);           // firstly we put each element to its cluster

  for (int i = 0 ; i < k_medians; i++){                                                                 // we're doing this for all elements but we're iterating through clusters
    int counter1 = 0 ;
    std::list<clustering_data_item *>::iterator it1 = clusters[i].begin();
    while ( it1 != clusters[i].end() ){
      std::list<clustering_data_item *>::iterator it2 = clusters[i].begin();
      float ai = 0;                                                                                     //sum of distances from same cluster

      while ( it2 != clusters[i].end() ){
        float dist = (*it1)->calculateDistance(*it2);
        ai+=dist;
        it2++;
      }

      ai /= clusters[i].size();                                                                         //average distance from his neighborhood


      float minD;
      int sec;

        //finding second nearest centroid
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

      float bi = 0;                                                                                     // sum of distances from closest cluster
      for (clustering_data_item* item : clusters[sec])
        bi += (*it1)->calculateDistance(item);
      bi /= clusters[sec].size();
      float max = bi;
      if (ai > bi)
        max = ai;
      float sil = (bi-ai)/max;
      (*it1)->setSilhouette(sil);
      total +=sil;
      it1++;
      counter1++;
      }
    }
  total /= input_data.size();
  return total;
}

void clustering::reverseAssignmentLSH(){
  LSH_solver solver(&input_data, k_lsh, l_lsh, n, r);                           // initialize LSH
  int population[k_medians];

  centroid *centroids = this->initpp();                                         // get first generation of centroids
  for (int i = 0; i < k_medians; i++)
    population[i] = 0;
  int num_of_centroids = centroids->size();
  int iterations = 0;
  std::list<clustering_data_item*> clusters[k_medians];
  centroid *nextCentroids = new centroid[k_medians];

  while (true){
    float radius = minCentroidDistance(centroids)/2;
    int changes = 0;
    while (true){
      for (int i = 0 ; i < k_medians; i++) {
        Data_item centr_to_di(std::to_string(i),centroids[i]);                  // make the centroid a 'Data_item'
        changes += solver.clusteringRangeSearch(radius,&centr_to_di,i);
      }
      if (changes < input_data.size() / 1000)
        break;
      changes = 0;
      radius *= 2;
    }

    for (int i = 0; i < k_medians; i++)                                         // fill each position of vector with int = 0
      nextCentroids[i].assign(num_of_centroids, 0);

    for (clustering_data_item* item : input_data){
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
    nextCentroids = new centroid[k_medians];
  }
  // float totalSil = silhouette(centroids);

  delete[] nextCentroids;
  delete[] centroids;
  std::ofstream output_file;
  output_file.open(this->output_filepath, std::ofstream::out | std::ofstream::app);
  for (clustering_data_item *item : input_data)
    output_file << item->getName() << " belongs to cluster : " << item->getCluster() /*<< " silhouette : " << item->getSilhouette()*/ << std::endl;
  // output_file << "total silhouette " << totalSil << std::endl;
  output_file.close();
}

float clustering::minCentroidDistance(centroid* centroids){
  float minDist = this->distanceFunction(centroids[0],centroids[1]);
  for (int i = 0 ; i < k_medians; i++){
    for (int j = i+1 ; j < k_medians; j++){
      float dist = distanceFunction(centroids[i], centroids[j]);
      if (dist < minDist)
        minDist = dist;
    }
  }
  return minDist;
}

void clustering::reverseAssignmentCube(){
  Cube_Solver_Clustering solver(input_data, k_cube, m_cube, probes_cube, n, r);             // initialize Cube_Solver_Clustering
  int population[k_medians];

  centroid *centroids = this->initpp();                                                     // get first generation of centroids
  for (int i = 0; i < k_medians; i++)
    population[i] = 0;

  int num_of_centroids = centroids->size();
  int iterations = 0;
  std::list<clustering_data_item*> clusters[k_medians];
  centroid *nextCentroids = new centroid[k_medians];

  while (true){
    float radius = minCentroidDistance(centroids)/2;
    int changes = 0;
    while (true){
      for (int i = 0 ; i < k_medians; i++) {
        Data_item centr_to_di(std::to_string(i),centroids[i]);                  // make the centroid a 'Data_item'
        changes += solver.clusteringRangeSearch(radius,&centr_to_di,i);
      }
      if (changes < input_data.size() / 1000)                                   // if changes to centroids were less than a number
        break;                                                                  // representing a floor, stop loop
      changes = 0;
      radius *= 2;
    }

    for (int i = 0; i < k_medians; i++)                                         // fill each position of vector with int = 0
      nextCentroids[i].assign(num_of_centroids, 0);

    for (clustering_data_item* item : input_data){
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
    nextCentroids = new centroid[k_medians];
  }
  // float totalSil = silhouette(centroids);

  delete[] nextCentroids;
  delete[] centroids;
  std::ofstream output_file;
  output_file.open(this->output_filepath, std::ofstream::out | std::ofstream::app);
  for (clustering_data_item *item : input_data)
    output_file << item->getName() << " belongs to cluster : " << item->getCluster() /*<< " silhouette : " << item->getSilhouette()*/ << std::endl;
  // output_file << "total silhouette " << totalSil << std::endl;
  output_file.close();

}


// Cube_Solver_Clustering Methods

Cube_Solver_Clustering::Cube_Solver_Clustering(std::vector<clustering_data_item*>& clusteringData, int k, int m, int probes, int n, int r, double (*distanceFunction)(const std::vector<int>& a, const std::vector<int>& b))
  : Solver(n, r), k(k), m(m), probes(probes)
  {
    int w = avgDistance(clusteringData);

    this->hashTable = new Cube_HashTable(k, clusteringData[0]->get_coordinates_size(), pow(2,k), clusteringData.size(), w);

    hashTable->insertV_points(clusteringData);
  }

  int Cube_Solver_Clustering::clusteringRangeSearch(float radius, Data_item* centroid, int id){
    int sum = 0;

    sum = hashTable->clusteringRangeSearch(centroid, radius, this->m, this->probes);

    return sum;

  }

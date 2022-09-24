#include "curves_clustering.hpp"
#include "Curves.hpp"


// Clustering_curve_solver methods
Clustering_curve_solver::Clustering_curve_solver(std::string input_file, std::string output_filepath, int k_lsh, int l_lsh, int n, int r, int k_medians, int m_cube,int k_cube, int probes_cube, int complete_flag, updateMethods um, assignmentMethods am)
:Solver(n, r, output_filepath), k_lsh(k_lsh), l_lsh(l_lsh), k_medians(k_medians), m_cube(m_cube), k_cube(k_cube), probes_cube(probes_cube), complete_flag(complete_flag){


  this->updateStep = um;
  this->assignmentStep = am;

  readItems(input_file, input_data);

  // std::cout <<"input_file is : " << input_file << " k is " << k_medians << std::endl;

  // updateMethods m = meanVector;
  // updateMethods m = meanFrechet;

  // std::cout << "total elements are : " << input_data.size() << std::endl;

  switch (updateStep){
    case meanFrechet :
        std::cout << "updateStep is meanFrechet" << std::endl;
        this->distanceFunction = discreteFrechet;
        break;

    case meanVector :
        std::cout << "updateStep is meanVector" << std::endl;
        this->distanceFunction = EuclidianDistance;
        break;
  }
}


std::vector<Clustering_curve_data_item>* Clustering_curve_solver::initpp(){
  unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator1(seed1);
  std::uniform_int_distribution<int> distribution(0, input_data.size());
  int t = 1;
  int pos = distribution(generator1);                                                                   // choosing starting centroid

  std::set<std::string> centroid_ids;                                                                   // set containing ids of already chosen points

  std::vector<Clustering_curve_data_item>* centroids = new std::vector<Clustering_curve_data_item>;

  centroids->reserve(k_medians);

  centroid_ids.emplace(input_data[pos]->get_item_id());                                                 // placing first elements id to the set of the centroid ids

  centroids->emplace_back(input_data[pos]->get_item_id(), input_data[pos]->getCoordinates());            // placing first centroid to the set of the centroids

  while (t != k_medians){
    // std::cout << "t is " << t << std::endl;

    double prev_sum = 0;

    using sumAndKey = std::pair<double, Clustering_curve_data_item *>;

    size_t arraySize = input_data.size() - t;

    sumAndKey partial_sums[arraySize];                                                                  // Allocating an array that will contain Clustering_curve_data_items* and their sums

    int pos = 0;

    for (Clustering_curve_data_item *item : input_data){

      if ( centroid_ids.find(item->get_item_id()) == centroid_ids.end() ){                              // if current point has not been chosen as centroid
        int it = 0;
        double minD = distanceFunction(&centroids->at(0), item);

        while (++it < t){
          double dist = distanceFunction(&centroids->at(it),item);                                      // Discrete Frechet distance between point and Centroid;

          if (dist < minD)
            minD = dist;
        }

        prev_sum += minD * minD;
        partial_sums[pos] = sumAndKey(prev_sum, item);
        ++pos;

      }
    }

    unsigned seed2 = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator2(seed2);
    std::uniform_real_distribution<double> pickDouble(0, prev_sum);

    double x = pickDouble(generator2);
    size_t start = 0, end = arraySize - 1;

    while (true){                                                                                       // binary search in order to find which point should be a centroid
      size_t mid = (start + end)/2;

      if ( partial_sums[mid].first < x  &&  partial_sums[mid + 1].first >= x){                          // new centroid found
        centroid_ids.emplace( partial_sums[mid + 1].second->get_item_id() );

        centroids->emplace_back( partial_sums[mid+1].second->get_item_id(), partial_sums[mid+1].second->getCoordinates() );
        break;
      }
      else {
        if ( x > partial_sums[mid].first  &&  start != 0 )
          start = mid;

        else if ( x >= partial_sums[mid].first ){
          centroid_ids.emplace( partial_sums[mid].second->get_item_id() );
          centroids->emplace_back( partial_sums[mid+1].second->get_item_id(), partial_sums[mid+1].second->getCoordinates() );
          break;
        }

        else end = mid;
      }

      // std::cout << "x is " << x << " partial_sums[mid] is " << partial_sums[mid].first << " partial_sums[mid+1] is " << partial_sums[mid+1].first << std::endl;
    }

    t++;
  }
  // std::cout <<"Centroids found with ";

  switch (updateStep){
    case meanFrechet :
                      std::cout << "meanFrechet method " << std::endl;
                      break;
    case meanVector  :
                      std::cout << "meanVector method " << std::endl;
                      break;
    }

    for (std::string id : centroid_ids)
      std::cout << "Element with id " << id << " selected as starting centroid" << std::endl;

    return centroids;
}


void Clustering_curve_solver::solve(){
  // this->updateStep = um;
  // this->assignmentStep = am;
  this->clusters.assign(k_medians,std::vector<Clustering_curve_data_item*>());
  genericClustering();

}


void Clustering_curve_solver::genericClustering(){
  int elems[k_medians];                                                         // each elem[i] holds the number of elements the cluster_i has
  for (int i = 0; i < k_medians; i++)
    elems[i] = 0;

  std::vector<Clustering_curve_data_item>* centroids = initpp();
  // std::vector<Clustering_curve_data_item> *centroids = new std::vector<Clustering_curve_data_item>;

  // int count = 0;
  // for (int i = 0 ; i < input_data.size(); i += 10){
  //   centroids->emplace_back(input_data[i]->get_item_id(),input_data[i]->getCoordinates());
  //   // centroids->at(count).print_coordinates();
  //   // std::cout << std::endl << std::endl;
  //   // count++;
  // }

  std::cout<< "centroids_size " << centroids->size() << std::endl;

  int iterations = 0 ;
  int limit = 20;
  size_t centroidSize = centroids[0].size();
  int changes = 0;

  std::cout << "Getting inside the loop" << std::endl;

  while (iterations != 3){
    this->updateStep = meanFrechet;

    std::vector<Clustering_curve_data_item>* nextCentroids = new std::vector<Clustering_curve_data_item>;
    nextCentroids->assign(k_medians, Clustering_curve_data_item());

    if (updateStep == meanVector){
      std::cout << "Trying to initialize nextCentroids" << std::endl;

      // nextCentroids->assign(k_medians,Clustering_curve_data_item());
      for (int i = 0; i < k_medians; i++)
        nextCentroids->at(i).getCoordinates().assign(centroidSize,0);

      // for (int i=0; i < k_medians; i++){
      //   nextCentroids->at(i).print_coordinates();
      //   std::cout << std::endl << std::endl;
      // }
      // std::cout << "Terminating..." << std::endl;
      // exit(1);
    }
    switch(assignmentStep){
          case lloydCurve     :{
            // std::cout << "trying to assign with lloyd" << std::endl;
            changes = lloydAssignment(nextCentroids,centroids);
            // std::cout << "done assigning with lloyd " << std::endl;
          }
            break;
          // case lshV         :
          //   break;
          // case hypercubeV   :
          //   break;
          case lsh_frechet :
            break;
    }


    // std::cout <<"changes " << changes << std::endl;
    if (changes < input_data.size()/500)
      break;

    changes = 0;
    updateStep = meanFrechet;
    switch (updateStep){
        case meanVector:
            for (int i = 0; i < k_medians; i++){                                        // creating next gen of centroids
              std::vector<double>& coordinates = nextCentroids->at(i).getCoordinates();
              for (int j = 0 ; j < centroidSize; j++)
                if (this->clusters[i].size() != 0 )
                  coordinates.at(j) /= this->clusters[i].size();

              elems[i] = 0;
              std::cout << "cluster " << i << "size before clean " << this->clusters[i].size() << std::endl;
              this->clusters[i].clear();
              std::cout << "cluster " << i << "size after clean " << this->clusters[i].size() << std::endl;
            }
        break;

        case meanFrechet:
            // std::cout << "Trying to meanFrechet update" << std::endl;
            meanFrechetUpdate(nextCentroids);
            // std::cout << " prin to delete1" << std::endl;
            for (int i = 0 ; i < k_medians; i++)
              clusters[i].clear();

            break;
    }

    // std::cout << " prin to delete" << std::endl;
    delete centroids;
    centroids = nextCentroids;

    // std::cout << "in generic, printing" << std::endl;
    // for (int i=0; i < k_medians; i++){
    //   centroids->at(i).print_ts();
    //   std::cout << std::endl << std::endl;
    // }
    // std::cout << "Terminating..." << std::endl;
    // exit(1);
    iterations++;
  }

  // for (int i=0; i < k_medians; i++){
  //   // centroids->at(i).print_ts();
  //   // std::cout << std::endl << std::endl;
  // }
  // std::cout << "mission accomplished" << std::endl;
  writeResult(this->updateStep, this->assignmentStep, centroids, this->clusters);

  delete centroids;
}


int Clustering_curve_solver::lloydAssignment(std::vector<Clustering_curve_data_item> *nextCentroids, std::vector<Clustering_curve_data_item> *centroids){
  int changes = 0;
  this->clusters.reserve(k_medians);

  for (Clustering_curve_data_item* item : input_data){
    // int c = 0;
    // double minD = this->distanceFunction(&centroids->at(0),item);
    //
    // std::cout <<"calculated minD" << std::endl;
    //
    // for (int i = 1 ; i < k_medians ; i++){                                   // find the cluster that point belongs to
    //   double dist = this->distanceFunction(&centroids->at(i), item);
    //   if (dist < minD){                                                      // distance from nearest
    //     minD = dist;
    //     c = i;                                                               // centroid with least distance from current element
    //   }
    // }

    int c = 0;
    double minD = this->distanceFunction(&centroids->at(0), item);

    // std::cout << "Centroid id = " << centroids->at(0).get_item_id() << std::endl;
    // centroids->at(0).print_coordinates();
    // std::cout << std::endl;
    // exit(1);
    // std::cout << "calculated minD" << std::endl;

    for (int i = 1; i < k_medians; i++){                                        // find the cluster that point belongs to
      double dist = this->distanceFunction(&centroids->at(i), item);
      if (dist < minD){                                                         // distance from nearest
        minD = dist;
        c = i;                                                                  // centroid with least distance from current element
      }
    }


    // std::cout << "done calculating minDistances" << std::endl;

    if (item->getCluster() != c)
      changes++;

    item->setCluster(c);
    item->setDistanceFromNearestCentroid(minD);
    this->clusters[c].emplace_back(item);
    // std::cout << "size = " << this->clusters[c].size() << std::endl;
    // std::cout << "done settingup clusters" << std::endl;

    // create new generation of new centroids
    switch (updateStep){
      case meanVector:{
            // std::cout << "where!!!!!?!?!?!?" << std::endl;

            std::vector<double> &itemCoordinates = item->getCoordinates();

            // std::cout << "Got coordinates 1" << std::endl;

            std::vector<double> &centroidCoordinates = nextCentroids->at(c).getCoordinates();

            // std::cout << "Got coordinates 2" << std::endl;

            for (int j = 0; j < nextCentroids->at(c).get_coordinates_size(); j++)

                centroidCoordinates.at(j) += itemCoordinates.at(j);
            break;
          }

      case meanFrechet:{
            // std::cout << "do nothing " << std::endl;
            break;
          }

    }
    // std::cout << "dafaq!?" << std::endl;
  }
  return changes;
}


void Clustering_curve_solver::meanFrechetUpdate(std::vector<Clustering_curve_data_item> *nextCentroids){

  // std::cout << "in meanFrechetUpdate" << std::endl << std::endl;
  for (int i = 0 ; i < k_medians; i++){

  std::list<Clustering_curve_data_item*> tree;

  // std::cout << "printing cluster " << i << std::endl;
  for (Clustering_curve_data_item* item : this->clusters[i]){
    tree.push_back(item);
  }

  while (tree.size() != 1){
    // std::cout << "tree.size is " << tree.size() << std::endl;
    Clustering_curve_data_item* item1 = tree.front();
    tree.pop_front();

    Clustering_curve_data_item* item2 = tree.front();
    tree.pop_front();
    Clustering_curve_data_item* item3 = meanFrechetCurve(item1,item2);

    // std::cout << "?" << std::endl;
    tree.push_back(item3);

    // std::cout << "??" << std::endl;
    if (tree.size() < this->clusters[i].size() / 2 ){
      delete item1;
      delete item2;
    }
    // std::cout << "??" << std::endl;
  }

  // auto it = nextCentroids.begin()+i;
  // nextCentroids->emplace_back(*tree.front());
  // std::cout << "???" << std::endl;
  nextCentroids->at(i) = *tree.front();

  // std::cout << i+1 << "-th new centroid" << std::endl;
  // nextCentroids->at(i).print_ts();
  }
  // std::cout << "in meanFrechetUpdate, terminating..." << std::endl;
  // exit(1);
}


void Clustering_curve_solver::writeResult(updateMethods um, assignmentMethods am, std::vector<Clustering_curve_data_item> *centroids, std::vector<std::vector<Clustering_curve_data_item*>> clusters1){

  // double s_total = silhouette(centroids);

  // std::vector<double> silhouettes;
  // for (int i = 0; i < k_medians; i++)
  //   silhouettes.push_back( 0.0 );

  std::vector<std::string> clusters[k_medians];

  for (Clustering_curve_data_item* item : input_data){                          // assign every item to the cluster for output
    int tmp = item->getCluster();
    clusters[tmp].emplace_back(item->get_item_id());
    // silhouettes[tmp] += item->getSilhouette();                                  // get total silhouette for cluster
  }

  // for (int i = 0; i < silhouettes.size(); i++)
  //   silhouettes[i] = silhouettes[i] / clusters[i].size();

  std::ofstream output_file;
  output_file.open(this->output_filepath, std::ofstream::out | std::ofstream::app);


  if ( am == lloydCurve)
    output_file << "Algorithm: LLoyds" << std::endl << std::endl;
  else if (am == lsh_frechet)
    output_file << "Algorithm: LSH_Frechet" << std::endl << std::endl;

  for (int i = 0; i < k_medians; i++){
    std::vector<ts_point> tmp = centroids->at(i).get_ts();
    output_file << "Cluster-" << i+1 << " { size: " << clusters[i].size() << ", centroid: ";
    output_file << "[ " ;
    for (int i = 0; i < tmp.size(); i++){
      output_file << "(" << tmp[i].first << ", " << tmp[i].second << ") ";
    }
    output_file << " ]" <<  std::endl;
    output_file << std::endl;
    // for (int j = 0; j < centroids[i].size(); j++){
    //
    //   if ( j == centroids[i].size()-1 )
    //     output_file << centroids[i][j] << ") }" << std::endl;
    //
    //   else
    //     output_file << centroids[i][j] << ", ";
    // }

    output_file << std::endl;
  }

  // output_file << "Silhouette: { ";
  // for (int i = 0; i < silhouettes.size(); i++)
  //   output_file << silhouettes[i] << ", ";
  //
  // output_file << s_total << " }" << std::endl << std::endl;

  // if condition
  if (this->complete_flag == 1){
    output_file << std::endl << "Cluster followed by the items assigned in it" << std::endl << std::endl;

    for (int i = 0; i < k_medians; i++){

      output_file << "Cluster-" << i+1 << " { ";

      for (int j = 0; j < clusters[i].size(); j++){

        if ( j == clusters[i].size()-1 )
          output_file << clusters[i][j] << " }" << std::endl;

          else
          output_file << clusters[i][j] << ", ";
        }
        output_file << std::endl;
      }
    }

  output_file.close();
}


// General methods
Clustering_curve_data_item* meanFrechetCurve(Clustering_curve_data_item* item1,Clustering_curve_data_item* item2){
  auto& ts1 = item1->get_ts();
  auto& ts2 = item2->get_ts();


  std::vector<std::vector<std::pair<size_t,size_t>>> bt;
  bt.assign(ts1.size(),std::vector<std::pair<size_t,size_t>>());
  for (int i = 0 ; i < ts1.size() ; i++)
    bt[i].assign(ts2.size(),std::pair<size_t,size_t>());

  // std::cout << "ts1.size() " << ts1.size() << std::endl;
  // std::cout << "ts2.size() " << ts2.size() << std::endl;

  // std::cout << "discreteFrechetBT start" << std::endl;
  discreteFrechetBT(item1,item2,bt);
  // std::cout << "discreteFrechetBT end" << std::endl;


  Clustering_curve_data_item* newCentroid = new Clustering_curve_data_item;

  size_t it1 = ts1.size() - 1;
  size_t it2 = ts2.size() - 1;

  // std::cout << "calculatingMean curve" << std::endl;

  // while (it1 != 0 || it2 != 0){
  std::vector<std::pair<double,double>> reverseCentroid;

  // std::cout << "where dafaq is the error " << std::endl;

  double x = (ts1[it1].first + ts2[it2].first)/2;

  // std::cout << "where dafaq is the error " << std::endl;

  double y = (ts1[it1].second + ts2[it2].second)/2;

  // std::cout << "where dafaq is the error " << std::endl;
  // std::cout << "x is " << x << " y is " << y << std::endl;

  reverseCentroid.emplace_back(std::make_pair(x,y));

  while (it2 != 0 || it2 != 0){
    std::pair<size_t,size_t> pos = bt[it1][it2];
    double x = (ts1[pos.first].first + ts2[pos.second].first)/2;
    double y = (ts1[pos.first].second + ts2[pos.second].second)/2;

    reverseCentroid.emplace_back(std::make_pair(x,y));
    it1--;
    it2--;
  }

  auto iterator = reverseCentroid.end();
  iterator--;
  while (true){
    newCentroid->ts_push_back(*iterator);
    if (iterator == reverseCentroid.begin())
      break;
    iterator--;
  }

  const std::vector<std::pair<double,double>> updatedc = newCentroid->get_ts();

    // for (auto pp : updatedc) std:: cout << pp.first << " " << pp.second << " " << std::endl;
  // std::cout << "---" << std::endl;
  //
  // std::cout << "done calculatingMean curve" << std::endl;

  return newCentroid;
}


double discreteFrechetBT(const Frechet_point *a, const Frechet_point *b, std::vector<std::vector<std::pair<size_t,size_t>>>& bt){
  std::vector<std::pair<double, double>> tsA = a->get_ts();
  std::vector<std::pair<double, double>> tsB = b->get_ts();

  size_t sizeA = tsA.size();
  size_t sizeB = tsB.size();

  // double **maxLength = new double *[sizeA];
  // for (int i = 0; i < sizeB; i++)
  //   maxLength[i] = new double[sizeB];

  // std::cout << "sizeA is " << sizeA << std::endl;
  // std::cout << "sizeB is " << sizeB << std::endl;

  std::vector<std::vector<double>> maxLength;
  maxLength.assign(sizeA, std::vector<double>());

  for (int i = 0; i < sizeA; i++)
    maxLength[i].assign(sizeB, 1.0 * 0);

  for (int i = 0; i < sizeA; i++){
    for (int j = 0; j < sizeB; j++)
      maxLength[i][j] = -1;
  }

  double res = discreteFrechetRecursionBT(a, b, sizeA - 1, sizeB - 1, maxLength, bt);
  // for (int i = 0; i < sizeA; i++)
  //   delete[] maxLength[i];
  // delete[] maxLength;

  return res;
}


double discreteFrechetRecursionBT(const Frechet_point *a, const Frechet_point *b, int i, int j, std::vector<std::vector<double>>& maxLength, std::vector<std::vector<std::pair<size_t, size_t>>>&bt){

  if (i == 0 || j == 0) {
    maxLength[i][j] = calculateDistance(a,b,i,j);
    return maxLength[i][j];
  }

  if (maxLength[i][j] != -1 )
    return maxLength[i][j];

  double min = discreteFrechetRecursionBT(a, b, i - 1, j, maxLength, bt);
  double x = discreteFrechetRecursionBT(a, b, i, j - 1, maxLength, bt);
  double y = discreteFrechetRecursionBT(a, b, i - 1, j - 1, maxLength, bt);

  bt[i][j] = std::make_pair(i-1,j);
  if (x < min) {
    min = x;
    bt[i][j] = std::make_pair(i,j-1);
  }
  if (y < min){
    min = y;
    bt[i][j] = std::make_pair(i-1,j-1);
  }

  double distij = calculateDistance(a,b,i,j);

  if (distij > min)
    maxLength[i][j] = distij;
  else maxLength[i][j] = min;

  return maxLength[i][j];
}

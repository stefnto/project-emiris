#include "Curves.hpp"

double (*Frechet_point::curveDistanceFunction)(const Frechet_point *a, const Frechet_point *b) = nullptr;


// Frechet_point methods
Frechet_point::Frechet_point(std::string line): Data_point(line){

  const std::vector<double>& coordinates_tmp = this->getCoordinates();

  this->altered_coordinates.reserve( 2 * this->get_coordinates_size() );        // reseve double the size of the timeseries coordinates

  for (int i = 0; i < coordinates_tmp.size(); i++){
    ts.emplace_back( std::make_pair( i * 1.0, coordinates_tmp[i] ) );
  }
}


Frechet_point::Frechet_point(Frechet_point* point){
  this->set_item_id(point->get_item_id());
  this->coordinates = point->getCoordinates();

  this->distanceFromQuery = point->getDistanceFromQuery() ;

  const std::vector<double>& coordinates_tmp = this->getCoordinates();

  this->altered_coordinates.reserve( 2 * this->get_coordinates_size() );        // reseve double the size of the timeseries coordinates

  for (int i = 0; i < coordinates_tmp.size(); i++){
    ts.emplace_back( std::make_pair( i * 1.0, coordinates_tmp[i] ) );
  }

}


Frechet_point::Frechet_point(std::string id,const std::vector<double>& c):Data_point(id,c){
  size_t x = 1;
  for (double y : c){
    ts.emplace_back(1.0*x,y);
    x++;
  }

}


void Frechet_point::setDistanceFromQuery(Data_query* query){
  Frechet_query* qquery = dynamic_cast<Frechet_query*>(query);
  Frechet_point b(qquery);
  this->distanceFromQuery = Frechet_point::curveDistanceFunction(this,&b);
}


void Frechet_point::ID_push_back(long ID) {
    this->IDs.push_back(ID);
}


long Frechet_point::getID(int pos) const {
    return this->IDs[0];
}


void Frechet_point::print_altered_coordinates(){
  for (int i = 0; i < this->altered_coordinates.size(); i++){
    std::cout << this->altered_coordinates[i] << " " ;
  }
  std::cout << std::endl;
}



// Frechet_query methods
Frechet_query::Frechet_query(std::string line): Data_query(line){

  const std::vector<double>& coordinates_tmp = this->getCoordinates();

  this->altered_coordinates.reserve( 2 * this->get_coordinates_size() );        // reserve double the size of the timeseries coordinates

  for (int i = 0; i < coordinates_tmp.size(); i++){
    ts.emplace_back( std::make_pair( i * 1.0, coordinates_tmp[i] ) );
  }

}


Frechet_query::Frechet_query(Frechet_query* query){

  this->set_item_id(query->get_item_id());
  this->coordinates = query->getCoordinates();

  this->setAlgorithmTime( query->getAlgorithmTime() );
  this->setBruteForcetime( query->getBruteForceTime() );
  this->setShorterDistance( query->getShorterDistance() );

  const std::vector<double>& coordinates_tmp = this->getCoordinates();

  this->altered_coordinates.reserve( 2 * this->get_coordinates_size() );        // reserve double the size of the timeseries coordinates

  for (int i = 0; i < coordinates_tmp.size(); i++){
    ts.emplace_back( std::make_pair( i * 1.0, coordinates_tmp[i] ) );
  }
}


void Frechet_query::ID_push_back(long ID) {
    this->IDs.push_back(ID);
}


long Frechet_query::getID(int pos) const {
    return this->IDs[0];
}


void Frechet_query::print_altered_coordinates(){
  for (int i = 0; i < this->altered_coordinates.size(); i++){
    std::cout << this->altered_coordinates[i] << " " ;
  }
  std::cout << std::endl;
}



// Discrete_snapping methods
Discrete_snapping::Discrete_snapping(double delta):delta(delta){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0,delta);
    this->t = distribution(generator);
}


void Discrete_snapping::operator()(Frechet_point* elem){                        // performs snapping, duplicate removal and concatenation(vectorization)
  const std::vector<ts_point>& points = elem->get_ts();
  double prevFirst = -1;

  double prevSecond = -1;

  for (ts_point point : points){

    double first = floor( ( ( point.first - t ) / delta ) + 0.5 ) * delta + t;
    double second = floor( ( ( point.second - t ) / delta ) + 0.5 ) * delta + t;

    if ( prevFirst != first || prevSecond != second ) {                     // instead of removing duplicate (p_i, p_j), dont add a (p_i, p_j) if it has been added before
      elem->altered_coordinates_emlace_back(first);
      elem->altered_coordinates_emlace_back(second);

      prevFirst = first;
      prevSecond = second;
    }
  }
}


void Discrete_snapping::operator()(Frechet_query* elem){                        // performs snapping, duplicate removal and concatenation(vectorization)
  const std::vector<ts_point>& points = elem->get_ts();

  double prevFirst = -1;

  double prevSecond = -1;

  for (ts_point point : points){

    double first = floor( ( ( point.first - t ) / delta ) + 0.5 ) * delta + t;
    double second = floor( ( ( point.second - t ) / delta ) + 0.5 ) * delta + t;

    if ( prevFirst != first || prevSecond != second ) {                     // instead of removing duplicate (p_i, p_j), dont add a (p_i, p_j) if it has been added before
      elem->altered_coordinates_emlace_back(first);
      elem->altered_coordinates_emlace_back(second);

      prevFirst = first;
      prevSecond = second;
    }
  }
}



// Continuous_snapping methods
Continuous_snapping::Continuous_snapping(double delta):delta(delta){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0,delta);
    this->t = distribution(generator);
}


void Continuous_snapping::operator()(Frechet_point* elem){                      // performs snapping, minMaxFunction
  const std::vector<double>& filtered = elem->getCoordinates();

  std::vector<double> snapped_timeseries;

  double prevSecond = -1;

  for (double coordinate : filtered){
    double second = floor(  ( coordinate + t ) / delta  ) * delta;

    if (prevSecond != second) {                                                 // don't add consecutive duplicates
      snapped_timeseries.emplace_back(second);
      prevSecond = second;
      }
    }

  minMaxFunction(snapped_timeseries);

  elem->setMinimaMaxima(snapped_timeseries);
}


void Continuous_snapping::operator()(Frechet_query* elem){                      // performs snapping, minMaxFunction
  const std::vector<double>& filtered = elem->getCoordinates();

  std::vector<double> snapped_timeseries;

  double prevSecond = -1;

  for (double coordinate : filtered){
    double second = floor(  ( coordinate + t ) / delta  ) * delta;

    if (prevSecond != second) {                                                 // don't add consecutive duplicates
      snapped_timeseries.emplace_back(second);
      prevSecond = second;
    }
  }

  minMaxFunction(snapped_timeseries);

  elem->setMinimaMaxima(snapped_timeseries);
}


void Continuous_snapping::minMaxFunction(std::vector<double>& timeseries){
  if (timeseries.size() > 1) {
    for (int i = 1; i < timeseries.size(); i++){

      if ( i + 1 == timeseries.size() )
        break;

      double min = std::min( timeseries[i-1], timeseries[i+1] );                // get min
      double max = std::max( timeseries[i-1], timeseries[i+1] );                // get max

      double a = timeseries[i-1];
      double b = timeseries[i];
      double c = timeseries[i+1];

      if (b >= min && b <= max){                                                // if b ∈ [min, max]
        timeseries.erase( timeseries.begin() + i);                              // erase it
        i--;
      }
    }
  }
}



// Frechet_solver methods
// Constrctor for Discrete Frechet
Frechet_solver::Frechet_solver(std::string dataset_path, std::string query_path, std::string output_filepath, std::string algorithm, int k, int l, int n, int r, double (*distanceFunction)(const std::vector<double>& a, const std::vector<double>& b))
  : Solver(n, r, output_filepath), l(l), algorithm(algorithm){
  double sttime, endtime;                                                       // to compute total run time
  this->hashTables = new Frechet_HashTable[l];

  Frechet_point::setCurveDistanceFunction(discreteFrechet);

  int itemsRead = readItems(dataset_path, points_coordinates);                  // reads and inserts items to points_coordinates

  int queriesRead = readItems(query_path, queries);

  this->w = 200;

  if ( itemsRead ) {

    int itemDim = points_coordinates[0]->get_coordinates_size();
    itemDim *= 2;

    for (int i = 0 ; i < l ; i++){

      double delta = 10;
      hashTables[i].init(itemDim, itemsRead/8, k, w, new Discrete_snapping(delta));                           // initialize each hash table

    }
    for (Frechet_point* item : points_coordinates){

      for (int i = 0 ; i < l ; i ++)
        hashTables[i].insertDiscrete(item);                                                                   // insert a pointer pointing to the item on "points_coordinates" in each hashtable

    }
  }else std::cout << "dataset_path is empty" << std::endl;
  std::cout << "HASHING DONE" << std::endl;
}


// Constrctor for Continuous Frechet
Frechet_solver::Frechet_solver(std::string dataset_path, std::string query_path, std::string output_filepath, std::string algorithm, int k, int n, int r, double (*distanceFunction)(const std::vector<double>& a, const std::vector<double>& b))
  : Solver(n, r, output_filepath), l(1), algorithm(algorithm){
  double sttime, endtime;                                                       // to compute total run time
  this->hashTables = new Frechet_HashTable;

  int itemsRead = readItems(dataset_path, points_coordinates);                  // reads and inserts items to points_coordinates

  int queriesRead = readItems(query_path, queries);

  this->dim = points_coordinates[0]->get_coordinates_size();

  this->w = ceil(dim/500.0) * 500;

  if ( itemsRead ) {
    int itemDim = points_coordinates[0]->get_coordinates_size();
    itemDim *= 2;

    double delta = 10;
    hashTables->init(itemDim, itemsRead/16, k, w, new Continuous_snapping(delta));                           // initialize each hash table

    for (Frechet_point* item : points_coordinates){
        hashTables->insertContinuous(item);                                                                  // insert a pointer pointing to the item on "points_coordinates" in each hashtable
    }
  }else std::cout << "dataset_path is empty" << std::endl;
  std::cout << "HASHING DONE" << std::endl;


}


bool Frechet_solver::solveDiscreteFrechet(){
  LSH_Set* result_approx_NN;                                                    // ordered set of ANNs for each query

  LSH_Set* result_true_NN;                                                      // ordered set of true NNs for each query

  double maf = 0.0;
  double tAproxSum = 0.0;
  double tTrueSum = 0.0;

  for (Frechet_query* query : queries){
    result_approx_NN = NNandRSDiscrete(query);

    result_true_NN = bruteForceSearchDiscrete(query, this->points_coordinates, this->n);

    writeResult(result_approx_NN, query, result_true_NN);

    double tmp_maf = ( ( *std::next(result_approx_NN->begin(), 0 ) )->getDistanceFromQuery() / ( *std::next(result_true_NN->begin(), 0 ) )->getDistanceFromQuery() );

    if (tmp_maf > maf)
      maf = tmp_maf;

    tAproxSum += query->getAlgorithmTime();

    tTrueSum += query->getBruteForceTime();


    delete result_approx_NN;
    result_true_NN->clear();
  }

  std::ofstream output_file;                                                    //  to write tApproximateAverage, tTrueAverage, MAF
  output_file.open(output_filepath, std::ofstream::out | std::ofstream::app);

  output_file << std::endl;
  output_file << "tApproximateAverage: " << tAproxSum / queries.size() << std::endl;
  output_file << "tTrueAverage: " << tTrueSum / queries.size() << std::endl;
  output_file << "MAF: " << maf << std::endl;

  return true;
}


bool Frechet_solver::solveContinuousFrechet(){

  LSH_Set* result_approx_NN;                                                    // ordered set of ANNs for each query

  LSH_Set* result_true_NN;                                                      // ordered set of true NNs for each query

  Curves frechet_points_as_curves(1);

  for (Frechet_point* point: this->points_coordinates){                         // make a temporary Curves class that holds points_coordinates as Curves to use on bruteForceSearch
    Curve pointCurve (1, point->get_item_id());
    constructFredFrechetCurve(point, pointCurve);
    frechet_points_as_curves.add(pointCurve);
  }

  double maf = 0.0;
  double tAproxSum = 0.0;
  double tTrueSum = 0.0;

  for (Frechet_query* query : queries){
    result_approx_NN = NNandRSContinuous(query);

    result_true_NN = bruteForceSearchContinuous(query, this->points_coordinates, this->n, frechet_points_as_curves);

    writeResult(result_approx_NN, query, result_true_NN);

    double tmp_maf = ( ( *std::next(result_approx_NN->begin(), 0 ) )->getDistanceFromQuery() / ( *std::next(result_true_NN->begin(), 0 ) )->getDistanceFromQuery() );

    if (tmp_maf > maf)
      maf = tmp_maf;

    tAproxSum += query->getAlgorithmTime();

    tTrueSum += query->getBruteForceTime();

    delete result_approx_NN;
    result_true_NN->clear();
  }

  std::ofstream output_file;                                                    //  to write tApproximateAverage, tTrueAverage, MAF
  output_file.open(output_filepath, std::ofstream::out | std::ofstream::app);

  output_file << std::endl;
  output_file << "tApproximateAverage: " << tAproxSum / queries.size() << std::endl;
  output_file << "tTrueAverage: " << tTrueSum / queries.size() << std::endl;
  output_file << "MAF: " << maf << std::endl;

  return true;
}


LSH_Set* Frechet_solver::NNandRSDiscrete(Frechet_query* query){

  auto comp = [](const Data_point* a,const Data_point* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  LSH_Set*  nn = new LSH_Set(comp);                                             // ordered_set that contains NNs

  double sttime, endtime;                                                       // to compute total run time

  sttime = ((double) clock())/CLOCKS_PER_SEC;

  for (int i = 0; i < l; i++){
    this->hashTables[i].NearestNeighboursDiscrete(query, nn, i);                         // search NN of query for each hashTable
  }
  endtime =( (double) clock())/CLOCKS_PER_SEC;
  query->setAlgorithmTime( endtime - sttime );

  return nn;
}


LSH_Set* Frechet_solver::NNandRSContinuous(Frechet_query* query){
  auto comp = [](const Data_point* a,const Data_point* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  LSH_Set*  nn = new LSH_Set(comp);                                             // ordered_set that contains NNs

  double sttime, endtime;                                                       // to compute total run time

  sttime = ((double) clock())/CLOCKS_PER_SEC;

  this->hashTables->NearestNeighboursContinuous(query, nn, 0);                         // search NN of query for each hashTable

  endtime = ((double) clock())/CLOCKS_PER_SEC;

  query->setAlgorithmTime( endtime - sttime );

  return nn;
}


void Frechet_solver::printQueries() const {
  int i = 1;
  for (Data_query* query : queries)
    std::cout << "query " << i++ << " : " << query->get_item_id() << std::endl;
}


void Frechet_solver::writeResult(LSH_Set* result, Data_query* query, LSH_Set* true_nn){
  std::ofstream output_file;
  output_file.open(output_filepath, std::ofstream::out | std::ofstream::app);


  output_file << "Query : " << query->get_item_id() << std::endl;
  output_file << "Algorithm: " << this->algorithm << std::endl;

  if (result->size() == 0){
    output_file << "No elements were found near this query using LSH" << std::endl;
    output_file << "True Nearest neighbor : " << ( *std::next(true_nn->begin(), 0) )->get_item_id() << std::endl;
    output_file << "distanceTrue : " << ( *std::next(true_nn->begin(), 0) )->getDistanceFromQuery() << std::endl;
    output_file << "tTrue : " << query->getBruteForceTime() << std::endl << std::endl;
  }
  else {
    auto it = true_nn->begin();
    int counter = 0;

    for (Data_point* point : *result){
      if (counter == this->n)
        break;

      output_file << "Approximate Nearest neighbor: " << point->get_item_id() << std::endl;
      output_file << "True Nearest neighbor: " << ( *std::next(true_nn->begin(), counter) )->get_item_id() << std::endl;
      output_file << "distanceApproximate : " << point->getDistanceFromQuery() << std::endl;
      output_file << "distanceTrue : " << ( *std::next(true_nn->begin(), counter) )->getDistanceFromQuery() << std::endl;
      output_file << "tLSH : " << query->getAlgorithmTime() << std::endl;
      output_file << "tTrue : " << query->getBruteForceTime() << std::endl << std::endl;
      counter++;
    }
  }
}


Frechet_solver::~Frechet_solver(){
  if (this->l > 1)
    delete[] this->hashTables;
  else if (this->l == 1)
    delete this->hashTables;
  for (Data_item* item : this->points_coordinates) delete item;
  // std::cout << "deleted points_coordinates" << std::endl;
  for (Data_item* item : this->queries) delete item;
  // std::cout << "deleted queries" << std::endl;
}



// Frechet_HashTable Methods
Frechet_HashTable::Frechet_HashTable(int itemDim, unsigned long long tableSize, int k, int w, snapping *snapper)
    : HashTable(k, tableSize), hashingFunction(itemDim, k, tableSize, w),snapper(snapper){

    this->buckets = new std::list<Data_point *>[tableSize];
}


void Frechet_HashTable::init(size_t itemDim, unsigned long long tableSize, int k, int w, snapping *snapper){
  this->k = k;
  this->size = tableSize;
  this->buckets = new std::list<Data_point *>[tableSize];
  this->snapper = snapper;
  this->itemDim = itemDim;

  this->hashingFunction.init(itemDim, k, tableSize, w);
}


Frechet_HashTable::~Frechet_HashTable(){
  delete[] this->buckets;
}


void Frechet_HashTable::insertDiscrete(Frechet_point* item){
  Frechet_point* cItem = new Frechet_point(item);                               // creating another Frechet_point

  snapper->operator()(cItem);                                                   // snapping the element

  padding(cItem, 500);                                                          // filling the array

  int index = this->hashingFunction(cItem, &cItem->get_altered_coordinates());

  this->buckets[index].emplace_back(cItem);
}


void Frechet_HashTable::insertContinuous(Frechet_point* cItem){

  std::vector<double> timeseries_filtered;

  timeseries_filtered = cItem->getCoordinates();

  filtering(timeseries_filtered);                                               // filtering the timeseries

  cItem->set_coordinates(timeseries_filtered);

  snapper->operator()(cItem);                                                   // snapping the element

  padding(cItem, 500);                                                          // padding the vector

  int index = this->hashingFunction(cItem, &cItem->get_altered_coordinates());
  this->buckets[index].push_back(cItem);
}


void Frechet_HashTable::NearestNeighboursDiscrete(Frechet_query* query, LSH_Set* ordSet, int index){
  Frechet_query* frechetQuery = new Frechet_query(query);

  this->snapper->operator()(frechetQuery);

  this->padding(frechetQuery, 500);

  int pos = this->hashingFunction(frechetQuery, &frechetQuery->get_altered_coordinates());

  for (Data_point* point : this->buckets[pos]){
    if ( frechetQuery->getID(index) == point->getID(index) ){
      point->setDistanceFromQuery(frechetQuery);
      ordSet->insert(point);
    }
  }

}


void Frechet_HashTable::NearestNeighboursContinuous(Frechet_query* query, LSH_Set* ordSet, int index){

  std::vector<double> timeseries_filtered;

  timeseries_filtered = query->getCoordinates();

  filtering(timeseries_filtered);                                               // filtering the timeseries

  query->set_coordinates(timeseries_filtered);                                  // set filtered timeseries as the coordinates that Frechet Continuous will use

  snapper->operator()(query);                                                   // snapping the element

  this->padding(query, 500);                                                    // padding the vector

  Curve queryCurve(1, query->get_item_id());

  constructFredFrechetCurve(query, queryCurve);

  int pos = this->hashingFunction(query, &query->get_altered_coordinates());

  int count = 0;

  for (Data_point* point : this->buckets[pos]){
    if ( query->getID(index) == point->getID(index) ){
      count++;
      Frechet_point* frechetPoint = dynamic_cast<Frechet_point*>(point);

      Curve pointCurve(1, frechetPoint->get_item_id());

      constructFredFrechetCurve(frechetPoint, pointCurve);

      Frechet::Continuous::Distance distance = Frechet::Continuous::distance(queryCurve, pointCurve);

      point->setDistanceFromQuery(distance.value);

      ordSet->insert(point);
    }
  }

  if (count <= this->buckets[pos].size()/5){                                    // if less than 1/5 of the bucket size is checked with the ID method
    for (Data_point* point : this->buckets[pos]){                               // check all points in the bucket

      Frechet_point* frechetPoint = dynamic_cast<Frechet_point*>(point);

      Curve pointCurve(1, frechetPoint->get_item_id());

      constructFredFrechetCurve(frechetPoint, pointCurve);

      Frechet::Continuous::Distance distance = Frechet::Continuous::distance(queryCurve, pointCurve);

      point->setDistanceFromQuery(distance.value);

      ordSet->insert(point);
    }
  }

}



// General methods
double calculateDistance(const Frechet_point* a,const Frechet_point* b,size_t i,size_t j){
  const std::pair<double,double>& axy = a->get_ts(i) ,bxy = b->get_ts(j);
  std::vector<double> avec,bvec;
  avec.push_back(axy.first);
  avec.push_back(axy.second);
  bvec.push_back(bxy.first);
  bvec.push_back(bxy.second);

  return a->getDistanceFunction()(avec, bvec);
}


double EuclidianDistance(const Frechet_point *a,const Frechet_point *b){
  return EuclidianDistance(a->getConstCoordinates(),b->getConstCoordinates());
}


double discreteFrechet(const Frechet_point* a, const Frechet_point* b){
  std::vector<std::pair<double,double>> tsA = a->get_ts();
  std::vector<std::pair<double,double>> tsB = b->get_ts();

  int sizeA = tsA.size();
  int sizeB = tsB.size();

  double** maxLength = new double*[sizeA];
  for (int i = 0 ; i < sizeB; i++) maxLength[i] = new double[sizeB];


  for (int i = 0 ; i < sizeA; i++){
    for (int j = 0 ; j < sizeB; j++)
      maxLength[i][j] = -1;
  }

  double res = discreteFrechetRecursion(a,b,sizeA-1,sizeB-1,maxLength);

  for (int i = 0 ; i < sizeA; i++)
    delete[] maxLength[i];

  delete[] maxLength;

  return res;
}


double discreteFrechet(const std::vector<double>& a,const std::vector<double>& b,double (*distFunction)(const std::vector<double>& a, const std::vector<double>& b)){
  int sizeA = a.size();
  int sizeB = b.size();

  double** maxLength = new double*[sizeA];
  for (int i = 0 ; i < sizeB; i++) maxLength[i] = new double[sizeB];


  for (int i = 0 ; i < sizeA; i++){
    for (int j = 0 ; j < sizeB; j++)
      maxLength[i][j] = -1;
  }

  double res = discreteFrechetRecursion(a,b,sizeA-1,sizeB-1,maxLength,distFunction);

  for (int i = 0 ; i < sizeA; i++)
    delete[] maxLength[i];

  delete[] maxLength;

  return res;
}


double discreteFrechetRecursion(const Frechet_point *a, const Frechet_point *b,int i,int j,double** maxLength/* vector2d& maxLength */){

  if (i == 0 || j == 0) {
    maxLength[i][j] = calculateDistance(a,b,i,j);
    return maxLength[i][j];
  }

  if (maxLength[i][j] != -1 )
    return maxLength[i][j];

  double min = discreteFrechetRecursion(a,b,i-1,j,maxLength);
  double x = discreteFrechetRecursion(a,b,i,j-1,maxLength);
  double y = discreteFrechetRecursion(a,b,i-1,j-1,maxLength);

  if (x < min) min = x;
  if (y < min) min = y;

  double distij = calculateDistance(a,b,i,j);

  if (distij > min)
    maxLength[i][j] = distij;
  else
    maxLength[i][j] = min;

  return maxLength[i][j];

}


double discreteFrechetRecursion(const std::vector<double>& a, const std::vector<double>& b,int i,int j,double** maxLength,double (*distFunction)(const std::vector<double>&a,const std::vector<double>& b)){

  if (i == 0 || j == 0) {
    maxLength[i][j] = distFunction({(double)i,a[i]},{(double)j,b[j]});
    return maxLength[i][j];
  }

  if (maxLength[i][j] != -1 )
    return maxLength[i][j];

  double min = discreteFrechetRecursion(a,b,i-1,j,maxLength,distFunction);
  double x = discreteFrechetRecursion(a,b,i,j-1,maxLength,distFunction);
  double y = discreteFrechetRecursion(a,b,i-1,j-1,maxLength,distFunction);

  if (x < min) min = x;
  if (y < min) min = y;

  double distij = distFunction({(double)i,a[i]},{(double)j,b[j]});;

  if (distij > min)
    maxLength[i][j] = distij;
  else
    maxLength[i][j] = min;

  return maxLength[i][j];
}


LSH_Set* bruteForceSearchDiscrete(Frechet_query* query, std::vector<Frechet_point*>& points_coordinates, int n){
  auto comp = [](const Data_point* a,const Data_point* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  LSH_Set*  result_true_NN = new LSH_Set(comp);

  double sttime, endtime;                                                       // to compute total run time

  sttime = ((double) clock())/CLOCKS_PER_SEC;

  std::vector<ts_point> ts1;                                                    //[(1,y1)(2,y2)...(n,yn)]

  ts1 = query->get_ts();

  Frechet_point fpoint(query);                                                  // make Frechet_query a Frechet_point to call curveDistanceFunction

  for (Frechet_point* point: points_coordinates){
    point->setDistanceFromQuery(query);

    if (result_true_NN->size() < n){                                            // if size of set less than n, add the distance
      result_true_NN->insert(point);
    }
    else if (result_true_NN->size() >= n){
      if (point->getDistanceFromQuery() < ( *result_true_NN->rbegin() )->getDistanceFromQuery() ){   // if distance is less than biggest distance in set

        result_true_NN->erase( *result_true_NN->rbegin() );                     // erase the biggest distance that is saved on set
        result_true_NN->insert(point);                                          // insert new distance

      }
    }
  }

  endtime = ((double) clock())/CLOCKS_PER_SEC;

  query->setBruteForcetime( endtime - sttime);

  return result_true_NN;
}


LSH_Set* bruteForceSearchContinuous(Frechet_query* query, std::vector<Frechet_point*>& points_coordinates, int n, Curves& frechet_points_as_curves){
  auto comp = [](const Data_point* a,const Data_point* b) -> bool {return a->getDistanceFromQuery() < b->getDistanceFromQuery();};
  LSH_Set*  result_true_NN = new LSH_Set(comp);

  double sttime, endtime;                                                       // to compute total run time

  Curve queryCurve(1, query->get_item_id());

  constructFredFrechetCurve(query, queryCurve);

  sttime = ((double) clock())/CLOCKS_PER_SEC;

  int count = 0;
  for (Curve pointCurve: frechet_points_as_curves){

    Frechet::Continuous::Distance distance_t = Frechet::Continuous::distance(queryCurve, pointCurve);

    if (result_true_NN->size() < n){                                                                  // if size of set less than n, add the distance
      Data_point* p = new Data_point(pointCurve.get_name(), distance_t.value, 0);                     // make a Data_point out of pointCurve
      result_true_NN->insert(p);
    }
    else if (result_true_NN->size() >= n){
      if (distance_t.value < ( *result_true_NN->rbegin() )->getDistanceFromQuery()){                  // if distance is less than biggest distance in set
        Data_point* p = new Data_point(pointCurve.get_name(), distance_t.value, 0);                   // make a Data_point out of pointCurve

        result_true_NN->erase( *result_true_NN->rbegin() );                                           // erase the biggest distance that is saved on set
        result_true_NN->insert(p);                                                                    // insert new distance
      }
    }
  }

  endtime = ((double) clock())/CLOCKS_PER_SEC;

  query->setBruteForcetime( endtime - sttime);

  return result_true_NN;
}


void filtering(std::vector<double>& timeseries){                                // type T is Frechet_point or Frechet_query

  int e = 1;
  for (int i = 0; i < timeseries.size(); i++){

    if ( i + 2 > timeseries.size() )                                            // if 'c' is out of bounds stop filtering
      break;

    double a = timeseries[i];
    double b = timeseries[i+1];
    double c = timeseries[i+2];

    if ( abs(a-b) <= e && abs(b-c) <= e ){                                      // if condition true
      timeseries.erase( timeseries.begin() + i+1 );                             // erase 'b' from vector
    }
  }
}

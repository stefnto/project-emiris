#ifndef __UTILS__HPP__
#define __UTILS__HPP__

#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <chrono>
#include <fstream>
#include <utility>
#include <set>

class item_Exception{};


double EuclidianDistance(const std::vector<double>& a,const std::vector<double>& b);


class Data_item;
class Data_point;
class Data_query;

class Data_item {
  protected:
    std::string item_id;                                                        // is id from input_file
    std::vector<double> coordinates;
    std::vector<long> IDs;

    static double (*distanceFunction)(const std::vector<double> &a, const std::vector<double> &b);

    void set_item_id(std::string id){this->item_id = id;}

  public:
    Data_item(){};
    Data_item(std::string item_id, std::vector<double> coordinates);
    Data_item(std::string line);
    Data_item(std::string item_id,size_t size): item_id(item_id){coordinates.reserve(size);}

    virtual ~Data_item(){};

    void set_coordinates(std::vector<double>& coords){this->coordinates = coords;}

    virtual const std::vector<double>& getCoordinates() const;

    void c_push_back(double c){this->coordinates.push_back(c);}

    void print_coordinates();

    int get_coordinates_size();

    std::string get_item_id() const;

    int getIDs_size(){return this->IDs.size();}

    virtual void ID_push_back(long id) { IDs.push_back(id); }

    virtual long getID(int pos) const { return IDs[pos]; }

    static void setDistanceFunction(double (*distanceFunction)(const std::vector<double> &a, const std::vector<double> &b));

    using ptrF =  double (*)(const std::vector<double> &a, const std::vector<double> &b);
    ptrF getDistanceFunction() const {return this->distanceFunction;}
};


class Data_point: public Data_item {
  protected:
    // long ID;                                                                    // ID computed from Σ (r * h) mod M
    double distanceFromQuery = 0;                                               // distance of item from each query checked at a time

  public:
    Data_point(){};
    Data_point(std::string item_id, std::vector<double> coordinates): Data_item(item_id, coordinates){};
    Data_point(std::string line): Data_item(line){};
    Data_point(std::string item_id,size_t size):Data_item(item_id,size){};
    Data_point(std::string item_id, double distance, int i): distanceFromQuery(distance){this->item_id = item_id;};   // i works as place holder
    ~Data_point(){};



    virtual void setDistanceFromQuery(Data_query* query);
    void setDistanceFromQuery(double distanceFromQuery);
    double getDistanceFromQuery() const;

};


class Data_query: public Data_item {
  private:
    // long ID;                                                                    // ?
    double algorithmTime = 0;
    double bruteforceTime = 0;
    double shorterDistance = 0;                                                 // used in brute force method

  public:
    Data_query(){};
    Data_query(const Data_query&) = default;
    Data_query(std::string item_id,size_t size):Data_item(item_id,size){}
    Data_query(std::string item_id, std::vector<double> coordinates): Data_item(item_id, coordinates){};
    Data_query(std::string line): Data_item(line){};



    void setAlgorithmTime(double time);
    void setBruteForcetime(double time);
    void setShorterDistance(double value);
    double getAlgorithmTime();
    double getBruteForceTime();
    double getShorterDistance();

};


class Solver {
  protected:
    int n;                                                                      // number of Nearest Neighbours we're looking for
    int r;                                                                      // the search is made inside the r radius
    std::string output_filepath;

  public:
    Solver(int n, int r, std::string output_filepath, double (*distanceFunction)(const std::vector<double>& a, const std::vector<double>& b) = EuclidianDistance);
    Solver(int n, int r, double (*distanceFunction)(const std::vector<double>& a, const std::vector<double>& b) = EuclidianDistance);
    virtual ~Solver(){};
};


class HashTable {
  protected:
    int k;
    unsigned long long size;

  public:
    HashTable(){};
    HashTable(int k, int size) :k(k), size(size){};
    virtual ~HashTable(){};
};


class hFunction{                                                                // floor( (p*v + t)/ w )
  private:
    std::vector<float> v;                                                       //contains vector V
    float t;
    const int w;

  public:
    hFunction(int itemSize, int w);
    int operator()(const Data_item* item, const std::vector<double>* altered_coordinates = nullptr);
    std::vector<float>& getv();
};


long mod(long x, long y);





double EuclidianDistance(const std::vector<double>& a, const std::vector<double>&b,double (*f)(const std::vector<double>&aa,const std::vector<double>&bb));


int rGenerator();


void getNumbersWithHammingDistance(int k, unsigned long long number, int probes, std::set<unsigned long long>& set);


void bruteForceSearch(Data_query *query, std::vector<Data_point*>& points_coordinates, int n, std::set<double>& true_nn_distances);


void readConfig(std::string config_file, int& k_lsh, int& l_lsh, int& k_medians, int& m_cube, int& k_cube, int& probes_cube);


template <typename T>
int readItems(std::string dataset_path, std::vector<T *> &container){
    std::ifstream datafile;
    int counter = 0;
    datafile.open(dataset_path);
    if (datafile.is_open())
    {
        std::string line;
        double sttime, endtime;                                                 // to compute total run time
        sttime = ((double)clock()) / CLOCKS_PER_SEC;
        while (getline(datafile, line)){
            counter++;
            container.emplace_back(new T(line));                                // put newly constructed 'T' object at the end of container
        }
        endtime = ((double)clock()) / CLOCKS_PER_SEC;
        datafile.close();
        std::cout << "time needed to read elements : " << endtime - sttime << std::endl;
    }
    return counter;
}


template<typename T>
int avgDistance(std::vector<T*>& points_coordinates){
  srand(time(NULL));
  std::set<int> set;
  double dist = 0;
  int num = ( 5 * points_coordinates.size() ) / 1000;                           // for 1000 points check 5 vectors to find mean distance
                                                                                // so for y points in the data set, x = ( 5 * y ) / 1000 number of vectors will be checked for mean distance
  for (int i = 0; i < num; i++){
    int tmp = rand() % points_coordinates.size();

    auto search = set.find(tmp);
    if (search == set.end()){
      set.insert(tmp);
    }

    for (std::set<int>::iterator it = std::next(set.begin(), 1); it!=set.end(); ++it){  // compute each distance of first point to the other 'set.size() - 1' points
      dist += EuclidianDistance(points_coordinates[*set.begin()]->getCoordinates(), points_coordinates[*it]->getCoordinates());
    }
    dist = dist / set.size();                                                   // get the mean distance
  }
  return dist;
}


#endif

#ifndef __LSH_CURVES_HPP__
#define __LSH_CURVES_HPP__


#include <sstream>
#include <fstream>
#include <math.h>
#include <utility>
#include <unistd.h>
#include <string.h>
#include <algorithm>

#include "LSH.hpp"
#include "utils.hpp"

#include "../fred-frechet/include/frechet.hpp"


using ts_point = std::pair<double,double>;


class Frechet_query : public Data_query {
  private:
  std::vector<ts_point> ts; //[(1,y1)(2,y2)...(n,yn)]

  std::vector<double> altered_coordinates;

public:
  Frechet_query(Frechet_query* query);

  Frechet_query(std::string line);

  void altered_coordinates_emlace_back(double num){ this->altered_coordinates.emplace_back(num);}

  int get_altered_coordinates_size(){return this->altered_coordinates.size();}

  const std::vector<double>& get_altered_coordinates() const {return this->altered_coordinates;}

  const std::vector<ts_point>& get_ts() const { return this->ts; }

  const std::pair<double,double>& get_ts(size_t i) {return this->ts[i];}

  void ID_push_back(long ID) override;

  long getID(int pos) const override;

  void setMinimaMaxima(std::vector<double>& minmax_result){this->altered_coordinates = minmax_result;}

  void print_altered_coordinates();
};


class Frechet_point : public Data_point {
  protected:
    std::vector<ts_point> ts;                                                                   //[(1,y1)(2,y2)...(n,yn)]
    static double (*curveDistanceFunction)(const Frechet_point* a,const Frechet_point*b);

    std::vector<double> altered_coordinates;

  public:
    Frechet_point(Data_point* point):Data_point(point->get_item_id(),2*point->get_coordinates_size()){
      const std::vector<double>& coordinates = point->getCoordinates();
      for (int i = 0; i < coordinates.size(); i++){
        ts.emplace_back( std::make_pair( i * 1.0, coordinates[i] ) );
      }
    }

    Frechet_point() : Data_point() {}

    Frechet_point(Frechet_point* point);

    Frechet_point(Frechet_query* query):ts(query->get_ts()){}

    Frechet_point(std::string line);

    Frechet_point(std::string id,const std::vector<double>& c);

    void ts_push_back(const ts_point& tspoint){ this->ts.emplace_back(tspoint);}

    std::vector<double>& getCoordinates(){ return this->coordinates;}

    const std::vector<double>& getConstCoordinates() const { return this->coordinates;}

    double& refAt(size_t pos){return coordinates.at(pos);}

    void altered_coordinates_emlace_back(double num){ this->altered_coordinates.emplace_back(num);}

    int get_altered_coordinates_size(){return this->altered_coordinates.size();}

    void print_altered_coordinates();

    const std::vector<double>& get_altered_coordinates() const {return this->altered_coordinates;}

    const std::vector<ts_point>& get_ts() const { return this->ts;}

    const std::pair<double, double> &get_ts(size_t i) const { return this->ts[i]; }

    static void setCurveDistanceFunction(double (*cdf)(const Frechet_point* a, const Frechet_point*b)){
      curveDistanceFunction = cdf;
    };

    using ptrDF =  double (*)(const Frechet_point *a, const Frechet_point *b);

    ptrDF getCurveDistanceFunction() const {return this->curveDistanceFunction;}

    void setDistanceFromQuery(Data_query*) override;

    void ID_push_back(long ID) override;

    long getID(int pos) const override;

    void setMinimaMaxima(std::vector<double>& minmax_result){this->altered_coordinates = minmax_result;}
};


class snapping{
  public:
    using ts_point = std::pair<double, double>;
    virtual void operator()(Frechet_point *)=0;                                 // this is used to convert ts_vector [(1,y1)(2,y2)...(n,yn)] -> [1,y1,2,y2,...,r,yr]
    virtual void operator()(Frechet_query*)=0;                                  // -//-
    virtual ~snapping() = default;

};


class Discrete_snapping: public snapping{
  private:
    const double delta;
    double t;

  public:
    Discrete_snapping(double delta);
    void operator()(Frechet_point *)  override;
    void operator()(Frechet_query *) override;
    ~Discrete_snapping() = default;


};


class Continuous_snapping: public snapping {
  private:
    double delta;
    double t;

  public:
    Continuous_snapping(double delta);
    void operator()(Frechet_point *)  override;
    void operator()(Frechet_query *) override;
    ~Continuous_snapping() = default;

    void minMaxFunction(std::vector<double>& timeseries);
};


class Frechet_HashTable: public HashTable {
  protected:
    gFunction hashingFunction;
    std::list<Data_point*>* buckets;                                            // Array of Lists Aka Hash Table;
    snapping* snapper;
    size_t itemDim;

    template <typename T>
    void padding(T* item, double num = 0){
      if (item->get_altered_coordinates_size() > this->itemDim){
          std::cout << "item->altered_coordinates.size() > this->itemDim" << std::endl;
          exit(0);
      }
      while (item->get_altered_coordinates_size() < this->itemDim)
        item->altered_coordinates_emlace_back(num);
    }

  public:
    Frechet_HashTable() = default;
    Frechet_HashTable(int itemDim, unsigned long long tableSize, int k, int w,snapping* snapper);     // Constructs H and G functions;
    virtual ~Frechet_HashTable();

    void init(size_t itemDim, unsigned long long tableSize, int k, int w, snapping *snapper);

    void insertDiscrete(Frechet_point* item);

    void insertContinuous(Frechet_point* item);

    void NearestNeighboursDiscrete(Frechet_query* query, LSH_Set* ordSet,int index);

    void NearestNeighboursContinuous(Frechet_query* query, LSH_Set* ordSet,int index);

};


class Frechet_solver: public Solver {                                              // Class is used for both Discrete and Continuous Frechet
  private:
    const int l;                                                                   // number of HashTables, in Continuous l = 1
    int w;
    int dim;
    std::string algorithm;

    std::vector<Frechet_point*> points_coordinates;                                // Here we store the elements read from the file
    std::vector<Frechet_query*> queries;
    Frechet_HashTable* hashTables;                                                 // Hash the tables using the G hash functions


    void writeResult(LSH_Set* result, Data_query* query, LSH_Set* true_nn);        // given an ordered set,writes items to output path

  public :

    // Constrctor for Discrete Frechet
    Frechet_solver(std::string dataset_path, std::string query_path, std::string output_filepath, std::string algorithm, int k, int l, int n, int r, double (*distanceFunction)(const std::vector<double> &a, const std::vector<double> &b) = EuclidianDistance);

    // Constrctor for Continuous Frechet
    Frechet_solver(std::string dataset_path, std::string query_path, std::string output_filepath, std::string algorithm, int k, int n, int r, double (*distanceFunction)(const std::vector<double> &a, const std::vector<double> &b) = EuclidianDistance);

    ~Frechet_solver();

    bool solveDiscreteFrechet();                                                // This function is called to solve NN , kNN and Approximate Range Search.

    bool solveContinuousFrechet();

    LSH_Set* NNandRSDiscrete(Frechet_query* query);                                // 1-NN , k-NN and Approximate Range Search, returns LSH_Set with nearest neighbors

    LSH_Set* NNandRSContinuous(Frechet_query* query);

    void printQueries() const;
};


using vector2d = std::vector<std::vector<double>>;


double calculateDistance(const Frechet_point *a, const Frechet_point *b, size_t i, size_t j);


double EuclidianDistance(const Frechet_point *a,const Frechet_point *b);


double discreteFrechet(const Frechet_point *a, const Frechet_point *b);


double discreteFrechet(const std::vector<double> &a, const std::vector<double> &b, double (*distFunction)(const std::vector<double> &a, const std::vector<double> &b) = EuclidianDistance);


double discreteFrechetRecursion(const Frechet_point *a, const Frechet_point *b, int i, int j,double** maxLength /* vector2d &maxLength */);


double discreteFrechetRecursion(const std::vector<double> &a, const std::vector<double> &b, int i, int j, double **maxLength, double (*distFunction)(const std::vector<double> &a, const std::vector<double> &b));


LSH_Set* bruteForceSearchDiscrete(Frechet_query *query, std::vector<Frechet_point*>& points_coordinates, int n);


LSH_Set* bruteForceSearchContinuous(Frechet_query *query, std::vector<Frechet_point*>& points_coordinates, int n, Curves& frechet_points_as_curves);


void filtering(std::vector<double>& timeseries);


template<typename T>
void constructFredFrechetCurve(T* item, Curve& curve){
  std::vector<double> coordinates = item->getCoordinates();

  for (int i = 0; i < coordinates.size(); i++){

    Point tmpPoint(1);                                                          // each point is of 1 dimension

    tmpPoint.set(0, coordinates[i]);
    curve.push_back(tmpPoint);
  }
}

#endif

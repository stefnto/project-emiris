#ifndef __LSH_CURVES_HPP__
#define __LSH_CURVES_HPP__

#include "LSH.hpp"
#include <sstream>
#include <fstream>
#include <math.h>
#include "utils.hpp"


class lshc_point{        
    using ts_point = std::pair<int,double>;        //(a,x_a) 

    private:
    std::vector<ts_point> ts;                     //Contains the curve before the snapping & concatenation of x's and y's
    std::vector<double> lsh_vector;               //Contains the snapped & padded coordinates
    Data_item*  dataItem;                         //Carries extra information such as ts
    int ID;                                       //Used for lsh hashing   
        
    public:
        const std::vector<ts_point>& get_ts()const{return this->ts;}            
        void lsh_push_back(double num){this->lsh_vector.push_back(num);}
        int get_lshVectorSize() const { return lsh_vector.size();}
        const std::vector<double>& getCoordinates() const {return this->lsh_vector;}
        void ID_push_back(int ID) {this->ID = ID;}
        lshc_point(Data_item* dataItem):dataItem(dataItem){}
};







class LSHc_HashTable : public HashTable{
protected:
    gFunction hashingFunction;
    std::list<lshc_point *> *buckets;               // Array of Lists Aka Hash Table;
    snapper* snapping;                              // Converts data_it
    int itemDim;                             
    

public:

    LSHc_HashTable(int itemDim, unsigned long long tableSize, int k, int w,double delta);       // Constructs H and G functions;
    LSHc_HashTable()=default;
    virtual ~LSHc_HashTable();

    void init(int itemDim, unsigned long long tableSize, int k, int w,double delta);            // Done
    int getItemDim() const {return itemDim;}                                                    // Done
    void insert(Data_item* item);                                                               // Done
    void NearestNeighbours(Data_item *query, LSH_Set *ordSet, int index);                       // todo 
};

void padding(lshc_point* point,double value,int maxSize);
double discreteFrechet(const lshc_point *a, const lshc_point *b);
double discreteFrechetRecursion(const lshc_point *a, const lshc_point *b,int i,int j,vector2d& maxLength);

class LSHc_solver: public Solver {
  private:
    const int l;                                                                // number of HashTables;
    int w;

    std::vector<Data_item*> points_coordinates;
    std::vector<Data_item*> queries;
    LSHc_HashTable* hashTables;                                                 // Hash the tables using the G hash functions


    void writeResult(LSH_Set* result, Data_item* query, std::set<double>& true_nn);                                //given an ordered set,writes items to output path

  public :

    //H functions are constructed inside the LSH_solver constructor and picked by the G functions.
    LSHc_solver(std::string dataset_path, std::string query_path, std::string output_filepath, int k, int l, int n, int r, double (*distanceFunction)(const std::vector<double>& a,const std::vector<double>& b) = EuclidianDistance);
    ~LSHc_solver();

    bool solve();                                                               // This function is called to solve NN , kNN and Approximate Range Search.

    LSH_Set* NNandRS(Data_item *query);                                        // 1-NN , k-NN and Approximate Range Search, returns LSH_Set with nearest neighbors
    void printQueries() const;
};

#endif
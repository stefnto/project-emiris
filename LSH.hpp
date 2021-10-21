#include <iostream>
#include <vector>
#include <cmath>


class LSH_solver{
    private:

        class LSH_item;
        class LSH_HashTable;

        LSH_HashTable* hashTables;                                          //Hash tables using the G hash functions
        int N;                                                              //number of Nearest Neighbours we're looking for 
        int R;                                                              //the search is made inside the R Radius


        

    public:

        //H functions are constructed inside the LSH_solver constructor and picked by the G functions with the expected way.
        LSH_solver(std::string dataset_path,int k = 4,int L=5,int N = 1,int R = 10000,double (*distanceFunction)(std::vector<double> a, std::vector<double> b) = EuclidianDistance);
        bool solve(std::string query_path, std::string output_path);




};

class LSH_item{
    private:
        std::string item_id;
        std::vector<double> coordinates;
        int ID;                                 
    public:

        LSH_item(std::string item_id,std::vector<double> coordinates);
        ~LSH_item() = default;
        LSH_item(LSH_item&) = default;
        void set_id(int ID);
    
};

class LSH_HashTable{

    private:
        int size;
    public:
        LSH_HashTable(int size);            // Size = Number of Buckets;    
};






double EuclidianDistance(std::vector<double> a, std::vector<double> b);
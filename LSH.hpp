#include <iostream>
#include <vector>
#include <cmath>
#include <list>
#include <random>

double EuclidianDistance(std::vector<int> a, std::vector<int> b);

class LSH_solver{
    private:

        LSH_HashTable* hashTables;                                          //Hash tables using the G hash functions
        int N;                                                              //number of Nearest Neighbours we're looking for 
        int R;                                                              //the search is made inside the R Radius

    public:

        //H functions are constructed inside the LSH_solver constructor and picked by the G functions with the expected way.
        LSH_solver(std::string dataset_path,int k = 4,int L=5,int N = 1,int R = 10000,double (*distanceFunction)(std::vector<int> a, std::vector<int> b) = EuclidianDistance);
        bool solve(std::string query_path, std::string output_path);        //This function is called to solve NN , kNN and Approximate Range Search.




};

class LSH_item{
    private:
        std::string item_id;
        std::vector<int> coordinates;
        int ID;
    public:
        LSH_item(){std::cout << " Hello " << std::endl;}
        LSH_item(std::string item_id,std::vector<int> coordinates);
        ~LSH_item() = default;
        LSH_item(LSH_item&) = default;
        void set_id(int ID);
    
};

class LSH_HashTable{
private:
    int size;
    int k;                                                                                            //Number of H functions used by hashingFunction


    int (*hashingFunction)(LSH_item);
    std::list<LSH_item &>* buckets;                                                                   //Array of Lists Aka Hash Table;
    
public:
    LSH_HashTable(int size,int k);                                                                    //Constructs H and G functions;
    ~LSH_HashTable();
    LSH_HashTable(LSH_HashTable&) = default;
    void insert(LSH_item); 
};

class gFunction{                                                                                     //Σ r_i * h_i          
    private:
    class hFunction;                                                                            

    std::vector<std::pair<int,hFunction>> linearCombinationElements;                                    

    public:
        gFunction(int itemSize);                                                                        //gFunction is a functor
        int operator()(LSH_item&);                                                                      //Hashing function 

};

class hFunction{                            // floor( (p*v + t)/ w )

    private:
        std::vector<float> v;               //contains vector V
        float t;
        const int w;

    public:

        hFunction(int itemSize);
        int operator()(LSH_item&);
};

class LSH_Exception{};






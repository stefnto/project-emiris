#include "utils.hpp"

// a % n = a – ( n * trunc( a/n ) ).

 long mod(long x, long y){
    if (x < 0) {
        long modulo = (-x) % y;
        std::cout << "modulo = " << modulo << std::endl;
        return modulo == 0 ? 0 : y - modulo ;
    }
    return x % y;
}

 double EuclidianDistance(std::vector<int> a, std::vector<int> b){
    if (a.size() != b.size()) return 0;

    double sum = 0;

    for (int i = 0; i < a.size(); i++)
    {
        sum += pow((a.at(i) - b.at(i)), 2);
    }

    return sqrt(sum);
}

 int rGenerator(){


      return rand() % 2000 - 999;

}

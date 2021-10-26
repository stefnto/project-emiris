#include "utils.hpp"

int mod(int x, int y){
    if (x < 0)
        return y - ((-x) % y);
    return x % y;
}

double EuclidianDistance(std::vector<double> a, std::vector<double> b){
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
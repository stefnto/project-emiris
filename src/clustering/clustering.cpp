#include <clustering.hpp>

clustering::clustering(std::string input_file, std::string config_file, std::string output_file, char method,double (*distanceFunction)(std::vector<int> a, std::vector<int> b)){
    readItems("input/input_small_id.txt",input_data);
    this->distanceFunction = distanceFunction;
}

void clustering::solve(){
    this->initpp();
}

std::list<centeroid>* clustering::initpp(){
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator1(seed1);
    std::uniform_int_distribution<int> distribution(0,input_data.size());
    std::cout << "input_data.size()" << input_data.size() << std::endl;

    int t = 1;
    int pos = distribution(generator1);                                  //choosing starting centeroid
    
    std::cout << "position of first centeroid chosen : " << pos << std::endl;

    std::set<std::string> centeroid_ids;                                //set containing ids of already chosens points

    std::list<centeroid>* centeroids = new std::list<centeroid>();
    
    centeroid_ids.emplace(input_data.at(pos)->getName());               //placing first elements id to the set of the centeroid ids

    centeroids->emplace_back(input_data.at(pos)->getCoordinates());      //placing first centeroid to the set of the centeroids
    while (t != k){
    
        float prev_sum = 0;                 

        using sumAndKey = std::pair<float, clustering_data_item *>;
        using sumAndKeySet = std::set<sumAndKey, bool (*)(const sumAndKey &a, const sumAndKey &b)>;

        auto comp = [](const sumAndKey &a, const sumAndKey &b) -> bool{ return a.first < b.first; };

        sumAndKeySet partial_sums(comp);                                                            //contains elements and partial sums assigned to each element

        for (clustering_data_item* item : input_data){

            if ( centeroid_ids.find(item->getName()) == centeroid_ids.end() ){                      //checking if current point has already been chosen as centeroid

                std::list<centeroid>::iterator it = centeroids->begin();
                const std::vector<int> coordinates = item->getCoordinates();
                float minD = distanceFunction(*it,coordinates);
                while ( ++it != centeroids->end() ) {
                    float dist = distanceFunction(*it,coordinates);
                    if (dist < minD) minD = dist;
                }
                prev_sum += minD*minD;
                partial_sums.emplace(prev_sum,item);
            }
        }
        std::cout << "prev_sum is " << prev_sum << std::endl;
        unsigned seed2 = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator2(seed2);
        std::uniform_real_distribution<float> pickFloat(0,prev_sum);

        float x = pickFloat(generator2);

        std::cout << "float picked : " << x << std::endl; 

        sumAndKeySet::iterator current = partial_sums.begin();                            //checking where the picked float belongs p(r-1) < x <= p(r)

        while (current != partial_sums.end()){
            if ( (*current).first >= x ){                                                 //new centeroid found
                centeroid_ids.emplace(((*current).second)->getName());                    //adding new centeroid's id to the set of the centeroid_ids
                centeroids->emplace_back(((*current).second)->getCoordinates());          //adding new centeroid to the set of centeroids
                break;
            }
            current++;
        }
        t++;
    }
    for (std::string name : centeroid_ids) std::cout << name << std::endl;
    return centeroids;

}
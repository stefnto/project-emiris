#include <clustering.hpp>

clustering::clustering(std::string input_file, std::string config_file, std::string output_file, char method,double (*distanceFunction)(std::vector<int> a, std::vector<int> b)){
    readItems("input/input_small_id.txt",input_data);
    this->distanceFunction = distanceFunction;
}

void clustering::solve(){
    this->lloyd();
}

centeroid* clustering::initpp(){
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator1(seed1);
    std::uniform_int_distribution<int> distribution(0,input_data.size());
    std::cout << "input_data.size()" << input_data.size() << std::endl;

    int t = 1;
    int pos = distribution(generator1);                                  //choosing starting centeroid
    
    std::cout << "position of first centeroid chosen : " << pos << std::endl;

    std::set<std::string> centeroid_ids;                                //set containing ids of already chosens points

    centeroid* centeroids = new centeroid[k];
    
    centeroid_ids.emplace(input_data.at(pos)->getName());                       //placing first elements id to the set of the centeroid ids

    centeroids[0]= input_data.at(pos)->getCoordinates();                        //placing first centeroid to the set of the centeroids
    while (t != k){
    
        float prev_sum = 0;                 

        using sumAndKey = std::pair<float, clustering_data_item *>;
        using sumAndKeySet = std::set<sumAndKey, bool (*)(const sumAndKey &a, const sumAndKey &b)>;

        auto comp = [](const sumAndKey &a, const sumAndKey &b) -> bool{ return a.first < b.first; };

        sumAndKeySet partial_sums(comp);                                                            //contains elements and partial sums assigned to each element

        for (clustering_data_item* item : input_data){

            if ( centeroid_ids.find(item->getName()) == centeroid_ids.end() ){                      //checking if current point has already been chosen as centeroid

                int it = 0;
                const std::vector<int> coordinates = item->getCoordinates();
                float minD = distanceFunction(centeroids[0],coordinates);
                while ( ++it < t ) {
                    float dist = distanceFunction(centeroids[it],coordinates);
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
                centeroids[t]= ((*current).second)->getCoordinates();                     //adding new centeroid to the set of centeroids
                break;
            }
            current++;
        }
        t++;
    }
    for (std::string name : centeroid_ids) std::cout << name << std::endl;
    return centeroids;

}

void clustering::lloyd(){
    float sum[k];
    int elems[k];
    for (int i = 0; i < k; i++){ sum[i] = 0;elems[i] = 0;}
     
    centeroid* centeroids = this->initpp();
    int iterations = 0 ;
    int limit = 10;
    int vector_size = centeroids[0].size();

    while (iterations < limit){
        centeroid* nextCenteroids = new centeroid[k];                           //next iteration will use
        int changes = 0;
        for (int i = 0; i < k; i++) nextCenteroids[i].assign(vector_size,0);
    
        for (clustering_data_item* item : input_data){
            float minD1 = distanceFunction(item->getCoordinates(),centeroids[0]);
            float minD2 = distanceFunction(item->getCoordinates(),centeroids[1]);
            int c1 = 0,c2 = 1;
            if (minD1 > minD2){
                std::swap(minD1,minD2);
                std::swap(c1, c2);
            }
            int c = 0;
            for (int i = 1 ; i < k ; i++){
                float dist = distanceFunction(item->getCoordinates(), centeroids[i]);
                if (dist < minD1){                                                //distance from nearest
                    minD1 = dist;
                    c1 = i;                                                       //centeroid with least distance from current element 
                }else if (dist < minD2){                                          //distance from second nearest
                    minD2 = dist;
                    c2 = i;
                }
            }
            if (item->getCluster() != c1) changes++;
            item->setCluster(c1);
            item->setDistance1st(minD1);
            item->setDistance2nd(minD2);
            elems[c1]++;
            for (int j = 0 ; j < vector_size; j++) {
                nextCenteroids[c1][j] += item->getCoordinates()[j];   
            }
        }
        if (++iterations == limit) break;
        for (int i = 0; i < k; i++){                                             //creating next gen of centeroids
            for (int j = 0 ; j < vector_size; j++) {nextCenteroids[i][j] /= elems[i];}
            sum[i] = 0;
            elems[i] = 0;
        }
        delete[] centeroids;
        centeroids = nextCenteroids;
    }
    std::ofstream output_file;
    output_file.open("clustering.txt", std::ofstream::out | std::ofstream::app);
    for (clustering_data_item* item : input_data) output_file << item->getName() << " belongs to cluster : " << item->getCluster() << std::endl;
    output_file.close();
}
#include <clustering.hpp>

clustering::clustering(std::string input_file, std::string config_file, std::string output_file, char method,double (*distanceFunction)(const std::vector<int>& a,const std::vector<int>& b)){
    readItems("input/input_small_id.txt",input_data);
    this->distanceFunction = distanceFunction;
}

void clustering::solve(){
    this->reverseAssignment();
}

centroid* clustering::initpp(){
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator1(seed1);
    std::uniform_int_distribution<int> distribution(0,input_data.size());
    std::cout << "input_data.size()" << input_data.size() << std::endl;

    int t = 1;
    int pos = distribution(generator1);                                  //choosing starting centroid
    
    std::cout << "position of first centroid chosen : " << pos << std::endl;

    std::set<std::string> centroid_ids;                                //set containing ids of already chosens points

    centroid* centroids = new centroid[k];
    
    centroid_ids.emplace(input_data.at(pos)->getName());                       //placing first elements id to the set of the centroid ids

    centroids[0]= input_data.at(pos)->getCoordinates();                        //placing first centroid to the set of the centroids
    while (t != k){
    
        float prev_sum = 0;                 

        using sumAndKey = std::pair<float, clustering_data_item *>;
        using sumAndKeySet = std::set<sumAndKey, bool (*)(const sumAndKey &a, const sumAndKey &b)>;

        auto comp = [](const sumAndKey &a, const sumAndKey &b) -> bool{ return a.first < b.first; };

        sumAndKeySet partial_sums(comp);                                                            //contains elements and partial sums assigned to each element

        for (clustering_data_item* item : input_data){

            if ( centroid_ids.find(item->getName()) == centroid_ids.end() ){                      //checking if current point has already been chosen as centroid

                int it = 0;
                const std::vector<int> coordinates = item->getCoordinates();
                float minD = distanceFunction(centroids[0],coordinates);
                while ( ++it < t ) {
                    float dist = distanceFunction(centroids[it],coordinates);
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
            if ( (*current).first >= x ){                                                 //new centroid found
                centroid_ids.emplace(((*current).second)->getName());                    //adding new centroid's id to the set of the centroid_ids
                centroids[t]= ((*current).second)->getCoordinates();                     //adding new centroid to the set of centroids
                break;
            }
            current++;
        }
        t++;
    }
    for (std::string name : centroid_ids) std::cout << name << std::endl;
    return centroids;

}

void clustering::lloyd(){
    float sum[k];
    int elems[k];
    for (int i = 0; i < k; i++){ sum[i] = 0;elems[i] = 0;}
     
    centroid* centroids = this->initpp();
    int iterations = 0 ;
    int limit = 20;
    int vector_size = centroids[0].size();

    while (iterations < limit){
        centroid* nextCentroids = new centroid[k];                           //next iteration will use different centroids
        int changes = 0;
        for (int i = 0; i < k; i++) nextCentroids[i].assign(vector_size,0);
    
        for (clustering_data_item* item : input_data){
            float minD1 = distanceFunction(item->getCoordinates(),centroids[0]);
            float minD2 = distanceFunction(item->getCoordinates(),centroids[1]);
            int c1 = 0,c2 = 1;
            if (minD1 > minD2){
                std::swap(minD1,minD2);
                std::swap(c1, c2);
            }
            for (int i = 2 ; i < k ; i++){
                float dist = distanceFunction(item->getCoordinates(), centroids[i]);
                if (dist < minD1){                                                //distance from nearest
                    minD1 = dist;
                    c1 = i;                                                       //centroid with least distance from current element 
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
                nextCentroids[c1][j] += item->getCoordinates()[j];   
            }
        }
        std::cout <<"changes : " << changes << std::endl;
        if (++iterations == limit) break;
        for (int i = 0; i < k; i++){                                             //creating next gen of centroids
            for (int j = 0 ; j < vector_size; j++) {nextCentroids[i][j] /= elems[i];}
            sum[i] = 0;
            elems[i] = 0;
        }
        delete[] centroids;
        centroids = nextCentroids;
    }
    std::ofstream output_file;
    output_file.open("clustering.txt", std::ofstream::out | std::ofstream::app);
    for (clustering_data_item* item : input_data) output_file << item->getName() << " belongs to cluster : " << item->getCluster() << std::endl;
    output_file.close();
}

float clustering::silhouette(centroid* centroids){
    std::list<clustering_data_item*> clusters[k];
    float total = 0;
    for (clustering_data_item* item : input_data) clusters[item->getCluster()].push_back(item);     // firstly we put each element to its cluster

    for (int i = 0 ; i <k; i++){                                                                    //we're doing this for all elements but we're iterating through clusters
        std::cout << i << " i is " << std::endl;
        float distances[clusters[i].size()][clusters[i].size()];                                    //we're storing the distances we've already found in order not to double calc
        int counter1 = 0 ;
        std::list<clustering_data_item *>::iterator it1 = clusters[i].begin();
        while ( it1 != clusters[i].end() ){
            int counter2 = 0;
            std::list<clustering_data_item *>::iterator it2 = clusters[i].begin();
            float ai = 0;                                                                          //sum of distances from same cluster
            while (it2 != it1){                                                                    //adding to the sum already calculated dists
                ai+=distances[counter2][counter1];
                it2++;
            }

            it2++;
            counter2++;
            
            while ( it2 != clusters[i].end() ){
                float dist = (*it1)->calculateDistance(*it2);
                distances[counter1][counter2] = dist;
                ai+=dist;
                it2++;
            }

            ai /= clusters[i].size();      //average distance from his neighborhood
            float minD;
            int sec;
            //finding second nearest centroid
            if (i == 0 ) {
                minD = (*it1)->calculateDistance(centroids[1]);
                sec=1;
            }else{
                minD = (*it1)->calculateDistance(centroids[i-1]);
                sec = i-1;
            }
            for (int j = 0 ; j < k ; j++){
                if (j != i) {
                    float dist = (*it1)->calculateDistance(centroids[j]);
                    if (dist < minD) {
                        minD = dist;
                        sec = j;
                    }
                }
            }

            float bi = 0;
            for (clustering_data_item* item : clusters[sec]) bi += (*it1)->calculateDistance(item);
            
            bi /= clusters[sec].size();
            float max = bi;
            if (ai > bi) max = ai;
            float sil = (bi-ai)/max;
            (*it1)->setSilhouette(sil);
            total +=sil;
            it1++;
        }
    }
    total /= input_data.size();
    return total;
}

void clustering::reverseAssignment(){
    LSH_solver solver(&input_data);
    int population[k];
    centroid *centroids = this->initpp();
    for (int i = 0; i < k; i++)population[i] = 0;
    int vector_size = centroids->size();
    int iterations = 0;
    std::list<clustering_data_item*> clusters[k];
    centroid *nextCentroids = new centroid[k];
    while (true){
        float radius = minCentroidDistance(centroids)/2;
        while (true){
            int sum = 0;
            for (int i = 0 ; i < k; i++) sum += solver.clusteringRangeSearch(radius,centroids[i],i);
            if (sum < input_data.size() / 100) break;
        }
        int changes = 0;
            for (int i = 0; i < k; i++)nextCentroids[i].assign(vector_size, 0);
        for (clustering_data_item* item : input_data){
            if (item->isSetRadius() == false) item->findNearestCentroid(centroids, k);
            else item->unsetRadius();
            int index = item->getCluster();
            population[index]++;
            for (int j = 0; j < vector_size; j++) {
                const std::vector<int>& coors = item->getCoordinates();
                nextCentroids[index][j] += coors[j];
            }
        } 
        for (int i = 0 ; i < k ; i++) std::cout <<"population[" << i << "] = " << population[i] << std::endl;
           

        if (++iterations == 10 ) break;
        for (int i = 0 ; i < k ; i++){
            for (int j = 0 ; j < vector_size; j++) nextCentroids[i][j] /= population[i];
            population[i] = 0;
        }
        delete[] centroids;
        centroids = nextCentroids;
        nextCentroids = new centroid[k];
    }
    delete[] nextCentroids;
    float totalSil = silhouette(centroids);
    std::ofstream output_file;
    output_file.open("reverseAssignment.txt", std::ofstream::out | std::ofstream::app);
    for (clustering_data_item *item : input_data)output_file << item->getName() << " belongs to cluster : " << item->getCluster() << std::endl;
    for (clustering_data_item* item : input_data) output_file << "silhouette " << item->getName() << " " << item->getSilhouette() << std::endl;
    output_file << "total silhouette " << totalSil << std::endl;
    output_file.close();
}

float clustering::minCentroidDistance(centroid* centroids){
    float minDist = this->distanceFunction(centroids[0],centroids[1]);
    for (int i = 0 ; i < k; i++){
        for (int j = i+1 ; j < k; j++){
            float dist = distanceFunction(centroids[i], centroids[j]);
            if (dist < minDist) minDist = dist;
        }
    }
    return minDist;
};

#include <clustering.hpp>

clustering::clustering(std::string input_file, std::string config_file, std::string output_file, char method,double (*distanceFunction)(const std::vector<int>& a,const std::vector<int>& b)){
    readItems("input/input_small_id.txt",input_data);
    this->distanceFunction = distanceFunction;
}

void clustering::solve(){
    Data_item::setDistanceFunction(EuclidianDistance);
    this->lloyd();
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
    int changes = 0;
    while (true){
        centroid* nextCentroids = new centroid[k];                           //next iteration will use different centroids
        for (int i = 0; i < k; i++) nextCentroids[i].assign(vector_size,0);
    
        for (clustering_data_item* item : input_data){
            float minD = distanceFunction(item->getCoordinates(),centroids[0]);
            int c = 0;
            for (int i = 1 ; i < k ; i++){
                float dist = distanceFunction(item->getCoordinates(), centroids[i]);
                if (dist < minD){                                                //distance from nearest
                    minD = dist;
                    c = i;                                                       //centroid with least distance from current element   
                }
            }
            if (item->getCluster() != c) changes++;
            item->setCluster(c);
            item->setDistanceFromQuery(minD);
            elems[c]++;
            // create generation of new centroids
            for (int j = 0 ; j < vector_size; j++)  nextCentroids[c][j] += item->getCoordinates()[j];    
        }
        std::cout <<"changes " << changes << std::endl;
        if (changes < input_data.size()/500) break;
        changes = 0;
        for (int i = 0; i < k; i++){                                             //creating next gen of centroids
            for (int j = 0 ; j < vector_size; j++) {nextCentroids[i][j] /= elems[i];}
            sum[i] = 0;
            elems[i] = 0;
        }
        delete[] centroids;
        centroids = nextCentroids;
    }
    float total = silhouette(centroids);
    delete[] centroids;
    std::ofstream output_file;
    output_file.open("clustering.txt", std::ofstream::out | std::ofstream::app);
    for (clustering_data_item* item : input_data) output_file << item->getName() << " belongs to cluster : " << item->getCluster()<< " silhouette : " <<item->getSilhouette() << std::endl;
    output_file << "total silhouette is : " << total << std::endl;
    output_file.close();
}

float clustering::silhouette(centroid* centroids){
    std::list<clustering_data_item*> clusters[k];
    float total = 0;
    for (clustering_data_item* item : input_data) clusters[item->getCluster()].push_back(item);     // firstly we put each element to its cluster

    for (int i = 0 ; i <k; i++){                                                                    //we're doing this for all elements but we're iterating through clusters
        int counter1 = 0 ;
        std::list<clustering_data_item *>::iterator it1 = clusters[i].begin();
        while ( it1 != clusters[i].end() ){
            std::list<clustering_data_item *>::iterator it2 = clusters[i].begin();
            float ai = 0;                                                                          //sum of distances from same cluster
            while ( it2 != clusters[i].end() ){
                float dist = (*it1)->calculateDistance(*it2);
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
            counter1++;
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
        int changes = 0;
        while (true){
            for (int i = 0 ; i < k; i++) {
                Data_item centr_to_di(std::to_string(i),centroids[i]);
                changes += solver.clusteringRangeSearch(radius,&centr_to_di,i);
            }
            if (changes < input_data.size() / 1000) break;
            changes = 0;
            radius *= 2;
        }
        for (int i = 0; i < k; i++)nextCentroids[i].assign(vector_size, 0);
        for (clustering_data_item* item : input_data){
            if (item->getRadius() == 0) item->findNearestCentroid(centroids, k);
            else item->setRadius(0);
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
    float totalSil = silhouette(centroids);
    delete[] nextCentroids;
    delete[] centroids;
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

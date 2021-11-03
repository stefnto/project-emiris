#include "Cube.hpp"

// Vertex_point methods

Vertex_point::Vertex_point(int k, int itemDim): binary_hash(0) {
  for (int i = 0; i < k; i++)
    hFunc.emplace_back(hFunction(itemDim));

    // std::cout << "Done!" << std::endl;
}

void Vertex_point::init(int k, int itemDim){
  this->binary_hash = 0;
  for (int i = 0; i < k; i++)
    hFunc.emplace_back(hFunction(itemDim));
}

void Vertex_point::bit_concat(int value){
  if (value == 1){
    this->binary_hash = this->binary_hash << 1;
    this->binary_hash = this->binary_hash | 1;
  }
  else
    this->binary_hash = this->binary_hash << 1;
}

unsigned long long Vertex_point::operator()(Data_item* item, int k) {
  srand(time(NULL));

  // char buf[k+1];
  // int index = k;
  // buf[index] = '\0';

  int count = 0;
  for (hFunction h_i : hFunc){
    int tmp = h_i(item);                                                        // h_i(p)
    // std::cout << "tmp = " << tmp << std::endl;
    auto search = set.find(tmp);
    if (search == set.end()){                                                   // an h_i(p) with value 'tmp' doesn't exist
      int bit = mod(rand()*tmp, 2);                                             // randomly generate 0 or 1
      // std::cout << "1-------bit = " << bit << std::endl;
      set.insert( {tmp, bit} );                                                 // and then map it to the h_i(p) value
      bit_concat(bit);
      // std::cout << "hash =  " << this->binary_hash << std::endl;

    }
    else {
      // std::cout << "2-------bit = " << search->second << std::endl;
      bit_concat(search->second);
      // std::cout << "hash =  " << this->binary_hash << std::endl;
    }

  }

  return binary_hash;
}

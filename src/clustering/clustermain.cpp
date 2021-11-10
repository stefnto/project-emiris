#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include "clustering.hpp"


using namespace std;

int main(int argc, char *argv[]){



  double sttime, endtime;                                                       // to compute total run time
  extern char *optarg;
  extern int optind;
  int opt;
  int number;
  ifstream datafile;
  int counter = 0;                                                              // counter used to loop through lines of input_file

  string input_file,output_file, method, config_file;
  int iflag = 1, oflag = 1, cflag = 1, mflag = 1;
  int k_lsh = 4, l_lsh = 3, n = 1, r = 10000;                                           // default values if not changed
  int k_medians = 3;
  int m_cube = 10, k_cube = 3, probes_cube = 2;

  string num_clust = "number_of_clusters", num_vector_ht = "number_of_vector_hash_tables",
         num_vector_hf = "number_of_vector_hash_functions", m_hcube = "max_number_M_hypercube",
         num_hcube_dim = "number_of_hypercube_dimensions", number_of_probes = "number_of_probes";


  while ((opt = getopt(argc, argv, "i:c:o:m:")) != -1)
        switch (opt) {
          case 'i':
                  input_file = optarg;
                  iflag--;
                  break;
          case 'c':
                  config_file = optarg;
                  cflag--;
                  break;
          case 'o':
                  output_file = optarg;
                  oflag--;
                  break;
          case 'm':
                  method = optarg;
                  mflag--;
                  break;
        }

  // options -i, -q, -o are mandatory
  if (iflag != 0 || oflag != 0){
    cout << "Error: Missing -i or -o option\n";
    std::cout << "Terminating..." << '\n';
    exit(1);
  }

  if (mflag == 0){
      //check m
  }

  if (cflag == 0){
    //check complete
  }


  std::ifstream config;

  config.open(config_file);
  if (config.is_open()){
    std::string line;
    while (getline(config, line)){
      if (num_clust.compare(0, 18, line) == 0)
    }
  }


  cout << "input_file = " << input_file << "\n";
  cout << "output_file = " << output_file << "\n";
  cout << "N = " << n << "\n";
  cout << "R = " << r << "\n";
  cout << "k_lsh = " << k_lsh << "\n";
  cout << "L_lsh = " << l_lsh << "\n";
  cout << "k_cube = " << k_cube << endl;
  cout << "m_cube = " << m_cube << endl;
  cout << "probes_cube = " << probes_cube << endl;




  // clustering a(input_file, "config", output_file, 2);
  // a.solve();
}

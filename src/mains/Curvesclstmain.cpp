#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include "../../hdr/curves_clustering.hpp"


//run1 ./cclustering -i nasd_input.csv -o cclusteringClassic.txt --update Mean_Frechet --assignment Classic - working with --complete too (~84sec)
//run2 ./cclustering -i nasd_input.csv -o cclusteringClassic.txt --update Mean_Vector --assignment Classic - not working
//run3 ./cclustering -i nasd_input.csv -o cclusteringLSH.txt --update Mean_Vector --assignment LSH


using namespace std;

int main(int argc, char *argv[]){

  double sttime, endtime;                                                       // to compute total run time
  extern char *optarg;
  extern int optind;
  int opt;
  int number;
  ifstream datafile;
  int counter = 0;                                                              // counter used to loop through lines of input_file

  string input_file, output_file, config_file, update_str, assignment_str;
  int iflag = 1, oflag = 1, cflag = 1, uflag = 1, aflag = 1, complete_flag = 0, silhouette_flag = 0;
  int k_lsh = 4, l_lsh = 3, n = 1, r = 10000;                                   // default values if not changed
  int k_medians = 3;
  int m_cube = 10, k_cube = 3, probes_cube = 2;
  updateMethods um;
  assignmentMethods am;

  static struct option long_options[] = {
      {"complete", 0, NULL, 'p'},
      {"assignment", required_argument, NULL, 'a'},
      {"update", required_argument, NULL, 'u'},
      {"silhouette", required_argument, NULL, 's'},
      {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "i:c:o:u:a:p:s:", long_options, NULL)) != -1)
        switch (opt){
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
          case 'u':
                  update_str = optarg;
                  uflag--;
                  break;
          case 'a':
                  assignment_str = optarg;
                  aflag--;
                  break;
          case 'p':
                  complete_flag++;
                  break;
          case 's':
                  silhouette_flag++;
                  break;
        }

  // options -i, -q, -o are mandatory
  if (iflag != 0 || oflag != 0 || uflag != 0 || aflag != 0)
  {
    cout << "Error: Missing -i or -o or --update --assignment option" << endl;
    cout << "Terminating..." << endl;
    exit(1);
  }

  // check --assignment
  if (aflag == 0){
    if (assignment_str.compare("Classic") == 0)
      am = lloydCurve;
    else if (assignment_str.compare("LSH") == 0)
      am = lshV;
    else if (assignment_str.compare("Hypercube") == 0)
      am = hypercubeV;
    else if (assignment_str.compare("LSH_Frechet") == 0)
      am = lsh_frechet;
    else {
      cout << "Error: Undefined --assignment option" << endl;
      cout << "Terminating..." << endl;
      exit(1);
    }
  }

  // check --update
  if (uflag == 0){
    if (update_str.compare("Mean_Frechet") == 0)
      um = meanFrechet;
    else if (update_str.compare("Mean_Vector") == 0)
      um = meanVector;
    else {
      cout << "Error: Undefined --update option" << endl;
      cout << "Terminating..." << endl;
      exit(1);
    }
  }

  if (um == meanVector &&  am == lsh_frechet ){
    std::cout << "Cannot use assignment method LSH Frechet with update method Mean Vector" << std::endl;
    std::cout << "Terminating..." << std::endl;
    exit(1);
  }

  if (um == meanFrechet && ( am == lshV || am == hypercubeV ) ){
    if (am == lshV)
      std::cout << "Cannot use assignment method LSH Vector with update method Mean Frechet" << std::endl;

    if (am == hypercubeV)
      std::cout << "Cannot use assignment method Hypercube with update method Mean Frechet" << std::endl;
    std::cout << "Terminating..." << std::endl;
    exit(1);
  }


  readConfig(config_file, k_lsh, l_lsh, k_medians, m_cube, k_cube, probes_cube);

  cout << "input_file = " << input_file << "\n";
  cout << "output_file = " << output_file << "\n";
  cout << "N = " << n << "\n";
  cout << "R = " << r << "\n";
  cout << "k_lsh = " << k_lsh << "\n";
  cout << "L_lsh = " << l_lsh << "\n";
  cout << "k_cube = " << k_cube << endl;
  cout << "m_cube = " << m_cube << endl;
  cout << "probes_cube = " << probes_cube << endl;
  cout << "k_medians = " << k_medians << endl;
  cout << "assignment_str = " << assignment_str << endl;
  cout << "update_str = " << update_str << endl;


  if (am == lshV || am == hypercubeV){
    method m;
    if (am == lshV)
      m = lsh;
    else if (am == hypercubeV)
      m = hypercube;
    sttime = ((double)clock()) / CLOCKS_PER_SEC;

    Clustering_Solver a(input_file, output_file, k_lsh, l_lsh, n, r, k_medians, m_cube, k_cube, probes_cube, complete_flag);

    a.solve(m);

    endtime = ((double)clock()) / CLOCKS_PER_SEC;

  }
  else if (am == lloydCurve || am == lsh_frechet){
    sttime = ((double)clock()) / CLOCKS_PER_SEC;

    Clustering_curve_solver a(input_file, output_file, k_lsh, l_lsh, n, r, k_medians, m_cube, k_cube, probes_cube, complete_flag, um, am);


    a.solve();

    endtime = ((double)clock()) / CLOCKS_PER_SEC;
  }

  std::cout << "Clustering run in " << endtime - sttime << " seconds" << endl;
}

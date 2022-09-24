#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <vector>
#include <time.h>
#include "Curves.hpp"
#include "Cube.hpp"

using namespace std;

int main(int argc, char *argv[]){


  double sttime, endtime;                                                       // to compute total run time
  extern char *optarg;
  extern int optind;
  int opt;
  int number;
  ifstream datafile;
  int counter = 0;                                                              // counter used to loop through lines of input_file

  string input_file, query_file, output_file, algo_str, metric_str;
  int iflag = 1, qflag = 1, oflag = 1, aflag = 1, mflag = 1;
  int k = 4, l = 5, m = 10, n = 1, r = 10000, probes = 2;                                           // default values if not changed

  static struct option long_options[] = {
    {"algorithm", required_argument, NULL, 'a'},
    {"metric", required_argument, NULL, 'm'},
    {"probes", required_argument, NULL, 'r'},
    {0, 0, 0, 0}
  };


  while ((opt = getopt_long(argc, argv, "i:q:k:L:o:N:R:a:m:r:M:", long_options, NULL)) != -1)
        switch (opt) {
          case 'i':
                  input_file = optarg;
                  iflag--;
                  break;
          case 'q':
                  query_file = optarg;
                  qflag--;
                  break;
          case 'k':
                  k = atoi(optarg);
                  break;
          case 'L':
                  l = atoi(optarg);
                  break;
          case 'o':
                  output_file = optarg;
                  oflag--;
                  break;
          case 'N':
                  n = atoi(optarg);
                  break;
          case 'R':
                  r = atoi(optarg);
                  break;
          case 'a':
                  algo_str = optarg;
                  aflag--;
                  break;
          case 'm':
                  metric_str = optarg;
                  mflag--;
                  break;
          case 'r':
                  probes = atoi(optarg);
                  break;
          case 'M':
                  m = atoi(optarg);
                  break;
        }

  // options -i, -q, -o, --algorithm are mandatory
  if (iflag != 0 || qflag != 0 || oflag != 0 || aflag != 0){
    cout << "Error: Missing -i or -q or -o or --algorithm option\n";
    std::cout << "Terminating..." << '\n';
    exit(1);
  }

  cout << "input_file = " << input_file << "\n";
  cout << "query_file = " << query_file << "\n";
  cout << "output_file = " << output_file << "\n";
  cout << "k = " << k << "\n";
  cout << "L = " << l << "\n";
  cout << "N = " << n << "\n";
  cout << "R = " << r << "\n";
  cout << "probes = " << probes << "\n\n";

  sttime=((double) clock())/CLOCKS_PER_SEC;

  if (aflag == 0){
    if (algo_str.compare("LSH") == 0){

      // call LSH for time series as a vector
      std::cout << "LSH for timeseries as vector" << std::endl;

      LSH_solver solver1(input_file, query_file, output_file, k, l, n, r);

      endtime = ((double) clock())/CLOCKS_PER_SEC;

      cout << "Data initialized in " << endtime-sttime << " seconds" << endl;

      solver1.solve();

      endtime = ((double) clock())/CLOCKS_PER_SEC;

      cout << "LSH run in " << endtime - sttime << " seconds" << endl;

    }
    else if (algo_str.compare("Hypercube") == 0){

      // call Hypercube for time series as a vector
      std::cout << "Hypercube for timeseries as vector" << std::endl;

      Cube_Solver solver1(input_file, query_file, output_file, k, m, probes, n, r);

      endtime = ((double) clock())/CLOCKS_PER_SEC;

      cout << "Data initialized in " << endtime-sttime << " seconds" << endl;

      solver1.solve();

      endtime = ((double) clock())/CLOCKS_PER_SEC;

      cout << "HyperCube run in " << endtime - sttime << " seconds" << endl;

    }
    else if (algo_str.compare("Frechet") == 0){
      if (mflag == 0){

        if (metric_str.compare("discrete") == 0){

          // call discrete Frechet algo
          std::cout << "Discrete Frechet" << std::endl;

          Frechet_solver solver1(input_file, query_file, output_file, "LSH_Frechet_Discrete", k, l, n, r);

          endtime = ((double) clock())/CLOCKS_PER_SEC;

          cout << "Data initialized in " << endtime-sttime << " seconds" << endl;

          solver1.solveDiscreteFrechet();

          endtime = ((double) clock())/CLOCKS_PER_SEC;

          cout << "Discrete Frechet run in " << endtime - sttime << " seconds" << endl;
        }
        else if (metric_str.compare("continuous") == 0){

          // call continuous Frechet algo
          std::cout << "Continuous Frechet" << std::endl;

          Frechet_solver solver1(input_file, query_file, output_file, "LSH_Frechet_Continuous", k, n, r);

          endtime = ((double) clock())/CLOCKS_PER_SEC;

          cout << "Data initialized in " << endtime-sttime << " seconds" << endl;

          solver1.solveContinuousFrechet();

          endtime=((double) clock())/CLOCKS_PER_SEC;

          cout << "Continuous Frechet run in " << endtime - sttime << " seconds" << endl;
        }
      } else {
        std::cout << "Error: Missing --metric option for Frechet algorithm" << std::endl;
        std::cout << "Terminating..." << std::endl;
      }
    }
    else {
      std::cout << "Error: Undefined --algorithm option" << std::endl;
      std::cout << "Terminating..." << std::endl;
      exit(1);
    }
  }




}

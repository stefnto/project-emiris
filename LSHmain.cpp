#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <vector>
#include <time.h>
#include "LSH.hpp"

using namespace std;

int main(int argc, char *argv[]){


  double sttime, endtime;                                                       // to compute total run time
  extern char *optarg;
  extern int optind;
  int opt;
  int number;
  ifstream datafile;
  int counter = 0;                                                              // counter used to loop through lines of input_file

  string input_file, query_file, output_file;
  int iflag = 1, qflag = 1, oflag = 1;
  int k = 4, l = 5, n = 1, r = 10000;                                           // default values if not changed



  while ((opt = getopt(argc, argv, "i:q:k:L:o:N:R:")) != -1)
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
        }

  // options -i, -q, -o are mandatory
  if (iflag != 0 || qflag != 0 || oflag != 0){
    cout << "Error: Missing -i or -q or -o option\n";
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

  sttime=((double) clock())/CLOCKS_PER_SEC;


  LSH_solver solver1(input_file,query_file,output_file,k, l, n, r);
  // solver1.printQueries();
  solver1.solve();

  endtime=((double) clock())/CLOCKS_PER_SEC;
  cout << "time: " << endtime - sttime << endl;


  // display coordinates
  // points_coordinates[0].print_coordinates();
  // cout << "points_coordinates size = " << points_coordinates[0].get_coordinates_size() << endl;
  // cout << "points_coordinates size = " << points_coordinates[points_coordinates.size() - 1].get_coordinates_size() << endl;
  // points_coordinates[points_coordinates.size() - 1].print_coordinates();
  // cout << "size = " << points_coordinates.size() << endl;


  // hFunction h1(points_coordinates[0].get_coordinates_size());
  // hFunction h2(points_coordinates[points_coordinates.size() - 1].get_coordinates_size());
  //
  // cout << "sum = " << h1(points_coordinates[0]) << endl;
  // cout << "sum = " << h2(points_coordinates[points_coordinates.size() - 1]) << endl;

  // gFunction g1(points_coordinates[0].get_coordinates_size(), k, points_coordinates.size()/4);
  // gFunction g2(points_coordinates[0].get_coordinates_size(), k, points_coordinates.size()/4);
  //
  // int s = g1(points_coordinates[0]);
  // int f = g2(points_coordinates[points_coordinates.size() - 1]);
  // cout << "g1 = " << s << endl;
  // cout << "g2 = " << f << endl;
}

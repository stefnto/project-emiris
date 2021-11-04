#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <vector>
#include <time.h>
#include "Cube.hpp"
// #include "LSH.hpp"

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
  int k = 14, m = 5, probes = 2, n = 1, r = 10000;                                           // default values if not changed



  while ((opt = getopt(argc, argv, "i:q:k:M:p:o:N:R:")) != -1)
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
          case 'M':
                  m = atoi(optarg);
                  break;
          case 'p':
                  probes = atoi(optarg);
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
  cout << "M = " << m << "\n";
  cout << "probes = " << probes << "\n";
  cout << "N = " << n << "\n";
  cout << "R = " << r << "\n";


  Cube_Solver solver1(input_file, query_file, output_file, k, k, probes, n, r);
  // std::vector<Data_item*> points_coordinates;
  //
  // datafile.open(input_file);
  // if (datafile.is_open()){
  // std::string line;
  // while (getline(datafile, line)){
  //   counter++;
  //   points_coordinates.emplace_back(new Data_item(line)); // creates a 'Data_item' and puts it at the end of the vector 'points_coordinates'
  // }
  // datafile.close();
  // }

  // sttime=((double) clock())/CLOCKS_PER_SEC;
  //
  //
  // LSH_solver solver1(input_file,query_file,output_file,k, l, n, r);
  // // solver1.printQueries();
  // solver1.solve();
  //
  // endtime=((double) clock())/CLOCKS_PER_SEC;
  // cout << "time: " << endtime - sttime << endl;


  // display coordinates
  // points_coordinates[0].print_coordinates();
  // cout << "points_coordinates size = " << points_coordinates[0]->get_coordinates_size() << endl;
  // cout << "points_coordinates size = " << points_coordinates[points_coordinates.size() - 1]->get_coordinates_size() << endl;
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


  // Vertex_point ver[5];
  //
  // for (int i=0; i<5; i++){
  //   ver[i].init(k, points_coordinates[0]->get_coordinates_size());
  //   unsigned long long f = ver[i](points_coordinates[i], k);
  //   cout << f << endl;
  // }
}

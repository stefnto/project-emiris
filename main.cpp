#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <getopt.h>
#include <vector>
#include <time.h>

using namespace std;

int main(int argc, char *argv[]){

  double sttime, endtime;                                 // to compute total run time
  extern char *optarg;
  extern int optind;
  int opt;
  int number;
  vector<vector<int>> coordinates;                        // stores each vectors coordinates in each row
  ifstream datafile;
  int counter = 0;                                        // counter used to loop through lines of input_file

  string input_file, query_file, output_file;
  int iflag = 1, qflag = 1, oflag = 1;
  int k = 4, l = 5, n = 1, r = 10000;                     // default values if not changed





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

  // open input_file and get coordinates
  datafile.open(input_file);
  if (datafile.is_open()){
    string line;
    while (getline(datafile, line)){
      istringstream ss(line);
      while (ss >> number){
        coordinates.push_back(vector<int>());             // push vector that will store the coordinates, coordinates[i][0] displays the id
        coordinates[counter].push_back(number);           // push each coordinate
      }
      counter++;
    }
    datafile.close();
  }

  endtime=((double) clock())/CLOCKS_PER_SEC;
  cout << "time: " << endtime - sttime << endl;

  // display coordinates
    for (int j=0; j<coordinates[0].size(); j++){
      cout << coordinates[0][j] << " ";
    }
    cout << endl;
    //cout << "coordinates[0] size = " << coordinates[0].size() << endl;

    for (int j=0; j<coordinates[999999].size(); j++){
      cout << coordinates[999999][j] << " ";
    }
    cout << endl;
    //cout << "coordinates[999999] size = " << coordinates[999999].size() << endl;

    cout << "coordinates size = " << counter << endl;

}

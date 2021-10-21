#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <vector>

using namespace std;

int main(int argc, char *argv[]){

  extern char *optarg;
  extern int optind;
  int opt;
  vector<vector<string>> coordinates;                    // stores each vectors coordinates in each row
  vector<string> ids;                                    // stores the id of each vector
  ifstream datafile;
  int counter = 0;                                       // counter used to loop through lines of input_file

  string input_file, query_file, output_file;
  int iflag = 1, qflag = 1, oflag = 1;
  int k = 4, l = 5, n = 1, r = 10000;                    // default values if not changed





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

  // open input_file and get coordinates
  datafile.open(input_file);
  if (datafile.is_open()){
    string line;
    while (getline(datafile, line)){
      ids.push_back(line.substr(0,8));                    // push id shmeiou

      coordinates.push_back(vector<string>());            // push vector that will store the coordinates tou shmeiou

      coordinates[counter].push_back(line.substr(10,3));  // push coordinates
      coordinates[counter].push_back(line.substr(15,3));
      coordinates[counter].push_back(line.substr(20,3));
      counter++;
    }
    datafile.close();
  }

  // display coordinates
  for (int i=0; i< ids.size(); i++)
    cout << ids[i] << "\n";

  for (int i=0; i< coordinates.size(); i++){
    for (int j=0; j<coordinates[i].size(); j++){
      cout << coordinates[i][j] << " ";
    }
    cout << "\n";
  }

}

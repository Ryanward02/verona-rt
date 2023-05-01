#include <iostream>
#include <stdlib.h>
#include <iostream>
// #include "./test/bm/lotsofcowns/zipfDist.h"
#include <ctime>
#include <ratio>
#include <chrono>
#include <random>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

double parseZipfInput(char *input) {
  // input[0-7] = "zipfian["
  // input[8-n] = "number"
  // input[n+1] = "]"
  // extract 8-n and turn into double. Need to find n+1 = ];
  int n = 8;
  while (input[n] != ']') {
    n++;
  }

  char zipfNo[n-8];
  for (int i = 0; i < n-8; i++) {
    zipfNo[i] = input[i+8];
  }

  char *ptr;

  return strtod(zipfNo, &ptr);

}


int main(int argc, char **argv) {

    char* string = "zipfian<0.878>";

    std::cout << parseZipfInput(string) << std::endl;

    // std::cout << "(1)" << std::endl;
    // ScrambledZipfianGenerator gen = ScrambledZipfianGenerator(0, 100);
    // int indexList[10000];

    // for (int i = 0; i < 10000; i++) {
    //   int v = gen.nextValue();
    //   indexList[i] = v;
    // }

    // std::qsort(indexList,sizeof(indexList)/sizeof(*indexList),sizeof(*indexList), cmpInt);

    // for (int i = 0; i < 10000; i++) {
    //   std::cout << indexList[i] << "  ";
    // }
    // std::cout << std::endl;
    // std::string bashCall = "python3 /Users/ryanward/Documents/git_repos/verona-rt/graphOut.py " + graphout(zipflist);
    // system(bashCall.c_str());

    return 0;
}


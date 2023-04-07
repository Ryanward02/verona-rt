#include <iostream>
#include <stdlib.h>
#include <iostream>
#include "./test/bm/lotsofcowns/zipfDist.h"
#include <ctime>
#include <ratio>
#include <chrono>
#include <random>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    std::cout << "(1)" << std::endl;
    ScrambledZipfianGenerator gen = ScrambledZipfianGenerator(0, 100);
    int indexList[10000];

    for (int i = 0; i < 10000; i++) {
      int v = gen.nextValue();
      indexList[i] = v;
    }

    std::qsort(indexList,sizeof(indexList)/sizeof(*indexList),sizeof(*indexList), cmpInt);

    for (int i = 0; i < 10000; i++) {
      std::cout << indexList[i] << "  ";
    }
    std::cout << std::endl;
    // std::string bashCall = "python3 /Users/ryanward/Documents/git_repos/verona-rt/graphOut.py " + graphout(zipflist);
    // system(bashCall.c_str());

    return 0;
}


#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <random>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

std::string graphout(double zipflist[]) {
    std::string s = "";

    for (int i = 0; i < 100; i++) {

        s = s + std::to_string(zipflist[i]) + " ";
    }
    return s;
}

double H(int N, int s) {
  // if no skew is provided, assume a uniform distribution.
  double ret = 0;
  for (int i = 1; i <= N; i++) {
    ret += 1 / pow(i, s);
  }
  return ret;
}

double zipf(int x, int N, double s) {
  return ((1 / pow(x, s)) / H(N, s));
}

int main(int argc, char **argv) {
    double zipflist[100];
    for (int i = 0; i < 100; i++) {
        zipflist[i] = zipf(100 - i, 100, 0.01);
    }
    for (int i = 0; i < 100; i++) {
        std::cout << zipflist[i] << std::endl;
    }

    std::string bashCall = "python3 /Users/ryanward/Documents/git_repos/verona-rt/graphOut.py " + graphout(zipflist);
    system(bashCall.c_str());
}


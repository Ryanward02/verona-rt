#include <iostream>
#include <stdlib.h>
#include <cpp/when.h>
#include <snmalloc/snmalloc.h>
#include <verona.h>
#include <debug/harness.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <random>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

class cown
{
  // destructor. Not sure when this is called.
public:
  ~cown()
  {
    Logging::cout() << "Cown" << Logging::endl;
  }
};

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

using namespace verona::cpp;
void sub_array_random(cown_ptr<cown> arr[], cown_ptr<cown> sub_arr[], int count, int arr_len) {
  for (int i = 0; i <= count; i++) {
    int index = rand() % arr_len;
    // int index = (int)(zipf(rand() % arr_len, arr_len, 0.3) * (1000 * arr_len)) % arr_len;
    //   std::cout << index << std::endl;

    sub_arr[i] = arr[index];
  }
}
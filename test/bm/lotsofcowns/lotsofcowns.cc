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


/**
 * Tests execution time of a program that contains lots of cowns all being accessed by the same behaviour.
 * We will build a list of Cowns, each accessible separately, which are then staggered to a when block that
 * monitors the time. This will then be compared to the starting execution time.
 */


class cown
{
  // destructor. Not sure when this is called.
public:
  ~cown()
  {
    Logging::cout() << "Cown" << Logging::endl;
  }
};

using namespace verona::cpp;
void sub_array(cown_ptr<cown> arr[], cown_ptr<cown> sub_arr[], int count) {
  for (int i = 0; i <= count; i++) {
      sub_arr[i] = arr[i];
  }
}

using namespace verona::cpp;
void sub_array_sequential(cown_ptr<cown> arr[], cown_ptr<cown> sub_arr[], int count) {
  for (int i = 0; i <= count; i++) {
      sub_arr[i] = arr[i];
  }
}

using namespace verona::cpp;
void sub_array_random(cown_ptr<cown> arr[], cown_ptr<cown> sub_arr[], int count, int arr_len) {
  for (int i = 0; i <= count; i++) {
      sub_arr[i] = arr[rand() % arr_len]; // TODO: make random choice zipfian
  }
}

auto spin(double seconds) {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  duration<double> timespan = duration_cast<duration<double>>(t1 - t1);

  while (timespan.count() < seconds) {
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    timespan = duration_cast<duration<double>>(t2 - t1);
  }

}

std::string printTlist(duration<double> timeList[]) {
  std::string ret = "";
  
  for (int i = 0; i < 100; i++) {
    ret = ret + std::to_string(timeList[i].count()) + " ";
  }
  
  return ret;
}

using namespace verona::cpp;
void test_body()
{
  int no_of_cowns = 1000;

  cown_ptr<cown> cowns[no_of_cowns];
  for (int i = 0; i < no_of_cowns; i++) {
    cowns[i] = make_cown<cown>();
  }

  /** 
   * Created implementations for "lists" of cowns to be passed. 
   * When accepts a variable argument count, so we can provide a pointer to an array instead, 
   * which provides us with our arguments.
  */ 

  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  duration<double> timeList[no_of_cowns];
  int execCount = 0;

  for (int i = 0; i < no_of_cowns; i++) {

    int sub_arr_size = rand() % no_of_cowns;
    cown_ptr<cown> sub_arr[sub_arr_size+1]; 
    sub_array(cowns, sub_arr, sub_arr_size);

    when(cowns[i]) << [=, &timeList](auto){
      high_resolution_clock::time_point t2 = high_resolution_clock::now();

      auto time = ((float)(rand() % 1000) / 10000); // TODO: make spin time zipfian
      std::cout << "(" << (i+1) / 10 << "%) " << "Spin time:\t\t" << time << std::endl;

      spin(time);

      t2 = high_resolution_clock::now();
      
      duration<double> total = duration_cast<duration<double>>(t2 - t1);
      
      if ((i + 1) % 10 == 0) {
        std::memcpy(&timeList[(i + 1) / 10], &total, sizeof(total));
      }


      std::cout << "(" << (i+1) / 10 << "%) " << "Execution Time: \t\t" << total.count() << std::endl;

      if (i == no_of_cowns - 1) {
        std::cout << printTlist(timeList) << std::endl;
        std::string bashCall = "python3 /Users/ryanward/Documents/git_repos/verona-rt/graphOut.py " + printTlist(timeList);
        system(bashCall.c_str());
      }
        
    };
  }
}


int main(int argc, char** argv) {
  
  SystematicTestHarness harness(argc, argv);
  harness.run(test_body);

  // Get time here to find out how long the whole test took.
  return 0;
}
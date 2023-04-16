#include <iostream>
#include <stdlib.h>
#include <cpp/when.h>
#include <snmalloc/snmalloc.h>
#include <verona.h>
#include <debug/harness.h>
#include <zipfDist.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <random>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>


/**
 * Tests execution time of a program that contains lots of cowns all being accessed by the same behaviour.
 * We will build a list of Cowns, each accessible separately, which are then staggered to a when block that
 * monitors the time. This will then be compared to the starting execution time.
 */

// using namespace verona::cpp;
// void sub_array(cown_ptr<cown> arr[], cown_ptr<cown> sub_arr[], int count) {
//   for (int i = 0; i <= count; i++) {
//       sub_arr[i] = arr[i];
//   }
// }

// using namespace verona::cpp;
// void sub_array_sequential(cown_ptr<cown> arr[], cown_ptr<cown> sub_arr[], int count) {
//   for (int i = 0; i <= count; i++) {
//       sub_arr[i] = arr[i];
//   }
// }
using namespace verona::cpp;
bool isIn(cown_ptr<cown> item, cown_ptr<cown> arr[], int arr_len) {
    for (int i = 0; i < arr_len; i++) {
        if (item == arr[i]) {
            return true;
        }
    }
    return false;
}

using namespace verona::cpp;

void sub_array_random(cown_ptr<cown> arr[], cown_ptr<cown> sub_arr[], int count, int arr_len, double zipfConstant = 0.99) {
  ZipfianGenerator gen = ZipfianGenerator(0, arr_len - 1, zipfConstant);
  for (int i = 0; i <= count; i++) {
    int index = gen.nextValue();
    // int index = i;
    sub_arr[i] = arr[index];
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

std::string printTlist(std::vector<double> *timeList, int length) {
  
  // std::qsort(timeList,sizeof(timeList)/sizeof(*timeList),sizeof(*timeList), cmpDouble);
   
  std::string ret = "";
  
  for (int i = 0; i < length; i++) {
    std::cout << "time (i): " << std::to_string(timeList->at(i)) << std::endl;
    ret = ret + std::to_string(timeList->at(i)) + " ";
  }
  return ret;
}
using namespace verona::cpp;
using namespace verona::rt;
void test_body()
{
  int no_of_cowns = 1000;

  // std::vector<cown_ptr<cown>> cowns;
  cown_ptr<cown> cowns[no_of_cowns];
  for (int i = 0; i < no_of_cowns; i++) {
    // cowns.push_back(make_cown<cown>(i));
    cowns[i] = make_cown<cown>(i);

    // cowns[i] = new Cown();
  }

  /** 
   * Created implementations for "lists" of cowns to be passed. 
   * When accepts a variable argument count, so we can provide a pointer to an array instead, 
   * which provides us with our arguments.
  */ 

  // TODO: move into for loop, and make timeList cumulative (during post-execution).
  high_resolution_clock::time_point t1;

  pthread_mutex_t lock;

  pthread_mutex_init(&lock, NULL);

  ScrambledZipfianGenerator *gen = new ScrambledZipfianGenerator(0, no_of_cowns);

  // Define how many when blocks you wish to call.
  const int no_of_whens = 1000;

  std::vector<double>* timeList = new std::vector<double>;
  timeList->reserve(no_of_whens);
  
  std::cout << "size: " << timeList->size() << std::endl;
  // duration<double> timeList[no_of_cowns];
  
  for (int i = 0; i < no_of_whens; i++) {
    // int sub_arr_size = no_of_iterations;

    int sub_arr_size = gen->nextValue();

    cown_ptr<cown> sub_arr[sub_arr_size+1]; 

    sub_array_random(cowns, sub_arr, sub_arr_size, no_of_cowns, 2);

    // std::cout << "given sub_arr_size: " << in << std::endl;

    t1 = high_resolution_clock::now();

    when(*sub_arr) << [=, &lock](auto){
      high_resolution_clock::time_point t2 = high_resolution_clock::now();

      // double time = (double)(gen->nextValue() / 500);
      double time = 0.1;

      spin(time);
      
      duration<double> total = duration_cast<duration<double>>(t2 - t1);
      pthread_mutex_lock(&lock);

      std::cout << std::to_string((int)((timeList->size() / no_of_whens) * 100)) << "%\t" << total.count() << " seconds" << std::endl;

      timeList->push_back(total.count());
      std::cout << "size: " << timeList->size() << std::endl;
      // pthread_mutex_unlock(&lock);
      
      // pthread_mutex_lock(&lock);
      if (timeList->size() == no_of_whens - 1) {
        std::cout << printTlist(timeList, timeList->size()) << std::endl;
        pthread_mutex_unlock(&lock);
        std::string bashCall = "python3 /Users/ryanward/Documents/git_repos/verona-rt/graphOut.py " + printTlist(timeList, timeList->size());
        system(bashCall.c_str());
      } else {
        pthread_mutex_unlock(&lock);
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
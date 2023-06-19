#include <stdlib.h>
#include <cpp/when.h>
#include <snmalloc/snmalloc.h>
#include <verona.h>
#include <debug/harness.h>
#include <zipfDist.h>
#include <ratio>
#include <chrono>
#include <random>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iostream>


/**
 * Tests the scheduling latency of behaviours based on how cowns are distributed. 
 * We create lots of cowns, and have a function that creates a subarray of cowns based on a Zipfian distribution.
 * We then create lots of whens, and time how long they take to begin executing. 
*/

using namespace verona::cpp;
void sub_array_random(std::vector<cown_ptr<cown>>* arr, std::vector<cown_ptr<cown>>* sub_arr, int count, bool uniform, double zipfConstant = 0.99) {
  Generator *gen = new Generator;
  std::set<int> indexes;
  if (uniform) {
    gen = new UniformGenerator(0, int(arr->size()) - 1);
  } else {
    gen = new ZipfianGenerator(0, int(arr->size() - 1), zipfConstant);
  }
  for (int i = 0; i < count; i++) {
    int index = gen->nextValue();
    while (!indexes.emplace(index).second) {
      index = gen->nextValue();
    }
    cown_ptr<cown> ptr = arr->at(index);
    sub_arr->emplace_back(ptr);
  }

}

double parseZipfInput(char *input) {
  // input[0-7] = "zipfian["
  // input[8-n] = "number"
  // input[n+1] = "]"
  // extract 8-n and turn into double. Need to find n+1 = >;
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

void parseServiceTime(char *input, bool fixed_servicetime, int *serviceTime1, int *serviceTime2, int *serviceTime3) {
  // Fixed or exponential take 1 input
  if (fixed_servicetime || input[0] == 'e' || input[0] == 'E') {
    // "fixed[" is length 6, so index 6 is first value of time;
    std::string s = input;
    // remove naming
    s.erase(0, s.find('[') + 1);
    std::string serviceTime = s.substr(0, s.find(']'));
    *serviceTime1 = atoi(serviceTime.c_str());
  } else {
    // Bimodal takes 3 inputs
    std::string s = input;
    s.erase(0, s.find('[') + 1);
    
    std::string low_time = s.substr(0, s.find(':'));
    s.erase(0, s.find(':') + 1);
    std::string high_time = s.substr(0, s.find(':'));
    s.erase(0, s.find(':') + 1);
    std::string percentage = s.substr(0, s.find(']'));

    *serviceTime1 = atoi(low_time.c_str());
    *serviceTime2 = atoi(high_time.c_str());
    *serviceTime3 = atoi(percentage.c_str());
  }
}

/**
 * Spins for the time given via the 'seconds' parameter.
*/
auto spin(double seconds) {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  duration<double> timespan = duration_cast<duration<double>>(t1 - t1);

  while (timespan.count() < seconds) {
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    timespan = duration_cast<duration<double>>(t2 - t1);
  }

}



/**
 * Function to create a printable list of doubles. 
 * This allows us to use as input for argv inside python script (which gives us graph)
*/
std::string printTlist(std::vector<double> *timeList, int length) {
     
  std::string ret = "";
  
  for (int i = 0; i < length; i++) {
    ret = ret + std::to_string(timeList->at(i)) + " ";
  }
  return ret;
}

std::string printParallelTlist(std::vector<double> *timeList1, std::vector<double> *timeList2) {
  
  std::vector<double> *mergedList = new std::vector<double>;

  for (int i = 0; i < timeList1->size(); i++) {
    mergedList->emplace_back(timeList1->at(i));
  }
  for (int i = 0; i < timeList2->size(); i++) {
    mergedList->emplace_back(timeList2->at(i));
  }

  // std::cout << mergedList->size() << std::endl;
  std::sort(mergedList->begin(), mergedList->end());

  std::string ret = "";
  
  for (size_t i = 0; i < mergedList->size(); i++) {
    ret = ret + std::to_string(mergedList->at(i)) + " ";
  }
  return ret;

}

/**
 * Main test body. Creates large list of cowns
  * Given no of when blocks, schedules all when blocks
  * Creates and initialises a lock to stop concurrent data access to timeList
  * Gets the current time.
  * Generates a new cown list for each "when" based on our given zipf dist.
    * Schedules the when block. Inside the when block we:
      * Get the time again. This is the time between schedule and execution.
      * Spin (simulate a behaviour being executed) for a certain amount of time. 
      * This can be distributed, or it can be fixed.
      * We get the difference in times, and lock our mutex.
        * Add the time count to timeList.
        * if our timeList has the same as the no of iterations, we have reached our final when block. 
          * As such, we can call printTList which gives us the input for our system call to the python script.
          * so we can create the system call (variable bashCall) and unlock our mutex.
          * Then we call python to create our graph.
        * Otherwise, we just unlock the mutex.
 * 
*/
using namespace verona::cpp;
using namespace verona::rt;
void test_body(int argc, char** argv)
{
  bool parallel = false;
  bool uniform_cowns = false;
  double cownConstant = double(2);
  bool fixed_servicetime = true;
  bool expo_servicetime = false;
  int serviceTime1 = 100;
  int serviceTime2 = 1000;
  int serviceTime3 = 95;
  int no_of_cowns = int(1000);
  int sub_arr_percentage = int(5);
  int no_of_whens = int(1000);
  std::string argString = "";

  

  // Parsing cmd inputs. ASSUMING VALID INPUT IF EXISTS.
  // Input is format: ./lotsofcowns --cownPop uniform|zipfian[const] --cownCount no_of_cowns --whenCount no_of_whens 
  //                    --servTime fixed[time] | expo[base,initial_exponent,repeat_exponent] | bimodal[time_low, time_high, ratio].
  for (int i = 0; i < argc; i++) {
    if (i > 0) {
      argString = argString + argv[i] + " ";
    }
    if (strcmp(argv[i], "--parallel") == 0) {
      parallel = true;
      std::cout << "Atomic Schedule, parallel execution" << std::endl;
    }
    if (strcmp(argv[i], "--cownPop") == 0) {
      if (strcmp(argv[i+1], "uniform") == 0) {
        uniform_cowns = true;
        std::cout << "Uniform Cown Population" << std::endl;
      } else {
        cownConstant = parseZipfInput(argv[i+1]);
        std::cout << "Zipfian cown population with constant:" << std::to_string(cownConstant) << std::endl;
      }
    }
    if (strcmp(argv[i], "--totalCowns") == 0) {
      std::cout << argv[i+1] << " cowns" << std::endl;
      no_of_cowns = atoi(argv[i+1]);
    }
    if (strcmp(argv[i], "--behaviourCowns") == 0) {
      std::cout << argv[i+1] << " cowns" << std::endl;
      sub_arr_percentage = atoi(argv[i+1]);
    }

    if (strcmp(argv[i], "--whenCount") == 0) {
      std::cout << argv[i+1] << " when blocks" << std::endl;
      no_of_whens = atoi(argv[i+1]);
    }
    if (strcmp(argv[i], "--servTime") == 0) {
      // Fixed, bimodal, e.g. fixed[10] (input in milliseconds)
      
      if (argv[i+1][0] == 'f') {
        std::cout << argv[i+1] << std::endl;
        fixed_servicetime = true;
        std::cout << "Fixed Service Time" << std::endl;
      } else {
        fixed_servicetime = false;
        if (argv[i+1][0] == 'b') {
          expo_servicetime = false;
          std::cout << "Bimodal Service Time" << std::endl;
        } else {
          expo_servicetime = true;
          std::cout << "Exponential Service Time" << std::endl;
        }
      }
      parseServiceTime(argv[i+1], fixed_servicetime, &serviceTime1, &serviceTime2, &serviceTime3);
    }
  }

  int sub_arr_size = (double)no_of_cowns * ((double)sub_arr_percentage / (double)100);
  std::vector<cown_ptr<cown>> *cowns = new std::vector<cown_ptr<cown>>;
  for (int i = 0; i < no_of_cowns; i++) {
    cowns->emplace_back(make_cown<cown>());

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

  // UniformGenerator *gen = new UniformGenerator(0, no_of_cowns);
  // ScrambledZipfianGenerator *gen = new ScrambledZipfianGenerator(0, no_of_cowns);
  
  Generator *timeDist;

  if (fixed_servicetime) {
    timeDist = new FixedGenerator(serviceTime1);
  } else {
    if (expo_servicetime) {
      timeDist = new ExponentialGenerator(serviceTime1);
    } else {
      timeDist = new BimodalGenerator(serviceTime1, serviceTime2, serviceTime3);
    }
  }

  

  if (parallel && fixed_servicetime) {

    std::vector<double> *timeList1 = new std::vector<double>;
    std::vector<double> *timeList2 = new std::vector<double>;

    bool *finished = new bool(false);

    for (int i = 0; i < (no_of_whens); i++) {

      t1 = high_resolution_clock::now();

      std::vector<cown_ptr<cown>>* sub_arr = new std::vector<cown_ptr<cown>>;
      std::vector<cown_ptr<cown>>* sub_arr1 = new std::vector<cown_ptr<cown>>(); 
      std::vector<cown_ptr<cown>>* sub_arr2 = new std::vector<cown_ptr<cown>>();

      sub_array_random(cowns, sub_arr, sub_arr_size, uniform_cowns, cownConstant);

      // std::cout << sub_arr_size << std::endl;
      // std::cout << sub_arr->size() << std::endl;

      for (int i = 0; i < sub_arr->size() / (size_t)2; i++) {
        sub_arr1->push_back(sub_arr->at(i));
      }
      for (int i = sub_arr->size() / (size_t)2; i < sub_arr->size(); i++) {
        sub_arr2->push_back(sub_arr->at(i));
      }

      // std::cout << "sub arr 1: " << sub_arr1->size() << std::endl;
      // std::cout << "sub arr 2: " << sub_arr2->size() << std::endl;
      
      when2(*sub_arr1, *sub_arr2, 
        [=](auto){
          double time = double(timeDist->nextValue()) / (double)1000; // timeDist gives value in milliseconds; must convert to seconds.
                  
          // double time = 0.1;

          high_resolution_clock::time_point t2 = high_resolution_clock::now();
          spin(time / double(2));
          
          duration<double> total = duration_cast<duration<double>>(t2 - t1);
          
          std::cout << "(1): " << timeList1->size() << " of " << no_of_whens << "\t" << total.count() << " seconds" << std::endl;
          // pthread_mutex_lock(&lock);
          timeList1->emplace_back(total.count());
          if (timeList1->size() == size_t(no_of_whens)) {
            if (*finished) {
              std::string bashCall = "python3 /Users/ryanward/Documents/git_repos/verona-rt/graphOut.py --csv " + argString + printParallelTlist(timeList1, timeList2);
              // pthread_mutex_unlock(&lock);
              system(bashCall.c_str());
            } else {
              *finished = true;
              // pthread_mutex_unlock(&lock);
            }
          }
          
        }, [=](auto){
          double time = double(timeDist->nextValue()) / (double)1000; // timeDist gives value in milliseconds; must convert to seconds.

          high_resolution_clock::time_point t2 = high_resolution_clock::now();
          spin(time / double(2));
          duration<double> total = duration_cast<duration<double>>(t2 - t1);
          std::cout << "(2): " << timeList2->size() << " of " << no_of_whens << "\t" << total.count() << " seconds" << std::endl;
          // pthread_mutex_lock(&lock);
          timeList2->emplace_back(total.count());
          if (timeList2->size() == size_t(no_of_whens)) {
            if (*finished) {
              std::string bashCall = "python3 /Users/ryanward/Documents/git_repos/verona-rt/graphOut.py --csv " + argString + printParallelTlist(timeList1, timeList2);
              // pthread_mutex_unlock(&lock);
              system(bashCall.c_str());
              
            } else {
              *finished = true;
              // pthread_mutex_unlock(&lock);
            }
          }
          
        }
      );
    }
  } else {

    std::vector<double>* timeList = new std::vector<double>;

    for (int i = 0; i < no_of_whens; i++) {

      std::vector<cown_ptr<cown>>* sub_arr = new std::vector<cown_ptr<cown>>; 

      sub_array_random(cowns, sub_arr, sub_arr_size, uniform_cowns, cownConstant);

      t1 = high_resolution_clock::now();

      if (parallel) {
        std::cout << "Cannot be parallel - not fixed service time" << std::endl;
        return;
      }
      when(*sub_arr->data()) << [=, &lock](auto){

        double time = double(timeDist->nextValue()) / (double)1000; // timeDist gives value in milliseconds; must convert to seconds.
        std::cout << time << " spin " << std::endl;
        // double time = 0.1;
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        spin(time);
        
        duration<double> total = duration_cast<duration<double>>(t2 - t1);
        

        // if (fmod(double(timeList->size()) / double(no_of_whens) * double(100), 10) == 0) {
        //   std::cout << (double)timeList->size() / (double)no_of_whens * double(100) << " percent complete" << std::endl;
        // }
        
        pthread_mutex_lock(&lock);
        std::cout << timeList->size() << " of " << no_of_whens << "\t" << total.count() << " seconds" << std::endl;
        timeList->emplace_back(total.count());
        if (timeList->size() == size_t(no_of_whens)) {
          std::string bashCall = "python3 /Users/ryanward/Documents/git_repos/verona-rt/graphOut.py --csv " + argString + printTlist(timeList, timeList->size());
          pthread_mutex_unlock(&lock);
          system(bashCall.c_str());
        } else {
          pthread_mutex_unlock(&lock);
        }
      };
    }
  }
}

/**
 * Function Main body. Purely for calling test_body. Uses Systematic testing.
*/
int main(int argc, char** argv) {
  
  SystematicTestHarness harness(argc, argv);

  // An update for snmalloc was causing a debug allocation error, so this needs to be disabled. 
  harness.detect_leaks = false;
  
  harness.run(test_body, argc, argv);

  // test_body(argc, argv);

  return 0;
}
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
  Generator *gen;
  if (uniform) {
    gen = new UniformGenerator(0, int(arr->size()) - 1);
  } else {
    gen = new ZipfianGenerator(0, arr->size() - 1, zipfConstant);
  }
  for (int i = 0; i <= count; i++) {
    int index = gen->nextValue();
    cown_ptr<cown> ptr = arr->at(index);
    sub_arr->emplace_back(ptr);
  }
}

double parseZipfInput(char *input) {
  // input[0-7] = "zipfian<"
  // input[8-n] = "number"
  // input[n+1] = ">"
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

  bool uniform_cowns = false;
  double cownConstant = double(2);
  bool uniform_servicetime = false;
  double servConstant = double(2);
  int no_of_cowns = int(1000);
  int no_of_whens = int(1000);


  // Parsing cmd inputs. ASSUMING VALID INPUT IF EXISTS.
  // Input is format: ./lotsofcowns --cownPop uniform|zipfian[const] --cownCount no_of_cowns --whenCount no_of_whens --servTime uniform|zipfian[const].
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "--cownPop") == 0) {
      if (strcmp(argv[i+1], "uniform") == 0) {
        uniform_cowns = true;
        std::cout << "uniform cown population" << std::endl;
      } else {
        cownConstant = parseZipfInput(argv[i+1]);
        std::cout << "zipfian cown population with zipfian constant:" << std::to_string(cownConstant) << std::endl;
      }
    }
    if (strcmp(argv[i], "--cownCount") == 0) {
      std::cout << argv[i+1] << " cowns" << std::endl;
      no_of_cowns = atoi(argv[i+1]);
    }
    if (strcmp(argv[i], "--whenCount") == 0) {
      std::cout << argv[i+1] << " when blocks" << std::endl;
      no_of_whens = atoi(argv[i+1]);
    }
    if (strcmp(argv[i], "--servTime") == 0) {
      if (strcmp(argv[i+1], "uniform") == 0) {
        std::cout << "uniform service time" << std::endl;
        uniform_servicetime = true;
      } else {
        // Do same as cownPop but for serviceTime Generator
        std::cout << argv[i+1] << std::endl;
        servConstant = parseZipfInput(argv[i+1]);
        std::cout << "zipfian service time with zipfian constant:" << std::to_string(servConstant) << std::endl;
      }
    }
  }



  std::vector<cown_ptr<cown>> *cowns = new std::vector<cown_ptr<cown>>;
  for (int i = 0; i < no_of_cowns; i++) {
    cowns->emplace_back(make_cown<cown>(i));

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

  Generator *gen;

  if (uniform_cowns) {
    gen = new UniformGenerator(no_of_cowns);
  } else {
    gen = new ScrambledZipfianGenerator(0, int(no_of_cowns-1), cownConstant);
  }

  // UniformGenerator *gen = new UniformGenerator(0, no_of_cowns);
  // ScrambledZipfianGenerator *gen = new ScrambledZipfianGenerator(0, no_of_cowns);
  
  Generator *timeDist;

  if (uniform_servicetime) {
    timeDist = new UniformGenerator(10, 30);
  } else {
    timeDist = new ZipfianGenerator(10, 30, servConstant);
  }

  std::vector<double>* timeList = new std::vector<double>;
  timeList->reserve(no_of_whens);
  
  
  for (int i = 0; i < no_of_whens; i++) {

    int sub_arr_size = gen->nextValue();

    std::vector<cown_ptr<cown>>* sub_arr = new std::vector<cown_ptr<cown>>; 

    sub_array_random(cowns, sub_arr, sub_arr_size, uniform_cowns, cownConstant);

    t1 = high_resolution_clock::now();

    when(*sub_arr->data()) << [=, &lock](auto){

      double time = (double)timeDist->nextValue() / (double)200;
      std::cout << time << " service" << std::endl;
      // Gets zipf-dist rand.var between 0.05 and 0.15.
       
      // std::cout << time << " spin " << std::endl;
      
      // double time = 0.1;

      spin(time);
      
      high_resolution_clock::time_point t2 = high_resolution_clock::now();
      duration<double> total = duration_cast<duration<double>>(t2 - t1);
      

      // if (fmod(double(timeList->size()) / double(no_of_whens) * double(100), 10) == 0) {
      //   std::cout << (double)timeList->size() / (double)no_of_whens * double(100) << " percent complete" << std::endl;
      // }
      
      pthread_mutex_lock(&lock);
      std::cout << timeList->size() << " of " << no_of_whens << "\t" << total.count() << " seconds" << std::endl;
      timeList->emplace_back(total.count());

      if (int(timeList->size()) == int(no_of_whens)) {
        std::cout << printTlist(timeList, timeList->size()) << std::endl;
        std::string bashCall = "python3 /Users/ryanward/Documents/git_repos/verona-rt/graphOut.py " + printTlist(timeList, timeList->size());
        std::ofstream out("../test.txt");
        out << printTlist(timeList, timeList->size());
        out.close();
        pthread_mutex_unlock(&lock);
        system(bashCall.c_str());
      } else {
        pthread_mutex_unlock(&lock);
      }
    };
  }
}

/**
 * Function Main body. Purely for calling test_body. Uses Systematic testing.
*/
int main(int argc, char** argv) {
  
  SystematicTestHarness harness(argc, argv);
  // Add command line input for "distribution"
  // If uniform, no parameters
  // If zipfian, allow parameter for constant "zipf:0.5" for example.
  
  // number of cowns
  // cown popularity
  // service time distribution
  harness.run(test_body, argc, argv);

  return 0;
}
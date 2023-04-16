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


/**
 * Tests execution time of a program that contains lots of cowns all being accessed by the same behaviour.
 * We will build a list of Cowns, each accessible separately, which are then staggered to a when block that
 * monitors the time. This will then be compared to the starting execution time.
 */

using namespace verona::cpp;
void sub_array_random(std::vector<cown_ptr<cown>>* arr, std::vector<cown_ptr<cown>>* sub_arr, int count, double zipfConstant = 0.99) {
  ZipfianGenerator gen = ZipfianGenerator(0, arr->size() - 1, zipfConstant);
  for (int i = 0; i <= count; i++) {
    int index = gen.nextValue();
    // int index = i;
    cown_ptr<cown> ptr = arr->at(index);
    sub_arr->emplace_back(ptr);
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

/**
 * Main test body. Creates large list of cowns
  * Given no of when blocks, schedules all when blocks
  * Creates and initialises a lock to stop concurrent data access to timeList
  * Gets the current time.
  * Generates a new cown list for each block based on our given zipf dist.
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
void test_body()
{
  int no_of_cowns = 1000;
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

  ScrambledZipfianGenerator *gen = new ScrambledZipfianGenerator(0, no_of_cowns);
  ZipfianGenerator *timeDist = new ZipfianGenerator(10, 30);

  // Define how many when blocks you wish to call.
  const int no_of_whens = 1000;

  std::vector<double>* timeList = new std::vector<double>;
  timeList->reserve(no_of_whens);
  
  std::cout << "size: " << timeList->size() << std::endl;
  
  for (int i = 0; i < no_of_whens; i++) {

    int sub_arr_size = gen->nextValue();

    std::vector<cown_ptr<cown>>* sub_arr = new std::vector<cown_ptr<cown>>; 

    sub_array_random(cowns, sub_arr, sub_arr_size, 2);


    t1 = high_resolution_clock::now();

    when(*sub_arr->data()) << [=, &lock](auto){

      double time = ((double)timeDist->nextValue() / (double)100); // Gets zipf-dist rand.var between 0.1 and 0.2.
      std::cout << time << " spin " << std::endl;
      
      // double time = 0.1;

      spin(time);
      
      high_resolution_clock::time_point t2 = high_resolution_clock::now();
      duration<double> total = duration_cast<duration<double>>(t2 - t1);
      pthread_mutex_lock(&lock);

      std::cout << timeList->size() << " of " << no_of_whens << "\t" << total.count() << " seconds" << std::endl;

      timeList->push_back(total.count());

      if (timeList->size() == no_of_whens - 1) {
        std::cout << printTlist(timeList, timeList->size()) << std::endl;
        std::string bashCall = "python3 /Users/ryanward/Documents/git_repos/verona-rt/graphOut.py " + printTlist(timeList, timeList->size());
        pthread_mutex_unlock(&lock);
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

  return 0;
}
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
auto sub_array(cown_ptr<cown> list[], int count) {
  if (count > (sizeof(list)/sizeof(list[0]))) {
    return list;
  }
  
  cown_ptr<cown> new_list[count];
  for (int i = 0; i < count; i++) {
    new_list[i] = list[i];
  }

  return new_list;

}

auto spin(double seconds) {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  duration<double> timespan = duration_cast<duration<double>>(t1 - t1);

  while (timespan.count() < seconds) {
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    timespan = duration_cast<duration<double>>(t2 - t1);
  }

}

using namespace verona::cpp;
void test_body()
{
  // Get time here

  cown_ptr<cown> cowns[1000];
  for (int i = 0; i < 1000; i++) {
    cowns[i] = make_cown<cown>();
  }

  /** 
   * Created implementations for "lists" of cowns to be passed. 
   * When accepts a variable argument count, so we can provide a pointer to an array instead, 
   * which provides us with our arguments.
  */ 

  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  when(*cowns) << [t1](auto) 
  { 
    // WORKING: implementation of a 5 second spin in the while.
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    
    spin(2.5);

    t2 = high_resolution_clock::now();
    duration<double> total = duration_cast<duration<double>>(t2 - t1);

    // Time from schedule to completion of behaviour
    std::cout << "total time taken:\t" << total.count() << std::endl;
  };
  
  when(*sub_array(cowns, 400)) << [](auto) { Logging::cout() << "log" << Logging::endl; };

  // Get time here. This will tell us the time it takes to create lots of cowns
  


  // Get time here. 
  
  // when(c1, c2, c3, c4, c5) << [](auto, auto, auto, auto, auto) 
  // {
  //   // and here. Will tell us when this block begins executing (and therefore how long 5/20 of the last block of whens took.)
  //   Logging::cout() << "log" << Logging::endl; 
  // };

  // // then here. Tells us how the rt handles an overlap of cowns.
  // when(c3, c4, c5, c6, c7) << [](auto, auto, auto, auto, auto) 
  // {
  //   // and here
  //   Logging::cout() << "log" << Logging::endl; 
  // };

  // // then here. Should only execute after everything else has completed.
  // when(c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20) << 
  //   [](auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto) 
  // {
  //   // and here. Will tell us how long all the above took to execute.
  //   Logging::cout() << "log" << Logging::endl;
  // };
  
}


int main(int argc, char** argv) {
  
  SystematicTestHarness harness(argc, argv);
  harness.run(test_body);

  // Get time here to find out how long the whole test took.
  return 0;
}
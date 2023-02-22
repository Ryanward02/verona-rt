#include <iostream>
#include <stdlib.h>
#include <cpp/when.h>
#include <snmalloc/snmalloc.h>
#include <verona.h>
#include <debug/harness.h>


/**
 * Designed to test the time between a behaviour being scheduled and a behaviour being executed.
 */


class cown
{
public:
  ~cown()
  {
    Logging::cout() << "Cown" << Logging::endl;
  }
};

using namespace verona::cpp;
void test_body()
{
  cown_ptr<cown> c1 = make_cown<cown>();


  // Get Time here
  when(c1) << [](auto) {
    // Then get time here. This should describe the discrepancy between when a when block is scheduled and when it begins execution
    Logging::cout() << "log" << Logging::endl; 
  };
}


int main(int argc, char** argv) 
{
  
  SystematicTestHarness harness(argc, argv);
  harness.run(test_body);

  return 0;
}
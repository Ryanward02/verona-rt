#include <iostream>
#include <stdlib.h>
#include <cpp/when.h>
#include <snmalloc/snmalloc.h>
#include <verona.h>
#include <debug/harness.h>


/**
 * Tests execution time of a program that contains lots of cowns all being accessed by the same behaviour.
 * We will build a list of Cowns, each accessible separately, which are then staggered to a when block that
 * monitors the time. This will then be compared to the starting execution time.
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
  cown_ptr<cown> c2 = make_cown<cown>();
  cown_ptr<cown> c3 = make_cown<cown>();
  cown_ptr<cown> c4 = make_cown<cown>();
  cown_ptr<cown> c5 = make_cown<cown>();


  // Get time here
  when(c1) << [](auto) { 
    // For each of these, get the time here.
    Logging::cout() << "log" << Logging::endl; 
  };

  // How will verona handle 20 identical behaviour and cown blocks? these should run sequentially. 
  // We should see a speed improvement using our optimisations since they aren't actually using c1, so can be de-critical sectioned!!.
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };


  // Take time here. Will c1 and c2 run together? they should. These would need c1,c2 to be sequential.
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };


  when(c1) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c3) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c4) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c5) << [](auto) { Logging::cout() << "log" << Logging::endl; };

  // will wait for everything to finish?
  when(c1, c2, c3, c4, c5) << [](auto, auto, auto, auto, auto) 
  {
    Logging::cout() << "log" << Logging::endl; 
  };
}


int main(int argc, char** argv) {
  
  SystematicTestHarness harness(argc, argv);
  harness.run(test_body);

  return 0;
}
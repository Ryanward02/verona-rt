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
  // Get time here
  cown_ptr<cown> c1  = make_cown<cown>();
  cown_ptr<cown> c2  = make_cown<cown>();
  cown_ptr<cown> c3  = make_cown<cown>();
  cown_ptr<cown> c4  = make_cown<cown>();
  cown_ptr<cown> c5  = make_cown<cown>();
  cown_ptr<cown> c6  = make_cown<cown>();
  cown_ptr<cown> c7  = make_cown<cown>();
  cown_ptr<cown> c8  = make_cown<cown>();
  cown_ptr<cown> c9  = make_cown<cown>();
  cown_ptr<cown> c10 = make_cown<cown>();
  cown_ptr<cown> c11 = make_cown<cown>();
  cown_ptr<cown> c12 = make_cown<cown>();
  cown_ptr<cown> c13 = make_cown<cown>();
  cown_ptr<cown> c14 = make_cown<cown>();
  cown_ptr<cown> c15 = make_cown<cown>();
  cown_ptr<cown> c16 = make_cown<cown>();
  cown_ptr<cown> c17 = make_cown<cown>();
  cown_ptr<cown> c18 = make_cown<cown>();
  cown_ptr<cown> c19 = make_cown<cown>();
  cown_ptr<cown> c20 = make_cown<cown>();

  // Get time here. This will tell us the time it takes to create lots of cowns
  when(c1)  << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c2)  << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c3)  << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c4)  << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c5)  << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c6)  << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c7)  << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c7)  << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c8)  << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c9)  << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c10) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c11) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c12) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c13) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c14) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c15) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c16) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c17) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c18) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c19) << [](auto) { Logging::cout() << "log" << Logging::endl; };
  when(c20) << [](auto) { Logging::cout() << "log" << Logging::endl; };


  // Get time here. 
  when(c1, c2, c3, c4, c5) << [](auto, auto, auto, auto, auto) 
  {
    // and here. Will tell us when this block begins executing (and therefore how long 5/20 of the last block of whens took.)
    Logging::cout() << "log" << Logging::endl; 
  };

  // then here. Tells us how the rt handles an overlap of cowns.
  when(c3, c4, c5, c6, c7) << [](auto, auto, auto, auto, auto) 
  {
    // and here
    Logging::cout() << "log" << Logging::endl; 
  };

  // then here. Should only execute after everything else has completed.
  when(c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20) << 
    [](auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto, auto) 
  {
    // and here. Will tell us how long all the above took to execute.
    Logging::cout() << "log" << Logging::endl;
  };
}


int main(int argc, char** argv) {
  
  SystematicTestHarness harness(argc, argv);
  harness.run(test_body);

  // Get time here to find out how long the whole test took.
  return 0;
}
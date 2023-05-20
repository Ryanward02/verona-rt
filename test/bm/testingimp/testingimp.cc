#include <stdlib.h>
#include <cpp/when.h>
#include <snmalloc/snmalloc.h>
#include <verona.h>
#include <debug/harness.h>
#include <ratio>
#include <chrono>
#include <random>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iostream>

class cown_obj
{
public:
    ~cown_obj() {
        std::cout << "cown" << std::endl;
    }
};

auto spin_and_print(double seconds) {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  duration<double> timespan = duration_cast<duration<double>>(t1 - t1);

  while (timespan.count() < seconds) {
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    timespan = duration_cast<duration<double>>(t2 - t1);
  }

}

using namespace verona::cpp;
using namespace verona::rt;
auto test_body() {
    cown_ptr<cown_obj> c1 = make_cown<cown_obj>();
    cown_ptr<cown_obj> c2 = make_cown<cown_obj>();
    cown_ptr<cown_obj> c3 = make_cown<cown_obj>();

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    // WARNING: cown_list_1, cown_list_2 must not be empty.
    when2(cowns<cown_ptr<cown_obj>>(c1), cowns<cown_ptr<cown_obj>>(c2, c3), 
        [=](auto) {
            spin_and_print(2);
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> total = duration_cast<duration<double>>(t2 - t1);
            std::cout << "total execution time: " << total.count() << std::endl;
        }, 
        [=](auto) {
            spin_and_print(2);
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> total = duration_cast<duration<double>>(t2 - t1);
            std::cout << "total execution time: " << total.count() << std::endl;
        }
    );
}

int main(int argc, char **argv) {

    SystematicTestHarness harness(argc, argv);
  
    harness.run(test_body);

    return 0;
}
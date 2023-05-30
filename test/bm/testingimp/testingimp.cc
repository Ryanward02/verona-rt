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

using namespace verona::cpp;
using namespace verona::rt;
auto test_body() {
    cown_ptr<cown_obj> c1 = make_cown<cown_obj>();
    cown_ptr<cown_obj> c2 = make_cown<cown_obj>();
    cown_ptr<cown_obj> c3 = make_cown<cown_obj>();

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    int c = 0;
    for (int i = 0; i < 1000000000; i++) {
        c++;
    }

    high_resolution_clock::time_point t2_single = high_resolution_clock::now();
    duration<double> total_single = duration_cast<duration<double>>(t2_single - t1);
    

    std::cout << "time for single block: " << total_single.count() << std::endl;

    t1 = high_resolution_clock::now();

    // WARNING: cown_list_1, cown_list_2 must not be empty.
    for (int i = 0; i < 10; i++) {
        when2(cowns<cown_ptr<cown_obj>>(c1), cowns<cown_ptr<cown_obj>>(c2, c3), 
            [=](auto) {
                int c = 0;
                for (int i = 0; i < 1000000000; i++) {
                    c++;
                }

                high_resolution_clock::time_point t2 = high_resolution_clock::now();
                duration<double> total = duration_cast<duration<double>>(t2 - t1);
                std::cout << "time for parallel block 1: " << total.count() << std::endl;
            }, 
            [=](auto) {
                int c = 0;
                for (int i = 0; i < 1000000000; i++) {
                    c++;
                }

                high_resolution_clock::time_point t2 = high_resolution_clock::now();
                duration<double> total = duration_cast<duration<double>>(t2 - t1);
                std::cout << "time for parallel block 2: " << total.count() << std::endl;
            }
        );
    }
}
// IT WORKS -> IT'S DEFINITELY PARALLEL YAY
/**
 * time for single block: 0.946114
 * cown
 * time for parallel block 2: 0.959729
 * cown
 * time for parallel block 1: 0.960236
 * cown
*/

int main(int argc, char **argv) {

    SystematicTestHarness harness(argc, argv);
  
    harness.run(test_body);

    return 0;
}
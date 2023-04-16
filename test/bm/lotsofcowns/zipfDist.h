#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <random>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

class cown
{
private:
  int i = 0;
  // destructor. Not sure when this is called.
public:
  cown(int i) {
    this->i = i;
  }
  ~cown()
  {
    
    std::cout << "I'm DEAD : " << std::to_string(i) << std::endl;
  }
};

class Generator
{
public:
    
    void setLastValue(long lastValue) {};
};

class ZipfianGenerator : public Generator
{
public:
    int items;
    int base;
    double zipfConstant;
    double alpha, zetan, eta, theta, zeta2Theta;
    int countForZeta;
    bool allowItemDecrease = false;
    constexpr static double ZIPF_CONST = 0.99;
    int lastValue;

    void setLastValue(int lastValue) {
        this->lastValue = lastValue;
    };

    ZipfianGenerator(int items) : ZipfianGenerator(0, items - 1) {}

    ZipfianGenerator(int min, int max) : ZipfianGenerator(min, max, ZIPF_CONST) {}

    ZipfianGenerator(int items, double zipfConstant) : ZipfianGenerator(0, items - 1, zipfConstant) {}

    ZipfianGenerator(int min, int max, double zipfConstant) : ZipfianGenerator(min, max, zipfConstant, zetaStatic(max - min + 1, zipfConstant)) {}

    ZipfianGenerator(int min, int max, double zipfConstant, double zetan) {
        items = max - min + 1;
        base = min;
        this->zipfConstant = zipfConstant;
        theta = zipfConstant;
        zeta2Theta = zeta(2, theta);
        alpha = 1 / (1 - theta);
        this->zetan = zetan;
        countForZeta = items;

        eta = (double)(1.0 - pow(2.0 / items, 1.0 - theta)) / (1.0 - zeta2Theta / zetan);
        
        nextValue();

    };

    double zeta(int n, double thetaVal) {
        countForZeta = n;
        return zetaStatic(n, thetaVal);
    };

     double zeta(int st, int n, double theta, double initSum) {
        countForZeta = n;
        return zetaStatic(st, n, theta, initSum);
    };

    static double zetaStatic(int n, double theta) {
        return zetaStatic(0, n, theta, 0);
    };

    static double zetaStatic(int st, int n, double theta, double initSum) {
        double sum = initSum;

        for (int i = st; i < n; i++) {
            sum += 1 / pow(i + 1, theta);
        }

        return sum;
    };

    
    int nextInt(int itemCount) {
        if (itemCount != countForZeta) {
            if (itemCount > countForZeta) {
                zetan = zeta(countForZeta, items, theta, zetan);
                eta = (1 - pow(2 / items, 1 - theta)) / (1 - zeta2Theta / zetan);
            } else {
                if ((itemCount < countForZeta) && (allowItemDecrease)) {
                    std::cout << "WARNING: RECOMPUTING ZIPF. SLOW..." << std::endl;

                    zetan = zeta(itemCount, theta);
                    eta = (1 - pow(2 / items, 1 - theta)) / (1 - zeta2Theta / zetan);
                }
            }
        }

        double u = (double)((std::rand() % 1000) / 1000.0);
        // std::cout << std::to_string(u) << std::endl;

        double uz = u * zetan;


        // if (uz < 1.0) {
        //     return base;
        // }

        // if (uz < 1.0 + pow(0.5, this->theta)) {
        //     return base + 1;
        // }
        
        int ret = base + (int)(itemCount * pow(eta * u - eta + 1, alpha));
        setLastValue(ret);
        return ret;
    };

    int nextValue() {
        return nextInt(items);
    };
};

// Create a scrambled distribution to avoid the head of the cown_ptr list being a cluster of the most popular.
class ScrambledZipfianGenerator : public Generator
{
public:
    ZipfianGenerator gen = ZipfianGenerator(0);
    int min, max, itemcount;

    constexpr static double ZETAN = 26.46902820178302;
    constexpr static double USED_Z_CONST = 0.99;
    constexpr static int ITEM_COUNT = 1000000000;
    int lastValue;

    static unsigned int FNVHash(std::string str) {
        const unsigned int fnv_prime = 0x811C9DC5;
        unsigned int hash = 0;
        unsigned int i = 0;
        unsigned int len = str.length();

        for (i = 0; i < len; i++)
        {
            hash *= fnv_prime;
            hash ^= (str[i]);
        }
	    return hash;
    }

    void setLastValue(int lastValue) {
        this->lastValue = lastValue;
    }
    
    ScrambledZipfianGenerator(int items) : ScrambledZipfianGenerator(0, items - 1) {}

    ScrambledZipfianGenerator(int min, int max) : ScrambledZipfianGenerator(min, max, ZipfianGenerator::ZIPF_CONST) {}

    ScrambledZipfianGenerator(int min, int max, double zipfianconstant) {
        this->min = min;
        this->max = max;
        itemcount = max - min + 1;
        if (zipfianconstant == USED_Z_CONST) {
            gen = ZipfianGenerator(0, itemcount, zipfianconstant, ZETAN);
        } else {
            gen = ZipfianGenerator(0, itemcount, zipfianconstant);
        }
    }

    int nextValue() {
        int nv = gen.nextValue();
        int ret = (int)(min + FNVHash(std::to_string(nv)) % itemcount);
        setLastValue(ret);
        return ret;
    }

    int mean() {
        return (min + max) / 2;
    }


};

int cmpDouble(const void *a,const void *b) {
  double *x = (double *) a;
  double *y = (double *) b;
  if (*x < *y)
   return -1;
  else {
    if (*x > *y) 
      return 1;
    else
      return 0;
  }
    
}

int cmpInt(const void *a,const void *b) {
  int *x = (int *) a;
  int *y = (int *) b;
  if (*x < *y)
   return -1;
  else {
    if (*x > *y) 
      return 1;
    else
      return 0;
  }
}


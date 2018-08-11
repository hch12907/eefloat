#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sys/time.h>
#include <vector>

#include "ps2float.hpp"

using namespace std;

static unsigned long long get_timestamp ()
{
    struct timeval now;
    gettimeofday (&now, NULL);
    return  now.tv_usec + (unsigned long long)now.tv_sec * 1000000;
}

int main(int argc, char** argv) 
{
    // INF test
    // - in IEEE the result is supposed to be INF
    // - in PS2 the result is supposed to be MAX divided by 2^63 (could be wrong)
    {
        Float test1(1.0f/0);
        Float test2((float)(1ULL << 63));
        Float test_result = test1 / test2;
        cout << test_result.get_ieee() << endl;
    }


    // Divided-By-Zero test
    // - in IEEE the result is supposed to be INF
    // - in PS2 the result is supposed to be the MAX(or INF in IEEE)
    {
        Float test1(1.0f);
        Float test2(0.0f);
        Float test_result = test1 / test2;
        cout << test_result.get_ieee() << endl;
    }


    // Normal test - A test where the float is not promoted to a double
    // - in both IEEE and PS2 the result is supposed to be 100.125
    {
        Float a(100.0f);
        Float b(0.125f);
        Float c = a + b;
        cout << c.get_ieee() << endl;
    }


    const int TEST_TIME = 100000000;
    std::srand(std::time(0));


    // Variable preparation
    std::vector<float> input1; input1.reserve(TEST_TIME*2);
    std::vector<Float> input2; input2.reserve(TEST_TIME*2);
    std::vector<float> result1; result1.reserve(TEST_TIME);
    std::vector<Float> result2; result2.reserve(TEST_TIME);


    // Input
    for (int i = 0; i < TEST_TIME * 2; i += 2) {
        int a = std::rand(), b = std::rand();
        input1[i] = (float)a + 0.1234f;
        input1[i+1] = (float)b + 0.2345f;
        input2[i] = Float((float)a + 0.1234f);
        input2[i+1] = Float((float)b + 0.2345f);
    }


    // Test 1
    unsigned long long start1 = get_timestamp();
    for (int i = 0, j = 0; i < TEST_TIME * 2; i += 2, j += 1) {
        result1.push_back(input1[i] * input1[i+1]);
    }
    unsigned long long end1 = get_timestamp();


    // Test 2
    unsigned long long start2 = get_timestamp();
    for (int i = 0, j = 0; i < TEST_TIME * 2; i += 2, j += 1) {
        result2.push_back(input2[i] * input2[i+1]);
    }
    unsigned long long end2 = get_timestamp();


    // Results
    cout << "Result: tested " << result1.size() << " cases for Test 1, time taken: " << end1 - start1 << " microsecs" << endl;
    cout << "Result: tested " << result2.size() << " cases for Test 2, time taken: " << end2 - start2 << " microsecs" << endl;
    
    return 0;
}
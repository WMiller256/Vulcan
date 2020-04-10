/*
 * William Miller
 * Nov 8, 2018
 *
 * Definitions for 'globally' available functions
 *
 */

#include "global.h"

double magnitude(vec v) {
	double ret = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	printrln(in("","magnitude")+"                 Vector "+v.info(2)+" magnitude is ", scientific(ret), 6);
	return ret;
}

double fetch_add(std::atomic<double>* shared, double h) {
    double expected = shared -> load();
    while (!atomic_compare_exchange_weak(shared, &expected, expected + h))
      ;
    return expected;
}


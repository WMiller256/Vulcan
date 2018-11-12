/*
 * rpos.c++
 *
 * William Miller
 * Nov 11, 2018
 *
 * Implementation for a (vec) subclass that also includes the an index
 * {idx} for the body to which the position corresponds. Named rpos in
 * reference to it's use as the  position returned by manual threading
 * to the parent thread, i.e. the result position. Designed to be very
 * lightweight because the  speed of  simulation is  very sensitive to
 * the piping speed, which is very sensitive to the size of the object
 * being piped. 
 *
 */

#include "rpos.h"

rpos::rpos() : vec() {

}

rpos::rpos(vec v, int index) : vec(v) {
	idx = index;
}

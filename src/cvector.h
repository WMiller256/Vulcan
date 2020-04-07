/* cvector.h
 *
 * William Miller
 * Mar 26, 2020
 *
 * Container derived from std::vector with modified indexing
 * behavior determined by the length of the vector
 *
 */

#ifndef CVECTOR_H
#define CVECTOR_H

#include <vector>

template<typename T> class CVector : public std::vector<T> {
public:
	static bool singular;
	T operator [] (const int idx) {
		singular = this->size() == 1 ? true : false;
		if (singular) return this->at(0);
		else return this->at(idx);
	}
};

#endif // CVECTOR_H

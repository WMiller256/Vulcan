/* matrix.c++
 *
 * William Miller
 * Apr 9, 2020
 *
 * Matrix class backed by one dimensional std::valarray
 * with multiple indexing behaving like multi-dimensional 
 * array
 */

#include <iostream>
#include "matrix.h"

int main() {
	Matrix<int> mat(3, 3);
	int n = 0;
	for (int r = 0; r < 3; r ++) {
		for (int c = 0; c < 3; c ++) {
			mat(r, c) = ++n;
			std::cout << mat(r, c) << std::endl;
		}
	}
}

/* matrix.h
 *
 * William Miller
 * Apr 9, 2020
 *
 * Basic matrix template backed by one dimensional std::valarray
 * with multiple indexing behaving like multi-dimensional 
 * array
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <valarray>

template <typename T>
class Matrix {
public:
	Matrix() {}
	Matrix(int r, int c) : mat(r * c), dim(c) {}
	T& operator()(int r, int c) { return mat[r * dim + c]; }
private:
	std::valarray<T> mat;
	int dim;
};

#endif // MATRIX_H

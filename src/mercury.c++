/* mercury.c++
 *
 * William Miller
 * Apr 15, 2020
 *
 * Implementation of class derived from Integrator
 * base class. Intended to be a verbatim, or at least
 * near-verbatim port of the original Mercury
 * hybrid symplectic integrator
 *
 */

#include "mercury.h"

const int Mercury::nsteps = 12;
const int Mercury::steps[] = {2, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128};
const double Mercury::grow = 1.3;
const double Mercury::shrink = 0.55;

Mercury::Mercury(double &h) : Integrator() {
	this->h = h;
	init();
}

void Mercury::init() const {
	tolerance = 1e-10;
	error = std::valarray<double>(0.0, nreal);
	s = std::valarray<int>(0, nreal);
	rscale = std::valarray<double>(0.0, nreal);
	vscale = std::valarray<double>(0.0, nreal);
	hc = std::valarray<double>(nreal);
	hs = Matrix<double>(nreal, nsteps);
	r = std::valarray<Pos>(nreal);
	v = std::valarray<Vel>(nreal);
	rn = std::valarray<Pos>(nreal);
	vn = std::valarray<Vel>(nreal);
	dr = Matrix<Pos>(nreal, nsteps);
	dv = Matrix<Vel>(nreal, nsteps);
}

vec Mercury::accleration(Pos &r, int &idx) const {
	vec a(0, 0, 0);
	for (int ii = 0; ii < nbodies; ii ++) {
		if (ii != idx) {
			// G M / r^2 r-hat
			a += r.direction(read[ii]->r) * (G * read[ii]->m) / read[ii]->squareDistance(r);
		}
	}
	return a;
}

void Mercury::bulirschStoer(CBody* b, CBody* w) {
	w->ncalcs++;
	int ii = b->idx;
	int fmax = 1;
	rscale[ii] = b->r.squared() > 0.0 ? 1.0 / b->r.squared() : 0.0;
	vscale[ii] = b->v.squared() > 0.0 ? 1.0 / b->v.squared() : 0.0;
	vec a(0, 0, 0);
	while (true) {
		for (int f = 1; f < fmax; f++) {
			for (int e = 0; e < nsteps; e ++) {
				int n = steps[e];
				vec a = acceleration(b->r, ii);
				hc[ii] = h / (2.0 * float(n * fmax));
				hs(ii, n-1) = pow(hc[ii], 2);
				h2[ii] = hc[ii] * 2.0;

				// Performed modified midpoint integration with {n} * 2 substeps;
				r[ii] = b->r + hc[ii] * b->v;
				v[ii] = b->v + hc[ii] * a;
				a = acceleration(r[ii], ii);
				rn[ii] = r[ii] + h2[ii] * v[ii];
				vn[ii] = v[ii] + h2[ii] * a;
				for (int j = 1; j < n; j ++) {
					a = acceleration(rn[ii], ii);
					r[ii] = r[ii] + h2[ii] * vn[ii];
					v[ii] = v[ii] + h2[ii] * a;
					a = acceleration(r[ii], ii);
					rn[ii] = rn[ii] + h2[ii] * v[ii];
					vn[ii] = vn[ii] + h2[ii] * a;
				}
				a = acceleration(rn[ii], ii);

				// Update the delta matrices (used for polynomial extrapolation)
				dr(ii, n-1) = 0.5 * (rn[ii] * r[ii] + hc[ii] * vn[ii]);
				dv(ii, n-1) = 0.5 * (vn[ii] + v[ii] + hc[ii] * a);
				// Perform polynomial extrapolation
				for (int jj = n - 2; jj >= 0; jj --) {
					dr(ii, jj) = (1.0 / (hs(ii, jj) - hs(ii, n-1))) * hs(ii, jj+1) - (1.0 / (hs(ii, jj) - hs(ii, n-1))) * hs(ii, n-1) * dr(ii, jj);
					dv(ii, jj) = (1.0 / (hs(ii, jj) - hs(ii, n-1))) * hs(ii, jj+1) - (1.0 / (hs(ii, jj) - hs(ii, n-1))) * hs(ii, n-1) * dv(ii, jj);
				}

				// After several integrations, check the relative error for 
				// satisfaction of completion condition
				if (n > 3) {
					w->r = dr(ii, 0);
					w->v = dv(ii, 0);
					for (int jj = 1; jj < n; jj ++) {
						w->r += dr(ii, jj);
						w->v += dv(ii, jj);
					}
					// Store the number of substeps which was actually used
					s[ii] = n;
					if (f == fmax - 1) return;
					else break;
				}
			}
			std::swap(b, w);
		}
		fmax *= 2;
	}
}


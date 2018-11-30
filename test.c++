#include <thread>
#include <iostream>
#include <atomic>

std::atomic<int> simTime;
std::atomic<int> tocalc;
int end = 10000;

void threadFunction(int n);

int main() {
	int nthreads = 4;
	simTime = 0;
	tocalc = 0;
	std::thread threads[nthreads];
	for (int ii = 0; ii < nthreads; ii ++) {
		threads[ii] = std::thread(threadFunction, ii+1);
	}

	int wait = 0;
	while (simTime < end) {
		tocalc = nthreads;
		simTime += 1;
		// do calculation
		while (tocalc > 0) {
			// wait until all the threads have done their calculation
			// or at least checked to see if they need to
		}
	}
	for (int ii = 0; ii < nthreads; ii ++) {
		threads[ii].join();
	}
}

void threadFunction(int n) {
	int prev = 0;
	int fix = prev;
	int ncalcs = 0;
	while (simTime < end) {
		if (simTime - prev > 0) {
			prev = simTime;
			if (simTime - fix >= n) {
				// do calculation
				ncalcs ++;
				fix = simTime;
			}
			tocalc --;
		}
	}
	std::cout << std::to_string(n)+" {ncalcs} - "+std::to_string(ncalcs)+"\n";
}

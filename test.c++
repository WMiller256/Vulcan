#include <thread>
#include <iostream>
#include <atomic>

std::atomic<int> simTime;
std::atomic<int> tocalc;
int end = 10000;
int nthreads = 4;
int** one = new int*[nthreads];
int** two = new int*[nthreads];
int** read = NULL;
int** write = NULL;

void threadFunction(int n, int ii);

int main() {
	for (int ii = 0; ii < nthreads; ii ++) {
		one[ii] = new int(ii);
		two[ii] = new int(ii);
	}
	read = one;
	write = two;
    simTime = 0;
    tocalc = 0;
    std::thread threads[nthreads];
    for (int ii = 0; ii < nthreads; ii ++) {
        threads[ii] = std::thread(threadFunction, 1, ii);
    	std::cout << "Thread " << ii << " initialized\n";
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
		if (read == one) {
			read = two;
			write = one;
		}
		else {
			read = one;
			write = two;
		}
    }
    for (int ii = 0; ii < nthreads; ii ++) {
        threads[ii].join();
    }
}

void threadFunction(int n, int ii) {
    int prev = 0;
    int fix = prev;
    int ncalcs = 0;
	while (simTime == 0) {}
    while (true) {
        if (simTime - prev > 0) {
            prev = simTime;
            if (simTime - fix >= n) {
				write[ii] = read[ii];
                // do calculation
                ncalcs ++;
                fix = simTime;
            }
            tocalc --;
            if (simTime == end) {
				break;
            }
        }
    }
    std::cout << std::to_string(ii)+" {ncalcs} - "+std::to_string(ncalcs)+"\n";
}

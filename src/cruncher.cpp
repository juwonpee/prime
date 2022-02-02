#include "cruncher.h"

using namespace std;

cruncher::cruncher(writeQueue_t* _writeQueue, currentNumber_t* _currentNumber, int crunchType) {
	if (crunchType == 0) {
		while (1) {
			writeQueue = _writeQueue;
			currentNumber = _currentNumber;

			// Get currentNumber
			currentNumber->lock.lock();
			number = currentNumber->currentNumber;
			currentNumber->currentNumber++;
			currentNumber->lock.unlock();

			// Time to crunch
			if (crunchNormal()) {	// If prime number
				// Add to writeQueue
				writeQueue->lock.lock();
				writeQueue->number.push(number);
				writeQueue->lock.unlock();
			}
		}
	}
	else if (crunchType == 1) {
		while (1) {
			// Get currentNumber
			currentNumber->lock.lock();
			number = currentNumber->currentNumber;
			currentNumber->currentNumber++;
			currentNumber->lock.unlock();

			// Time to crunch
			if (crunchAVX()) {	// If prime number
				// Add to writeQueue
				writeQueue->lock.lock();
				writeQueue->number.push(number);
				writeQueue->lock.unlock();
			}
		}
	}

}

cruncher::~cruncher() {
	currentNumber->lock.unlock();
}

bool cruncher::crunchNormal() {
	for (uint64_t divisor = 2; divisor < number - 1; divisor++) {
		if (number % divisor == 0) return false;
	}
	return true;
}

bool cruncher::crunchAVX() {
	return false;
}


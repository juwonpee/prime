#include "cruncher.h"

using namespace std;

cruncher::cruncher(writeQueue_t* _writeQueue, currentNumber_t* _currentNumber, runType_t crunchType) {
	// Make sure (float) is 32 bit

	writeQueue = _writeQueue;
	currentNumber = _currentNumber;
	while(1) {
		
		// Get currentNumber
		currentNumber->lock.lock();
		number = currentNumber->currentNumber;
		if (number == 0) {
			return;
		}
		currentNumber->currentNumber++;
		currentNumber->lock.unlock();

		
		if (crunchType == normal) {
			// Time to crunch
			if (crunchNormal()) {	// If prime number
				// Add to writeQueue
				writeQueue->lock.lock();
				writeQueue->number.push(number);
				writeQueue->lock.unlock();
			}
		
		}
		else if (crunchType == avx) {
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
	writeQueue->lock.unlock();
}

bool cruncher::crunchNormal() {
	for (uint64_t divisor = 2; divisor < number / 2 + 1; divisor++) {
		if (number % divisor == 0) return false;
	}
	return true;
}

bool cruncher::crunchAVX() {
	if (number <= 50) { //  Random number to avoid my avx implementation bugs
		return crunchNormal();
	}
	else if (number <= 8388608 - 1) { // Largest number of 32 bit float
		float divided[8];
		__m256 dividendAVX = _mm256_set1_ps((float)number);
		for (uint32_t divisor = 2; divisor + 8 < number / 2 + 1; divisor += 8) {
			__m256 divisorAVX = _mm256_set_ps((float)divisor, (float)divisor + 1, (float)divisor + 2, (float)divisor + 3, (float)divisor + 4, (float)divisor + 5, (float)divisor + 6, (float)divisor + 7);
			__m256 dividedAVX = _mm256_div_ps(dividendAVX, divisorAVX);
			_mm256_storeu_ps(divided, dividedAVX);
			for (uint8_t i = 0; i < 8; i++) {
				if (floorf(divided[i]) == divided[i]) return false;
			}
		}
	}
	else if (number <= 9007199254740992 - 1){ // Largest number of 64 bit float
		double divided[4];
		__m256d dividendAVX = _mm256_set1_pd((double)number);
		for (uint64_t divisor = 2; divisor + 4 < number / 2 + 1; divisor+=4) {
			__m256d divisorAVX = _mm256_set_pd((double)divisor, (double)divisor + 1, (double)divisor + 2, (double)divisor + 3);
			__m256d dividedAVX = _mm256_div_pd(dividendAVX, divisorAVX);
			_mm256_storeu_pd(divided, dividedAVX);
			for (uint8_t i = 0; i < 4; i++) {
				if (floorf(divided[i]) == divided[i]) return false;
			}
		}
	}
	else {
		return crunchNormal();
	}
	return true;
}


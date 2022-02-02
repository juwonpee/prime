#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <stdlib.h>
#include <chrono>
#include <stdint.h>
#include <csignal>
#include <vector>

#include "cruncher.h"

using namespace std;

currentNumber_t currentNumber;
writeQueue_t writeQueue;
vector<thread*> threadGroup;
cruncher* crunch;

void signalHandler(int sigNum) {
	cout << sigNum << " Signal Recieved" << endl;
	crunch->~cruncher();
	exit(sigNum);
}

void cruncherThread(int type) {
	// signal(SIGINT, signalHandler);
	crunch = new cruncher(&writeQueue, &currentNumber, type);
}

int main(void) {

	cout << "Prime number calculator" << endl;
	cout << "Just a practice projects for performance optimizations" << endl;

	int processor_count = thread::hardware_concurrency();
	cout << "Creating " << processor_count << " processing threads" << endl;

	// Start by initializing thread safe objects
	currentNumber.lock.lock();
	currentNumber.currentNumber = 3;
	currentNumber.lock.unlock();

	writeQueue.lock.lock();
	writeQueue.number.push(1);
	writeQueue.number.push(2);
	writeQueue.lock.unlock();

	// Start measuring time
	auto startTime = chrono::steady_clock::now();

	//---------------------------------NORMAL INTEGER OPERATIONS---------------------------------//
	// Start threads
	threadGroup.reserve(processor_count);
	for (int i = 0; i < processor_count; i++) {
		cout << "Starting thread: " << i << endl;
		threadGroup[i] = new thread(cruncherThread, 0);
	}

	// Write to file
	uint64_t count = 0;

	ofstream output;
	output.open ("prime numbers.txt", ios::out | ios::trunc);
	while (1) {
		queue<uint64_t> localWriteQueue;
		// Get writeQueue lock or if fail, try again
		if (writeQueue.lock.try_lock()) {
			// Empty glocal queue to local queue
			while (!writeQueue.number.empty()) {
				localWriteQueue.push(writeQueue.number.front());
				writeQueue.number.pop();
			}
			writeQueue.lock.unlock();
		}

		// Write if local queue is filled
		while (!localWriteQueue.empty()) {
			string temp = to_string(localWriteQueue.front());

			// Output current integer length every interval
			cout << '\r' << "Number Count: " << to_string(count) << ", Prime number: " << temp;
			count++;
			output << temp << '\n';
			localWriteQueue.pop();

			// Performance monitoring for normal integer operations
			if (count == 0x8000) {
				auto duration = chrono::duration_cast<std::chrono::milliseconds>(chrono::steady_clock::now() - startTime).count();
				cout << endl << "Time Elapsed: " << duration << "ms" << endl;
				// Wait till objects are thread safe
				writeQueue.lock.lock();
				currentNumber.lock.lock();
				// close threads
				for (int i = 0; i < processor_count; i++) {
					cout << "Closing thread: " << processor_count << endl;
					threadGroup[processor_count]->~thread();	
				}
			}

		}
	}

	//---------------------------------AVX INTEGER OPERATIONS---------------------------------//
	for (int i = 0; i < processor_count; i++) {
		cout << "Starting thread: " << i << endl;
		threadGroup[i] = new thread(cruncherThread);
	}
	output.close();
	return 0;
}
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

void cruncherThread(runType_t runType) {
	// signal(SIGINT, signalHandler);
	crunch = new cruncher(&writeQueue, &currentNumber, runType);
	crunch->~cruncher();
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

	// Write to file
	uint64_t count = 0;

	ofstream output;
	output.open ("prime numbers.txt", ios::out | ios::trunc);


	// Start measuring time
	auto startTime = chrono::steady_clock::now();
	threadGroup.reserve(processor_count);

	// //---------------------------------NORMAL INTEGER OPERATIONS---------------------------------//
	// // Start threads
	// for (int i = 0; i < processor_count; i++) {
	// 	cout << "Starting normal crunch thread: " << i << endl;
	// 	threadGroup[i] = new thread(cruncherThread, normal);
	// }
	// // Normal crunch start
	// while (1) {
	// 	queue<uint64_t> localWriteQueue;
	// 	// Get writeQueue lock or if fail, try again
	// 	if (writeQueue.lock.try_lock()) {
	// 		// Empty glocal queue to local queue
	// 		while (!writeQueue.number.empty()) {
	// 			localWriteQueue.push(writeQueue.number.front());
	// 			writeQueue.number.pop();
	// 		}
	// 		writeQueue.lock.unlock();
	// 	}

	// 	// Write if local queue is filled
	// 	while (!localWriteQueue.empty()) {
	// 		string temp = to_string(localWriteQueue.front());

	// 		// Output current integer length every interval
	// 		cout << '\r' << "Number Count: " << to_string(count) << ", Prime number: " << temp;
	// 		count++;
	// 		//output << temp << '\n';
	// 		localWriteQueue.pop();

	// 		// Performance monitoring for normal integer operations
	// 		if (count == 0x4FFFF) {
	// 			auto duration = chrono::duration_cast<std::chrono::milliseconds>(chrono::steady_clock::now() - startTime).count();
	// 			cout << endl << "Normal Crunch Time Elapsed: " << duration << "ms" << endl;

	// 			// close threads
	// 			// First by sending the signal to stop (0)
	// 			currentNumber.lock.lock();
	// 			currentNumber.currentNumber = 0;
	// 			currentNumber.lock.unlock();
	// 			for (int i = 0; i < processor_count; i++) {
	// 				cout << "Closing thread: " << i << endl;
	// 				threadGroup[i]->join();
	// 			}

	// 			// Wait till objects are thread safe and empty queue;
	// 			writeQueue.lock.lock();
	// 			queue<uint64_t> empty;
	// 			swap(writeQueue.number, empty);
	// 			writeQueue.lock.unlock();

	// 			currentNumber.lock.lock();
	// 			currentNumber.currentNumber = 3;
	// 			currentNumber.lock.unlock();
				
	// 			break;
	// 		}
	// 	}
	// 	// Check if normal execution stopped
	// 	if (!threadGroup[0]->joinable()) {
	// 		break;
	// 	}
	// }

	// //---------------------------------AVX INTEGER OPERATIONS---------------------------------//


	for (int i = 0; i < processor_count; i++) {
		cout << "Starting avx crunch thread: " << i << endl;
		threadGroup[i] = new thread(cruncherThread, avx);
	}

	// AVX crunch start
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
			//output << temp << '\n';
			localWriteQueue.pop();

			// Performance monitoring for normal integer operations
			if (count == 0xFFFFFF0) {
				auto duration = chrono::duration_cast<std::chrono::milliseconds>(chrono::steady_clock::now() - startTime).count();
				cout << endl << "Normal Crunch Time Elapsed: " << duration << "ms" << endl;

				// close threads
				// First by sending the signal to stop (0)
				currentNumber.lock.lock();
				currentNumber.currentNumber = 0;
				currentNumber.lock.unlock();
				for (int i = 0; i < processor_count; i++) {
					cout << "Closing thread: " << i << endl;
					threadGroup[i]->join();
				}

				// Wait till objects are thread safe and empty queue;
				writeQueue.lock.lock();
				queue<uint64_t> empty;
				swap(writeQueue.number, empty);
				writeQueue.lock.unlock();

				currentNumber.lock.lock();
				currentNumber.currentNumber = 3;
				currentNumber.lock.unlock();
				
				break;
			}
		}
		// Check if normal execution stopped
		if (!threadGroup[0]->joinable()) {
			break;
		}
	}

	output.close();
	return 0;
}
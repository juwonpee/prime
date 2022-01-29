#include <iostream>
#include <thread>


using namespace std;

int main(void) {
	cout << "Prime number calculator" << endl;
	cout << "Just a practice projects for performance optimizations" << endl;

	int processor_count = thread::hardware_concurrency();
	cout << "Creating " << processor_count << " processing threads" << endl;
	

	return 0;
}
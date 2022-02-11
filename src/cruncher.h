#include <queue>
#include <mutex>
#include <math.h>
#include <immintrin.h>


using namespace std;  

typedef struct writeQueue_t {
    mutex lock;
    queue<uint64_t> number;
} writeQueue_t;

typedef struct currentNumber_t {
    mutex lock;
    uint64_t currentNumber;
} currentNumber_t;

typedef enum runType_t {
    normal,
    avx,
} runType_t;


class cruncher {
    public:
        cruncher(writeQueue_t* _writeQueue, currentNumber_t* _currentNumber, runType_t crunchType);
        ~cruncher();

    private:
        writeQueue_t* writeQueue;
        currentNumber_t* currentNumber;
        uint64_t number;

        // Normal number crunching using %
        bool crunchNormal();

        // Number crunching using AVX intrinsics
        bool crunchAVX();
};
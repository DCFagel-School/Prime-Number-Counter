#include <iostream>
#include <sstream>

#include <chrono>
#include <thread>
#include <mutex>

using namespace std;

bool getArgs(int, char *[]);
void isPrime(int, int);
int  getSqrt(int);

// ---------------------------------------------------
// Global variables
//

const int MIN_THREADS = 1;
const int MAX_THREADS = 8192;

int primes = 0;     // Prime # count
int limit;          // Number to go to
int thds;           // Thread count

mutex sumMutex;

// ---------------------------------------------------
// MAIN

int main(int argc, char *argv[]) {
    thread *thdList;
    int thdWork;        // How much each thread works on
    int strtWork = 0;   // Where thread starts

    if(!getArgs(argc, argv)) {
        return 0;
    }

    thdList = new thread[thds];
    thdWork = (limit/thds);

    for(int i = 0; i < thds; i++) {
        thdList[i] = thread(isPrime, strtWork, (strtWork + thdWork));
        strtWork += thdWork;
    }

    auto t1 = chrono::high_resolution_clock::now();

    for(int i = 0; i < thds; i++) {
        thdList[i].join();
    }

    auto t2 = chrono::high_resolution_clock::now();

    cout << "Program took: " << 
        std::chrono::duration_cast<std::chrono::milliseconds>
            (t2 - t1).count() << " milliseconds" << endl;

    return 0;
}

// ---------------------------------------------------
// Threaded function
// Check if prime

void isPrime(int start, int end)
{
    bool p;
    int curr = start;

    while(start != end) {
        p = true;

        if(curr % 2 != 0) {
            for(int i = 3; i <= getSqrt(curr); i += 2) {
                if(curr % i == 0) {
                    p = false;
                    break;
                }
            }
        } else {
            p = false;
        }

        start++;
        curr++;

        lock_guard<std::mutex> lock1(sumMutex);
        if(p) {
            primes++;
        }
    }
}

// ---------------------------------------------------
// Returns estimated ceil sqrt of a number

int getSqrt(int x) 
{
    double sqrt = 0.0;
    double prev = 0.0;

    sqrt = static_cast<double>(x);

    do{
        prev = sqrt;

        sqrt = ((x/sqrt) + sqrt)/2;
    }while(sqrt != prev);

    sqrt++;

    return sqrt;
}

// ---------------------------------------------------
// Command line arguments stuff
// Check if valid format
// Get thread count and limit
//
// Format: ./prime -t <threadCount> -l <limit>

bool getArgs(int argc, char *argv[]) {
    stringstream ss;

    if(argc == 1) {
        cout << "Usage: ./prime -t <threadCount> -l <limit>";
        return false;
    }

    if (argc != 5 || string(argv[1]) != "-t") {
		cout << "Error, command line arguments invalid." << endl;
		return	false;
	}

    if (string(argv[2]) != "") {
		ss << argv[2];
		ss >> thds;
	}

    if (string(argv[4]) != "") {
        ss.str("");
        ss.clear();

		ss << argv[4];
		ss >> limit;
	}

    if (thds < MIN_THREADS || thds > MAX_THREADS) {
		cout << "Error, invalid thread count." << endl;
		return	false;
	}

    return true;
}
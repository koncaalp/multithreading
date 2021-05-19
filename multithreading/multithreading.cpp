#include <iostream>
#include <thread>
#include "HW7DynIntQueue.h"
#include <chrono>
#include <random>
#include <time.h>
#include <ctime>
#include <iomanip> 
#include <mutex>
using namespace std;
HW7DynIntQueue filling;
HW7DynIntQueue packing;
mutex queue;
mutex output;

int ID = 1;

int random_range(const int& min, const int& max)
{
    static mt19937 generator(time(0));
    uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

void produce(int min, int max)
{
    int duration = random_range(min, max);
    this_thread::sleep_for(chrono::seconds(duration));
    queue.lock();
    filling.enqueue(ID);
    queue.unlock();
    time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
    struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
    localtime_s(ptm, &tt);  //converting the time structures
    output.lock();
    cout << "Producer has enqueued a new box " << ID << " to filling queue (filling queue size is " << filling.getCurrentSize() << "): " << put_time(ptm, "%X") <<endl;
}

int totalitem;
int main()
{
    time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
    struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
    localtime_s(ptm, &tt);  //converting the time structures
    //above two lines may not work for xcode. instead you can replace these two lines with    struct tm *ptm = localtime(&tt);
    cout << "Time is now " << put_time(ptm, "%X") << endl;  //displaying the time  
    int minpro, maxpro, minfil, maxfil, minpac, maxpac;
    produce(3, 3);
    cout << "Please enter the total number of items: ";
    cin >> totalitem;
    cout << "Please enter the min-max waiting time range of producer: " << endl;;
    cout << "Min: ";
    cin >> minpro;
    cout << "Max: ";
    cin >> maxpro;
    cout << "Please enter the min-max waiting time range of filler workers: " << endl;;
    cout << "Min: ";
    cin >> minfil;
    cout << "Max: ";
    cin >> maxfil;
    cout << "Please enter the min-max waiting time range of packager workers: " << endl;;
    cout << "Min: ";
    cin >> minpac;
    cout << "Max: ";
    cin >> maxpac;
    
}


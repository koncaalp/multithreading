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
int totalitem = 2, fcounter1 = 1, fcounter2 = 1, pcounter1 = 1, pcounter2 = 1;

int random_range(const int& min, const int& max)
{
    static mt19937 generator(time(0));
    uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

void produce(int min, int max)
{
    while (ID <= totalitem)
    {
        int duration = random_range(min, max);
        this_thread::sleep_for(chrono::seconds(duration));
        queue.lock();
        filling.enqueue(ID);
        ID++;
        queue.unlock();
        time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
        struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
        localtime_s(ptm, &tt);  //converting the time structures
        output.lock();
        cout << "Producer has enqueued a new box " << ID-1 << " to filling queue (filling queue size is " << filling.getCurrentSize() << "): " << put_time(ptm, "%X") << endl;
        output.unlock();
    }
}

void filler(int min, int max, int worker)
{
    int boxid;
    while (fcounter1 + fcounter2 <= totalitem+1)
    {
        if (!filling.isEmpty())
        {
            time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
            struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
            localtime_s(ptm, &tt);  //converting the time structures
            int duration = random_range(min, max);
            queue.lock();
            filling.dequeue(boxid);
            packing.enqueue(boxid);
            queue.unlock();
            if (worker == 1)
            {
                fcounter1++;
            }
            else
            {
                fcounter2++;
            }
            output.lock();
            cout << "Filler " << worker << " started filling the box " << boxid << " (filling queue size is " << filling.getCurrentSize() << "): " << put_time(ptm, "%X") << endl;
            output.unlock();
            this_thread::sleep_for(chrono::seconds(duration));
            ptm->tm_sec++;
            output.lock();
            cout << "Filler " << worker << " finished filling the box " << boxid  << ": " << put_time(ptm, "%X") << endl;
            output.unlock();
        }
    }
}


void packager(int min, int max, int worker)
{
    int boxid;
    while (pcounter1 + pcounter2 <= totalitem+1)
    {
        if (!packing.isEmpty())
        {
            time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
            struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
            localtime_s(ptm, &tt);  //converting the time structures
            int duration = random_range(min, max);
            queue.lock();
            packing.dequeue(boxid);
            queue.unlock();
            if (worker == 1)
            {
                pcounter1++;
            }
            else
            {
                pcounter2++;
            }
            output.lock();
            cout << "Packager " << worker << " started packaging the box " << boxid << " (packaging queue size is " << packing.getCurrentSize() << "): " << put_time(ptm, "%X") << endl;
            output.unlock();
            this_thread::sleep_for(chrono::seconds(duration));
            ptm->tm_sec++;
            output.lock();
            cout << "Packager " << worker << " finished packaging the box " << boxid << ": " << put_time(ptm, "%X") << endl;
            output.unlock();
        }
    }
}

int main()
{
    time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
    struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
    localtime_s(ptm, &tt);  //converting the time structures
    //above two lines may not work for xcode. instead you can replace these two lines with    struct tm *ptm = localtime(&tt);
    cout << "Time is now " << put_time(ptm, "%X") << endl;  //displaying the time  
    produce(1, 1);
    produce(1, 1);
    filler(1, 1, 1);
    //filler(1, 1, 2);
    packager(1, 1, 1);
    //packager(1, 1, 2);

    cout << packing.getCurrentSize();
    int minpro, maxpro, minfil, maxfil, minpac, maxpac;
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


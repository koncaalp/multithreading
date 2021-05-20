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
mutex fqueue;
mutex pqueue;
mutex output;

int ID = 1;
int totalitem = 6, fcounter1 = 1, fcounter2 = 1, pcounter1 = 1, pcounter2 = 1;

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
        fqueue.lock();
        filling.enqueue(ID);
        ID++;
        int fillingsize = filling.getCurrentSize();
        fqueue.unlock();
        time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
        struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
        localtime_s(ptm, &tt);  //converting the time structures
        output.lock();
        cout << "Producer has enqueued a new box " << ID-1 << " to filling queue (filling queue size is " << fillingsize << "): " << put_time(ptm, "%X") << endl;
        output.unlock();
    }
}

void filler(int min, int max, int worker)
{

    int boxid;
    
    while (fcounter1 + fcounter2 <= totalitem+1)
    {
        fqueue.lock();
        if (!filling.isEmpty() && fcounter1 + fcounter2 <= totalitem + 1)
        {
            filling.dequeue(boxid);
            int fillingsize = filling.getCurrentSize();
            time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
            struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
            localtime_s(ptm, &tt);  //converting the time structures
            output.lock();
            cout << "Filler " << worker << " started filling the box " << boxid << " (filling queue size is " << fillingsize << "): " << put_time(ptm, "%X") << endl;
            output.unlock();
            if (worker == 1)
            {
                fcounter1++;
            }
            else
            {
                fcounter2++;
            }
            fqueue.unlock();
            int duration = random_range(min, max);
            this_thread::sleep_for(chrono::seconds(duration));
            pqueue.lock();
            packing.enqueue(boxid);
            int packagingsize = packing.getCurrentSize();
            pqueue.unlock();
            tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
            localtime_s(ptm, &tt);  //converting the time structures
            output.lock();
            cout << "Filler " << worker << " finished filling the box " << boxid  << ": " << put_time(ptm, "%X") << endl;
            output.unlock();
            output.lock();
            cout << "Filler " << worker << " put box " << boxid << " into packaging queue (packaging queue size is " << packagingsize << " :"  << put_time(ptm, "%X") << endl;
            output.unlock();
        }
        else
        {
            fqueue.unlock();
        }
    }
}


void packager(int min, int max, int worker)
{
    int boxid;
    while (pcounter1 + pcounter2 <= totalitem+1)
    {
        pqueue.lock();
        if (!packing.isEmpty() && pcounter1 + pcounter2 <= totalitem + 1)
        {
            packing.dequeue(boxid);
            int packagingsize = packing.getCurrentSize();
            if (worker == 1)
            {
                pcounter1++;
            }
            else
            {
                pcounter2++;
            }
            pqueue.unlock();
            time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
            struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
            localtime_s(ptm, &tt);  //converting the time structures
            int duration = random_range(min, max);
            output.lock();
            cout << "Packager " << worker << " started packaging the box " << boxid << " (packaging queue size is " << packagingsize << "): " << put_time(ptm, "%X") << endl;
            output.unlock();
            this_thread::sleep_for(chrono::seconds(duration));
            tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
            localtime_s(ptm, &tt);  //converting the time structures 
            output.lock();
            cout << "Packager " << worker << " finished packaging the box " << boxid << ": " << put_time(ptm, "%X") << endl;
            output.unlock();
        }
        else
        {
            pqueue.unlock();
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
    thread t1(&produce, 1,1);
    thread t2(&filler,1,1, 1);
    thread t3(&filler,1,1, 2);
    thread t4(&packager,1,1, 1);
    thread t5(&packager,1,1, 2);


    /*cout << packing.getCurrentSize();
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
    */
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
    localtime_s(ptm, &tt);  //converting the time structures
    cout << "End of the simulation ends: " << put_time(ptm, "%X") << endl;
    return 0;
}


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

int ID = 1; //global variables
int totalitem, fcounter1 = 1, fcounter2 = 1, pcounter1 = 1, pcounter2 = 1;

int random_range(const int& min, const int& max) //thread safe random number generator
{
    static mt19937 generator(time(0));
    uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

void produce(int min, int max)
{
    while (ID <= totalitem) //if the produced boxes are less than demand, produce one
    {
        int duration = random_range(min, max);
        this_thread::sleep_for(chrono::seconds(duration)); //sleep for a random amount of time between given min and max values
        fqueue.lock(); //lock the mutex for filling queue
        filling.enqueue(ID); //enqueue
        ID++; //increment the ID
        int fillingsize = filling.getCurrentSize(); //get the size of queue now so it wont change uncontrollably
        fqueue.unlock(); //unlock the mutex
        //starting, code from the lecture examples
        time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
        struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
        localtime_s(ptm, &tt);  //converting the time structures
        //ending, code from the lecture examples
        output.lock(); //cout mutex lock and cout the message
        cout << "Producer has enqueued a new box " << ID-1 << " to filling queue (filling queue size is " << fillingsize << "): " << put_time(ptm, "%X") << endl;
        output.unlock(); //unlock
    }
}

void filler(int min, int max, int worker)
{

    int boxid;
    
    while (fcounter1 + fcounter2 <= totalitem+1) //until the total number of boxes filled is the total demand, fill
    {
        fqueue.lock(); //lock the mutex for filling
        if (!filling.isEmpty() && fcounter1 + fcounter2 <= totalitem + 1) //check if any other thread change the trueness value of that bool
        {
            filling.dequeue(boxid); //dequeu and get size as the producing one
            //starting, code from lecture examples
            int fillingsize = filling.getCurrentSize();
            time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
            struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
            localtime_s(ptm, &tt);  //converting the time structures
            //end of the sample code
            if (worker == 1) //increment the counters according to the worker number, before unlocking since we dont want any other thread to start the process before checking incremented values
            {
                fcounter1++;
            }
            else
            {
                fcounter2++;
            }
            fqueue.unlock();

            output.lock(); //cout mutex lock to print out the started filling message
            cout << "Filler " << worker << " started filling the box " << boxid << " (filling queue size is " << fillingsize << "): " << put_time(ptm, "%X") << endl;
            output.unlock(); //unlock cout mutex
            int duration = random_range(min, max); //get a random value between min and max and sleep for that much
            this_thread::sleep_for(chrono::seconds(duration));
            
            tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time for the second time
            localtime_s(ptm, &tt);  //converting the time structures
            output.lock(); //cout lock to print finished filling the box
            cout << "Filler " << worker << " finished filling the box " << boxid  << ": " << put_time(ptm, "%X") << endl;
            output.unlock(); //unlock

            pqueue.lock(); //lock the packaging queue to enqueue new box
            packing.enqueue(boxid); //enqueue and get the size after
            int packagingsize = packing.getCurrentSize();
            pqueue.unlock(); //unlock the mutex
            output.lock(); //lock cout mutex and print the message
            cout << "Filler " << worker << " put box " << boxid << " into packaging queue (packaging queue size is " << packagingsize << " :"  << put_time(ptm, "%X") << endl;
            output.unlock(); //unlock
        }
        else //to prevent mutex from staying locked if the case in if statement is false
        {
            fqueue.unlock();
        }
    }
}


void packager(int min, int max, int worker)
{
    int boxid;
    while (pcounter1 + pcounter2 <= totalitem+1) //until the total number of boxes packaged is the total demand, package
    {
        pqueue.lock(); //lock the mutex for packaging
        if (!packing.isEmpty() && pcounter1 + pcounter2 <= totalitem + 1) //check if any other thread change the trueness value of that bool
        {
            packing.dequeue(boxid); //take the box out of packaging queue
            int packagingsize = packing.getCurrentSize(); //get size in the mutex to prevent it from changing afterwards
            if (worker == 1) //increment the count of packaged box according to the workers
            {
                pcounter1++;
            }
            else
            {
                pcounter2++;
            }
            pqueue.unlock(); //unlock packaging queue
            //start, example from the lecture sample
            time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
            struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
            localtime_s(ptm, &tt);  //converting the time structures
            //end for the sample code
            int duration = random_range(min, max); //create a random value between min and max
            output.lock(); //lock output to cout the message
            cout << "Packager " << worker << " started packaging the box " << boxid << " (packaging queue size is " << packagingsize << "): " << put_time(ptm, "%X") << endl;
            output.unlock(); //unlock cout
            this_thread::sleep_for(chrono::seconds(duration)); //sleep for random amount of time
            tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time, get the new time
            localtime_s(ptm, &tt);  //converting the time structures 
            output.lock();//lock the output and print the time after waiting time elapsed
            cout << "Packager " << worker << " finished packaging the box " << boxid << ": " << put_time(ptm, "%X") << endl;
            output.unlock(); //unlock cout
        }
        else //so that pqueue wont be locked if the if statement returns false
        {
            pqueue.unlock();
        }
    }
}

int main()
{

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
    
    //start, example from the lecture samples
    time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time
    struct tm* ptm = new struct tm;  //creating the time struct to be used in thread
    localtime_s(ptm, &tt);  //converting the time structures
    
    //end

    cout << "Time is now " << put_time(ptm, "%X") << endl;  //displaying the time  
    thread t1(&produce, minpro, maxpro);//start the threads
    thread t2(&filler, minfil, maxfil, 1);
    thread t3(&filler, minfil, maxfil, 2);
    thread t4(&packager, minpac, maxpac, 1);
    thread t5(&packager, minpac, maxpac, 2);

    t1.join(); //make sure every thread finishes before the termiantion of the program
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    tt = chrono::system_clock::to_time_t(chrono::system_clock::now());  //gets the current time to print the end time
    localtime_s(ptm, &tt);  //converting the time structures
    cout << "End of the simulation ends: " << put_time(ptm, "%X") << endl;

    return 0;
}


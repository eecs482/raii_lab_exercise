#include <iostream>
#include "thread.h"

using std::cout;
using std::endl;

mutex mutex1;
cv cv1;

int child_done = 0;		// global variable; shared between the two threads

void child(void *a)
{
    auto message = static_cast<char *>(a);

    mutex1.lock();
    cout << "child called with message " << message << ", setting child_done = 1" << endl;
    child_done = 1;
    cv1.signal();
    mutex1.unlock();
}

void parent(void *a)
{
    auto arg = reinterpret_cast<intptr_t>(a);

    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1 (reinterpret_cast<thread_startfunc_t>(child), 
                const_cast<void *>(static_cast<const void *>("test message")));

    mutex1.lock();
    while (!child_done) {
        cout << "parent waiting for child to run\n";
        cv1.wait(mutex1);
    }
    cout << "parent finishing" << endl;
    mutex1.unlock();
}

int main()
{
    cpu::boot(reinterpret_cast<thread_startfunc_t>(parent), reinterpret_cast<void *>(100), 0); 
}


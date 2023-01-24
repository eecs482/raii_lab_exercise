#include <iostream>
#include "thread.h"

using std::cout;
using std::endl;

mutex mutex1;
cv cv1;

int child_done = 0;		// global variable; shared between the two threads

class raii_lock {
public:
  // acquire resources in constructor
  raii_lock(mutex &in) : my_mutex(in) {
    my_mutex.lock();
  }
  // release resources in constructor
  ~raii_lock() { 
    my_mutex.unlock();
  }
private:
  mutex &my_mutex;
};

void child(void *a)
{
    auto message = static_cast<char *>(a);

    raii_lock lock(mutex1);
    cout << "child called with message " << message << ", setting child_done = 1" << endl;
    child_done = 1;
    cv1.signal();
}

void parent(void *a)
{
    auto arg = reinterpret_cast<intptr_t>(a);

    {
        raii_lock lock(mutex1);
        cout << "parent called with arg " << arg << endl;
    }

    thread t1 (reinterpret_cast<thread_startfunc_t>(child), 
                const_cast<void *>(static_cast<const void *>("test message")));

    raii_lock lock(mutex1);
    while (!child_done) {
        cout << "parent waiting for child to run\n";
        cv1.wait(mutex1);
    }
    cout << "parent finishing" << endl;
}

int main()
{
    cpu::boot(reinterpret_cast<thread_startfunc_t>(parent), reinterpret_cast<void *>(100), 0); 
}


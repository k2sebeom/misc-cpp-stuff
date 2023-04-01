#include <thread>
#include <mutex>
#include <iostream>
#include <queue>
#include <chrono>

using namespace std::chrono_literals;

std::mutex mu;

class Foo
{
private:
    std::thread worker;
    

public:
    std::queue<int> buffer;

    void start() {
        worker = std::thread(&Foo::run, this);
    }

    void run() {
        int i = 0;
        while(i < 29) {
            if(!buffer.empty()) {
                mu.lock();
                int b = buffer.front();
                buffer.pop();
                std::cout << "Popped " << b << std::endl;
                mu.unlock();
                i++;
                std::this_thread::sleep_for(2ms);
            }
        }
    }

    void wait() {
        worker.join();
    }
};


int fib(int n) {
    if(n < 2) {
        return 1;
    }
    return fib(n-1) + fib(n - 2);
}

void bar() {
    fib(150);
}

int main() {
    Foo foo;
    foo.start();
    for(int i = 0; i < 30; i++) {
        mu.lock();
        foo.buffer.push(i);
        std::cout << "Pushed " << i << std::endl;
        mu.unlock();
        std::this_thread::sleep_for(5ms);
    }
    foo.wait();

    std::thread *workers[10];
    for(int i = 0; i < 10; i++) {
        workers[i] = new std::thread(bar);
    }
    workers[0]->join();

    for(int i = 0; i < 10; i++) {
        delete workers[i];
    }

    return 0;
}
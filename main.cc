#include <iostream>
#include <math.h>

class Point {
    private:
        int x_;
        int y_;
    
    public:
        float norm();

        int& get_x();

        Point(int x, int y) {
            x_ = x;
            y_ = y;
        }

        void print() {
            std::cout << x_ << "," << y_ << std::endl;
        }
};

float Point::norm() {
    return sqrt(x_ * x_ + y_ * y_);
}

int& Point::get_x() {
    return x_;
}

int fib(int n) {
    if(n < 2) {
        return 1;
    }
    return fib(n-1) + fib(n-2);
}

int main() {
    int* a;
    int b = 5;
    a = &b;
    int*& c = a;
    *c = 15;
    std::cout << a << std::endl;
    std::cout << &a << std::endl;
    std::cout << *a << std::endl;
    // for(int i = 1; i < 500; i++) {
    //     std::cout << "Trial " << i << ": " << fib(43) << std::endl;
    // }
    return 0;
}
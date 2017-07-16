// Test the threadpool project
#include <iostream>

#include "util.h"
#include "thread.h"
#include "threadpool.h"

class TestTask : public tp_ns::Task {
public:
    int run(void *arg) override
    {
        for (int i = 0; i < 10; ++i) {
            std::cout << i << '\t';
        }
        std::cout << std::endl;
        return 0;
    }
};

class TestTask1 : public tp_ns::Task {
public:
    int run(void *arg) override
    {
        int *a = reinterpret_cast<int*>(arg);
        std::cout << "input args: " << *a << std::endl;
        return 0;
    }
};


int main(int argc, char *argv[])
{
    TestTask tt;

    tp_ns::ThreadPool pool;
    pool.add_task(&tt); //`tt` must be definied before pool

    int arg = 11111;
    pool.add_task(new TestTask1(), (void*)&arg, true); //use `new` and the pool will free them

    std::cout << "task num: " << pool.get_task_num() << std::endl;

    pool.run();
    return 0;
}
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

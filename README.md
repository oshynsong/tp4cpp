# tp4cpp  -  A threadpool implemented for C++


## 1. Structure

- `Thread` : the wrapper class of pthread for easily usage
- `Task` : abstract base class for user-defined task
- `ThreadPool` : the main thread pool class

## 2. Usage

### Define the concrete task by deriving a sub-class from `Task`

```c++
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
```

### Init the ThreadPool object, add worker to the pool and calling `run`

```c++

    tp_ns::ThreadPool pool;
    pool.add_task(&tt); //`tt` should be definied before define `pool`

    //use `new` and the pool will free them
    int arg = 11111;
    pool.add_task(new TestTask1(), (void*)&arg, true); 

    pool.run();

```

The ThreadPool can free the `new` Task by given the third argument of the `add_task` method. You
can also add the local task object which should defined before the ThreadPool object.

------
- Contact: dualyangsong@gmail.com
- Copyrights 2016 (c) Oshyn Song


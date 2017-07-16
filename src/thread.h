/**
 * A thread pool framework
 * Copyright 2017 (c), Oshyn Song (dualyangsong@gmail.com)
 *
 * @file    thread.h
 * @author  Oshyn Song
 * @time    2017.6
 */
#ifndef THREADPOOL_THREAD_H
#define THREADPOOL_THREAD_H

#include <pthread.h>

#include "common.h"
#include "util.h"

BEGIN_NAMESPACE

class Task;

class Thread {
public:
    enum State {
        CREATING,
        RUNNING,
        SUSPENDED,
        DEAD
    };

    // Default is suspend and not detached
    Thread();
    Thread(bool create_suspend, bool detached, const char *name);

    // No copying
    Thread(const Thread &) = delete;
    Thread &operator=(const Thread &) = delete;

    virtual ~Thread();

    virtual void run() = 0;

    static void *thread_function(void *);

    bool start();
    bool join();

    // Test if the thread is detached or not
    bool is_daemon() const;

    // Must be called before start()
    void set_daemon();

    // Suspend the thread using semaphore
    void suspend();

    // Wakes up the thread for running
    void resume();

    // Request for cancelling by other thread
    bool cancel();

    unsigned long get_tid() const;
    int get_last_error() const;

    const char *get_name() const;
    void set_name(const char *);

    void set_thread_state(State state);
    State get_thread_state() const;

protected: // Just called by the thread self
    // Exit by self
    void exit();

    void set_error_code(int);

    int get_priority();
    void set_priority(int);

    int get_concurrency();
    void set_concurrency(int);

private:
    pthread_t    _id;
    bool         _create_suspend;
    bool         _detached;
    const char * _name;
    int          _error_code;
    State        _state;
    Semaphore    _semaphore;
};

class Worker : public Thread {
public:
    Worker();
    explicit Worker(Task *task);
    Worker(Task *task, void *arg);
    Worker(Task *task, void *arg, bool create_suspend,
           bool detached, const char *name);
    ~Worker();

    void run() override;

    void set_task(Task *task, void *arg=nullptr) { _task = task; _task_arg = arg; }
    Task *get_task() const { return _task; }

private:
    Task * _task;
    void * _task_arg;
};

inline Worker *NewWorker()
{
    return new Worker();
}
inline Worker *NewWorker(Task *task)
{
    return new Worker(task);
}
inline Worker *NewWorker(Task *task, void *arg)
{
    return new Worker(task, arg);
}
inline Worker *NewWorker(Task *task, void *arg,
        bool create_suspend, bool detached, const char *name)
{
    return new Worker(task, arg, create_suspend, detached, name);
}

END_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

/**
 * A thread pool framework
 * Copyright 2017 (c), Oshyn Song (dualyangsong@gmail.com)
 *
 * @file    threadpool.h
 * @author  Oshyn Song
 * @time    2017.6
 */
#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <vector>
#include <list>
#include <stack>
#include <unordered_map>

#include "common.h"
#include "util.h"
#include "thread.h"
#include "task.h"

BEGIN_NAMESPACE

class IdleThreadsStack {
public:
    IdleThreadsStack() = default;
    ~IdleThreadsStack() = default;

    void push(Thread*);
    Thread* pop();
    Thread* top() const;
    void clear();

    bool is_empty() const;
    size_t size() const;

private:
    std::stack<Thread*> _threads;
    Mutex               _mutex;
};

class BusyThreadsList {
public:
    BusyThreadsList() = default;
    ~BusyThreadsList() = default;

    void enter(Thread*);
    Thread* leave();
    Thread* front() const;
    bool exist(Thread*) const;
    void remove(Thread*);
    void clear();

    bool is_empty() const;
    size_t size() const;

private:
    std::list<Thread*> _threads;
    Mutex              _mutex;
};

class TaskQueue {
public:
    TaskQueue() = default;
    ~TaskQueue() = default;

    void enter(Task*);
    Task* leave();
    Task* front() const;
    bool exist(Task*) const;
    void remove(Task*);
    void clear();

    bool is_empty() const;
    size_t size() const;

private:
    std::list<Task*> _tasks;
    Mutex            _mutex;
};

class ThreadPool {
public:
    ThreadPool();
    explicit ThreadPool(unsigned long long threads);
    ~ThreadPool();

    bool add_worker(Thread * worker, bool need_clear=false);
    bool add_task(Task *, void *arg=nullptr, bool need_clear=false);
    void run();
    void stop();

    size_t get_thread_num() const { return _all_threads.size(); }
    size_t get_idle_thread_num() const { return _idle_threads.size(); }
    size_t get_busy_thread_num() const { return _busy_threads.size(); }
    size_t get_task_num() const { return _tasks.size(); }

protected:
    void terminate();
    void retrieve_busy_to_idle();

private:
    // Whole threads: pointer to a thread => clear needed
    std::vector<std::pair<Thread*, bool>>             _all_threads;

    // Task and its args and clear needed flag
    std::unordered_map<Task*, std::pair<void*, bool>> _task_args;

    IdleThreadsStack     _idle_threads;
    BusyThreadsList      _busy_threads;
    TaskQueue            _tasks;
};

END_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

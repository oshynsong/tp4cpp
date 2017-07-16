/**
 * A thread pool framework
 * Copyright 2017 (c), Oshyn Song (dualyangsong@gmail.com)
 *
 * @file    threadpool.cpp
 * @author  Oshyn Song
 * @time    2017.7
 */

#include "threadpool.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

BEGIN_NAMESPACE

unsigned long long g_threadpool_max_thread_num  = 100;
unsigned long long g_threadpool_init_thread_num = 10;
unsigned long long g_threadpool_max_task_num    = 100;

// Definition of class IdleThreadsStack
Thread* IdleThreadsStack::pop()
{
    _mutex.lock();
    Thread* top = _threads.top();
    _threads.pop();
    _mutex.unlock();

    return top;
}

void IdleThreadsStack::push(Thread *thread)
{
    _mutex.lock();
    _threads.push(thread);
    _mutex.unlock();
}

Thread* IdleThreadsStack::top() const
{
    return _threads.top();
}

void IdleThreadsStack::clear()
{
    _mutex.lock();
    while (!_threads.empty()) {
        _threads.pop();
    }
    _mutex.unlock();
}

bool IdleThreadsStack::is_empty() const
{
    return _threads.empty();
}

size_t IdleThreadsStack::size() const
{
    return _threads.size();
}

// Definition of class BusyThreadsList
void BusyThreadsList::enter(Thread *thread)
{
    _mutex.lock();
    _threads.push_back(thread);
    _mutex.unlock();
}

Thread *BusyThreadsList::leave()
{
    _mutex.lock();
    Thread *thread = _threads.front();
    _threads.pop_front();
    _mutex.unlock();

    return thread;
}

Thread* BusyThreadsList::front() const
{
    return _threads.front();
}

bool BusyThreadsList::exist(Thread *thread) const
{
    return std::find(_threads.begin(), _threads.end(), thread) != _threads.end();
}

void BusyThreadsList::remove(Thread *thread)
{
    _mutex.lock();
    _threads.erase(std::find(_threads.begin(), _threads.end(), thread));
    _mutex.unlock();
}

void BusyThreadsList::clear()
{
    _mutex.lock();
    _threads.clear();
    _mutex.unlock();
}

bool BusyThreadsList::is_empty() const
{
    return _threads.empty();
}

size_t BusyThreadsList::size() const
{
    return _threads.size();
}

// Definition of class TaskQueue
void TaskQueue::enter(Task *task)
{
    auto pos = find_if(
            _tasks.begin(), _tasks.end(),
            [task](Task *t) -> bool
            {return task->get_priority() >= t->get_priority();}
        );
    _mutex.lock();
    _tasks.insert(pos, task);
    _mutex.unlock();
}

Task* TaskQueue::leave()
{
    _mutex.lock();
    Task *front = _tasks.front();
    _tasks.pop_front();
    _mutex.unlock();

    return front;
}

Task* TaskQueue::front() const
{
    return _tasks.front();
}

bool TaskQueue::exist(Task *task) const
{
    return std::find(_tasks.begin(), _tasks.end(), task) != _tasks.end();
}

void TaskQueue::remove(Task *task)
{
    _mutex.lock();
    _tasks.erase(std::find(_tasks.begin(), _tasks.end(), task));
    _mutex.unlock();
}

void TaskQueue::clear()
{
    _tasks.clear();
}

bool TaskQueue::is_empty() const
{
    return _tasks.empty();
}

size_t TaskQueue::size() const
{
    return _tasks.size();
}

// Definition of class ThreadPool
ThreadPool::ThreadPool() : ThreadPool(g_threadpool_init_thread_num)
{
    // Nothing to do
}

ThreadPool::ThreadPool(unsigned long long init_threads)
{
    if (init_threads > g_threadpool_max_thread_num) {
        throw std::runtime_error("The initial threads number is too large!");
    }

    for (unsigned long long i = 0; i < init_threads; ++i) {
        Worker *w = NewWorker();
        _all_threads.push_back(std::pair<Thread*, bool>(w, true));
        _idle_threads.push(w);
        w->start();
    }
}

ThreadPool::~ThreadPool()
{
    terminate();
    // Clear the whole threads if needed
    for (auto &t : _all_threads) {
        if (t.first != nullptr && t.second) {
            delete t.first;
            t.first = nullptr;
        }
    }
    _all_threads.clear();

    // Clear the whole tasks if needed
    std::unordered_map<Task*, std::pair<void*, bool>>::iterator iter;
    for (iter = _task_args.begin(); iter != _task_args.end(); ++iter) {
        if (iter->first != nullptr && iter->second.second) {
            delete iter->first;
        }
    }
    _task_args.clear();

    _idle_threads.clear();
    _busy_threads.clear();
    _tasks.clear();
}

bool ThreadPool::add_worker(Thread *worker, bool need_clear)
{
    if (_all_threads.size() >= g_threadpool_max_thread_num) {
        return false;
    }

    _all_threads.push_back(std::pair<Thread*, bool>(worker, need_clear));
    _idle_threads.push(worker);
    return true;
}

bool ThreadPool::add_task(Task *task, void *arg, bool need_clear)
{
    if (_tasks.size() >= g_threadpool_max_task_num) {
        return false;
    }

    // Thread safe operation
    _tasks.enter(task);
    _task_args[task] = std::pair<void *, bool>(arg, need_clear);
    return true;
}

void ThreadPool::run()
{
    while (!_tasks.is_empty()) {
        while (_idle_threads.is_empty()) {
            retrieve_busy_to_idle();
        }

        Task *task = _tasks.leave();
        Worker *worker = dynamic_cast<Worker *>(_idle_threads.pop());
        if (worker == nullptr) {
            continue;
        }
        std::cout << "task" << task->get_tid() << std::endl;

        task->set_executor(worker);
        worker->set_task(task, _task_args[task].first);
        _busy_threads.enter(worker);

        worker->resume();
    }
}

void ThreadPool::stop()
{
    while (!_busy_threads.is_empty()) {
        Thread *t = _busy_threads.leave();
        if (t->get_thread_state() == Thread::State::RUNNING) {
            t->join();
        }
        _idle_threads.push(t);
    }
}

void ThreadPool::terminate()
{
    // Wait for the busy threads to be idle
    // Remove from the busy list and add to the idle list
    while (!_busy_threads.is_empty()) {
        for (auto &thread : _all_threads) {
            if (_busy_threads.exist(thread.first)) {
                if (thread.first->get_thread_state() == Thread::State::SUSPENDED) {
                    _busy_threads.remove(thread.first);
                    _idle_threads.push(thread.first);
                }
            }
        }
    }
    // Wait for all idle threads to finish
    while (!_idle_threads.is_empty()) {
        Thread *w = _idle_threads.pop();
        w->cancel();
        w->join();
    }
}

// Retrive the busy thread to idle when there is no idle threads
void ThreadPool::retrieve_busy_to_idle()
{
    while (_idle_threads.is_empty()) {
        for (auto &thread : _all_threads) {
            if (_busy_threads.exist(thread.first)) {
                if (thread.first->get_thread_state() == Thread::State::SUSPENDED) {
                    _busy_threads.remove(thread.first);
                    _idle_threads.push(thread.first);
                }
            }
        }
    }
}

END_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

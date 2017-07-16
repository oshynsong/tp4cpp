/**
 * A thread pool framework
 * Copyright 2017 (c), Oshyn Song (dualyangsong@gmail.com)
 *
 * @file    thread.cpp
 * @author  Oshyn Song
 * @time    2017.6
 */
#include "thread.h"
#include "task.h"

BEGIN_NAMESPACE

// Static function for pthread_create interface
void *Thread::thread_function(void *arg)
{
    Thread *call_obj = reinterpret_cast<Thread *>(arg);

    while (true) {
        switch (call_obj->get_thread_state()) {
            case RUNNING:
                call_obj->run();
                call_obj->set_thread_state(SUSPENDED);
                break;
            case SUSPENDED: // only fit in the first SUSPENDED state
                call_obj->suspend();
                break;
            case DEAD:
                call_obj->exit();
                break;
            default:
                call_obj->suspend();
                break;
        }
    }

    return nullptr;
}

Thread::Thread() : Thread(true, false, nullptr)
{
    // Nothine to do
}

Thread::Thread(bool create_suspend, bool detached, const char *name) : 
    _id(), _create_suspend(create_suspend), _detached(detached),
    _name(name), _error_code(0), _state(CREATING), _semaphore(0)
{
    // Nothine to do
    if (_create_suspend) {
        _state = SUSPENDED;
    } else {
        _state = RUNNING;
    }
}

Thread::~Thread()
{
    _create_suspend = true;
    _detached       = false;
    _error_code     = 0;
    if (_name) {
        _name = nullptr;
    }
    _state = DEAD;
}

bool Thread::start()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (_detached) {
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    } else {
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    }

    int status = pthread_create(&_id, &attr, Thread::thread_function,
            reinterpret_cast<void *>(this));
    pthread_attr_destroy(&attr);

    return (0 == status);
}

bool Thread::join()
{
    return (0 == pthread_join(_id, NULL));
}

bool Thread::is_daemon() const
{
    return (true == _detached);
}

void Thread::set_daemon()
{
    _detached = true;
}

void Thread::suspend()
{
    _state = SUSPENDED;
    _semaphore.wait();
}

void Thread::resume()
{
    _state = RUNNING;
    _semaphore.signal();
}

bool Thread::cancel()
{
    if (0 == pthread_cancel(_id)) {
        _state = DEAD;
        return true;
    }
    return false;
}

void Thread::exit()
{
    _state = DEAD;
    pthread_exit(NULL);
}

unsigned long Thread::get_tid() const
{
    return static_cast<unsigned long>(_id);
}

int Thread::get_last_error() const
{
    return _error_code;
}

const char * Thread::get_name() const
{
    return _name;
}

void Thread::set_name(const char *name)
{
    _name = name;
}

void Thread::set_thread_state(Thread::State state)
{
    _state = state;
}

Thread::State Thread::get_thread_state() const
{
    return _state;
}

int Thread::get_priority()
{
    int policy;
    sched_param sp;
    pthread_getschedparam(_id, &policy, &sp);
    return sp.sched_priority;
}

void Thread::set_priority(int p)
{
    int policy;
    sched_param sp;
    pthread_getschedparam(_id, &policy, &sp);
    sp.sched_priority = p;
    pthread_setschedparam(_id, policy,
            reinterpret_cast<const struct sched_param*>(&sp));
}

int Thread::get_concurrency()
{
    return pthread_getconcurrency();
}

void Thread::set_concurrency(int level)
{
    pthread_setconcurrency(level);
}

void Thread::set_error_code(int ecode)
{
    _error_code = ecode;
}

// Class definition of Worker
Worker::Worker() : Worker(nullptr)
{
    // Nothing to do
}

Worker::Worker(Task *task) : Worker(task, nullptr)
{
    // Nothing to do
}

Worker::Worker(Task *task, void *arg) : Worker(task, arg, true, false, nullptr)
{
    // Nothing to do
}

Worker::Worker(Task *task, void *arg, bool create_suspend, bool detached, const char *name)
    : Thread(create_suspend, detached, name), _task(task), _task_arg(arg)
{
    // Nothing to do
}

Worker::~Worker()
{
    if (_task) {
        _task = nullptr;
    }
    if (_task_arg) {
        _task_arg = nullptr;
    }
}

void Worker::run()
{
    if (_task == nullptr) {
        return;
    }

    int ret = _task->run(_task_arg);

    this->set_error_code(ret);
    if (ret == 0) {
        _task = nullptr;
        _task_arg = nullptr;
    }
}

END_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

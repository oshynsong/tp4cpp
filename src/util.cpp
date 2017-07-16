/**
 * A thread pool framework
 * Copyright 2017 (c), Oshyn Song (dualyangsong@gmail.com)
 *
 * @file    util.h
 * @author  Oshyn Song
 * @time    2017.6
 */

#include "util.h"

BEGIN_NAMESPACE

Mutex::Mutex() : _mutex()
{
    pthread_mutex_init(&_mutex, nullptr);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&_mutex);
}

bool Mutex::lock()
{
    if (0 != pthread_mutex_trylock(&_mutex)) {
        pthread_mutex_unlock(&_mutex);
    }
    return (0 == pthread_mutex_trylock(&_mutex));
}

bool Mutex::unlock()
{
    return (0 == pthread_mutex_unlock(&_mutex));
}

Condition::Condition(Mutex *mutex) : _cond(), _mutex(&(mutex->_mutex))
{
    pthread_cond_init(&_cond, nullptr);
}

Condition::~Condition()
{
    pthread_cond_destroy(&_cond);
    if (_mutex != nullptr) {
        _mutex = nullptr;
    }
}

void Condition::wait()
{
    pthread_cond_wait(&_cond, _mutex);
}

void Condition::signal()
{
    pthread_cond_signal(&_cond);
}

void Condition::broadcast()
{
    pthread_cond_broadcast(&_cond);
}

Semaphore::Semaphore() : Semaphore(1)
{
    // Nothing to do
}

Semaphore::Semaphore(long long count) : _count(count), _mutex(), _cond(&_mutex)
{
    // Nothing to do
}

void Semaphore::wait()
{
    _mutex.lock();
    if (--_count < 0) {
        _cond.wait();
    }
    _mutex.unlock();
}

void Semaphore::signal()
{
    _mutex.lock();
    if (++_count <= 0) {
        _cond.signal();
    }
    _mutex.unlock();
}

END_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

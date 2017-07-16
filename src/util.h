/**
 * A thread pool framework
 * Copyright 2017 (c), Oshyn Song (dualyangsong@gmail.com)
 *
 * @file    util.h
 * @author  Oshyn Song
 * @time    2017.6
 */
#ifndef THREADPOOL_UTIL_H
#define THREADPOOL_UTIL_H

#include <pthread.h>

#include "common.h"

BEGIN_NAMESPACE

class Mutex {
public:
    friend class Condition;
    Mutex();
    ~Mutex();
    bool lock();
    bool unlock();

private:
    pthread_mutex_t _mutex;
};

class Condition {
public:
    explicit Condition(Mutex *);
    ~Condition();
    void wait();
    void signal();
    void broadcast();

private:
    pthread_cond_t   _cond;
    pthread_mutex_t *_mutex;
};

class Semaphore {
public:
    Semaphore();
    explicit Semaphore(long long count);

    void wait();
    void signal();

private:
    long long _count;
    Mutex     _mutex;
    Condition _cond;
};

END_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

/**
 * A thread pool framework
 * Copyright 2017 (c), Oshyn Song (dualyangsong@gmail.com)
 *
 * @file    task.h
 * @author  Oshyn Song
 * @time    2017.6
 */
#ifndef THREADPOOL_TASK_H
#define THREADPOOL_TASK_H

#include <unordered_map>

#include "common.h"

BEGIN_NAMESPACE

using task_id_t = unsigned long long;

class Thread;

/**
 *
 * Base class for the abstract task to be running by threads
 */
class Task {
public:
    enum Priority {
        LOW,
        NORMAL,
        HIGH
    };

    Task();
    virtual ~Task();

    virtual int run(void *) = 0;

    task_id_t get_tid() const;

    void set_tname(const char *);
    const char *get_tname() const;

    void set_executor(Thread *);
    Thread *get_executor() const;

    void set_priority(Priority);
    Priority get_priority() const;

private:
    task_id_t        _tid;
    const char *     _tname;
    Thread *         _executor;
    Priority         _priority;

    static task_id_t _cur_tid;
    static std::unordered_map<task_id_t, bool> _asigned_tids;
};

END_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

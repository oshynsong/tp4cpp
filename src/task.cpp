/**
 * A thread pool framework
 * Copyright 2017 (c), Oshyn Song (dualyangsong@gmail.com)
 *
 * @file    task.cpp
 * @author  Oshyn Song
 * @time    2017.6
 */
#include "task.h"

BEGIN_NAMESPACE

unsigned long long g_task_max_id_cache_num = 100;

// Task id is unsigned long long starts from 1
task_id_t Task::_cur_tid = 0;
std::unordered_map<task_id_t , bool> Task::_asigned_tids;

Task::Task() : _tid(0), _tname(nullptr), _executor(nullptr), _priority(NORMAL)
{
    // Maintain the tid facility
    task_id_t avaliable_tid = 0;
    for (auto it = _asigned_tids.cbegin(); it != _asigned_tids.cend(); ++it) {
        if (!it->second) {
            avaliable_tid = it->first;
        }
    }

    while (avaliable_tid == 0 || 
            (_asigned_tids.count(avaliable_tid) > 0 &&
             _asigned_tids[avaliable_tid])) {
        avaliable_tid = ++_cur_tid;
    }

    _tid = avaliable_tid;
    _asigned_tids[_tid] = true;
}

Task::~Task()
{
    if (!_tname) {
        _tname = nullptr;
    }
    if (!_executor) {
        _executor = nullptr;
    }

    if (_asigned_tids.count(_tid) > 0) {
        if (_asigned_tids.size() > g_task_max_id_cache_num) {
            _asigned_tids.erase(_tid);
            _cur_tid = 0;
        } else {
            _asigned_tids[_tid] = false;
        }
    }
}

task_id_t Task::get_tid() const
{
    return _tid;
}

void Task::set_tname(const char *tname)
{
    _tname = tname;
}

const char *Task::get_tname() const
{
    return _tname;
}

void Task::set_executor(Thread *executor)
{
    _executor = executor;
}

Thread *Task::get_executor() const
{
    return _executor;
}

void Task::set_priority(Priority priority)
{
    if (static_cast<int>(priority) < LOW ||
            static_cast<int>(priority) > HIGH) {
        return;
    }
    _priority = priority;
}

Task::Priority Task::get_priority() const
{
    return _priority;
}

END_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

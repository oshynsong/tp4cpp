/**
 * A thread pool framework
 * Copyright 2017 (c), Oshyn Song (dualyangsong@gmail.com)
 *
 * @file    common.h
 * @author  Oshyn Song
 * @time    2017.6
 */
#ifndef THREADPOOL_COMMON_H
#define THREADPOOL_COMMON_H

#define BEGIN_NAMESPACE \
    namespace threadpool {
#define END_NAMESPACE }
#define tp_ns threadpool

BEGIN_NAMESPACE

extern unsigned long long g_task_max_id_cache_num;

extern unsigned long long g_threadpool_max_thread_num;
extern unsigned long long g_threadpool_init_thread_num;
extern unsigned long long g_threadpool_max_task_num;

END_NAMESPACE
#endif
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

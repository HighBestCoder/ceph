// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2011 New Dream Network
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#ifndef CEPH_DEBUG_H
#define CEPH_DEBUG_H

#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

#include "common/dout.h"

/* Global version of the stuff in common/dout.h
 */

#define dout(v) ldout((dout_context), (v))

#define pdout(v, p) lpdout((dout_context), (v), (p))

#define dlog_p(sub, v) ldlog_p1((dout_context), (sub), (v))

#define generic_dout(v) lgeneric_dout((dout_context), (v))

#define derr lderr((dout_context))

#define generic_derr lgeneric_derr((dout_context))

#define CEPH_ERROR 0
#define CEPH_WARN 1
#define CEPH_INFO 2

static void dc_common_vlog_stderr(int facility, const char* format, va_list args) {
    char str[4096];
    memset(str, 0, sizeof(str));

    static int64_t process_id = 0;
    if (process_id == 0) {
        process_id = getpid();
    }

    vsnprintf(str, sizeof(str), format, args);

    struct timeval tv;
    struct tm* tm = nullptr;
    char time_str[128];
    memset(time_str, 0, sizeof(time_str));
    uint64_t thd_id;

    /* Get timestamp */
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

    // get thread id
    thd_id = (uint64_t)pthread_self();

    switch (facility) {
        case CEPH_ERROR:
            fprintf(stderr, "%s %ld ERROR: %lu %s\n", time_str, process_id, thd_id, str);
            break;
        case CEPH_WARN:
            fprintf(stderr, "%s %ld WARN: %lu %s\n", time_str, process_id, thd_id, str);
            break;
        case CEPH_INFO:
            fprintf(stderr, "%s %ld INFO: %lu %s\n", time_str, process_id, thd_id, str);
            break;
        default:
            break;
    }
}

static void dc_common_vlog1(int facility, const char* format, va_list args) { dc_common_vlog_stderr(facility, format, args); }

static void dc_common_vlog(int facility, int level, const char* format, va_list args) { dc_common_vlog1(facility, format, args); }

static void dc_common_log(int facility, int level, const char* format, ...) {
    va_list args;

    va_start(args, format);
    dc_common_vlog1(facility, format, args);
    va_end(args);
}

static void dc_common_trace_log1(const char* file_name, const char* func_name, const int line_number, const int facility, const char* format, va_list args) {
    char str[4096];

    memset(str, 0, sizeof(str));
    vsnprintf(str, sizeof(str), format, args);

    dc_common_log(facility, 0 /*level*/, "%s at %s(%s:%d)", str, func_name, file_name, line_number);
}

static void dc_common_trace_log(const char* file_name, const char* func_name, const int line_number, const int facility, const char* format, ...) {
    va_list args;

    va_start(args, format);

    dc_common_trace_log1(file_name, func_name, line_number, facility, format, args);

    va_end(args);
}

#define LOG(...) dc_common_trace_log(__FILE__, __func__, __LINE__, __VA_ARGS__)

#define LOG_ROOT_ERR(ret, ...)                        \
    do {                                              \
        LOG(CEPH_ERROR, __VA_ARGS__);                 \
        LOG(CEPH_ERROR, "StackTrace:\n\t\t\t\t\t\t"); \
    } while (0)

#define LOG_CHECK_ERR_RETURN(ret, ...) \
    do {                               \
        if (unlikely(ret < 0)) {       \
            LOG(CEPH_ERROR, "\t at");  \
            return ret;                \
        }                              \
    } while (0)

#define LOG_CHECK_ERR(ret, ...)       \
    do {                              \
        if (unlikely(ret < 0)) {      \
            LOG(CEPH_ERROR, "\t at"); \
        }                             \
    } while (0)

#endif

```C++
/*
 * DO NOT CHANGE THESE.
 * DO NOT ADD MORE VALUES.
 */
#define DC_COMMON_LOG_ERROR	            0
#define DC_COMMON_LOG_WARN	            1
#define DC_COMMON_LOG_INFO	            2

static void
dc_common_vlog_stderr(int facility, const char *format, va_list args)
{
    char str[4096];
    memset(str, 0, sizeof(str));

    if (process_id == 0) {
        process_id = getpid();
    }

    vsnprintf(str, sizeof(str), format, args);

    /* generate timestamp and print the timestamp into str[] array*/
    struct timeval tv;
    struct tm *tm;
    char time_str[128];
    memset(time_str, 0, sizeof(time_str));
    uint64_t thd_id;

    /* Get timestamp */
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

    std::lock_guard<std::mutex> lock(dc_common_log_mutex);

    if (!has_open_log_file && !has_try_to_open_file) {
        dc_common_vlog_handle_init();
        if (log_fp != NULL) {
            has_open_log_file = 1;
        }
        has_try_to_open_file = 1;
    }

    // get thread id
    thd_id = (uint64_t)pthread_self();


    switch (facility)
    {
    case DC_COMMON_LOG_ERROR:
        {
            if (has_open_log_file) {
                fprintf(log_fp, "%s %d ERROR: %lu %s\n", time_str, process_id, thd_id, str);
            } else {
                fprintf(stderr, "%s %d ERROR: %lu %s\n", time_str, process_id, thd_id, str);
            }
        }
        break;
    case DC_COMMON_LOG_WARN:
        {
            if (has_open_log_file) {
                fprintf(log_fp, "%s %d WARN: %lu %s\n", time_str, process_id, thd_id, str);
            } else {
                fprintf(stderr, "%s %d WARN: %lu %s\n", time_str, process_id, thd_id, str);
            }
        }
        break;
    case DC_COMMON_LOG_INFO:
        {
            if (has_open_log_file) {
                fprintf(log_fp, "%s %d INFO: %lu %s\n", time_str, process_id, thd_id, str);
            } else {
                fprintf(stderr, "%s %d INFO: %lu %s\n", time_str, process_id, thd_id, str);
            }
        }
    default:
        break;
    }
}

static void
dc_common_vlog1(int facility, const char *format, va_list args)
{
    dc_common_vlog_func(facility, format, args);
}

void dc_common_vlog(int facility, int level, const char *format, va_list args)
{
    if (level <= dc_common_log_level)
        dc_common_vlog1(facility, format, args);
}

void dc_common_log(int facility, int level, const char *format, ...)
{
    va_list args;

    if (level <= dc_common_log_level)
    {
        va_start(args, format);
        dc_common_vlog1(facility, format, args);
        va_end(args);
    }
}

static void __dc_unused
dc_common_trace_log(const char* file_name,
              const char* func_name,
              const int line_number,
              const int facility,
              const char* format,
              ...)
{
    va_list                             args;

    va_start(args, format);

    dc_common_trace_log1(file_name,
                   func_name,
                   line_number,
                   facility,
                   format,
                   args);

    va_end(args);
}

#define LOG(...) dc_common_trace_log(__FILE__,              \
                         __func__,                          \
                         __LINE__,                          \
                         __VA_ARGS__)

#define LOG_ROOT_ERR(ret, ...)    do {                      \
    LOG(DC_COMMON_LOG_ERROR, __VA_ARGS__);                  \
    LOG(DC_COMMON_LOG_ERROR,                                \
        "StackTrace: %s\n\t\t\t\t\t\t",                     \
        #ret);                                              \
} while (0)

#define LOG_CHECK_ERR_RETURN(ret, ...)                      \
    do {                                                    \
    if (unlikely(ret) {                                     \
        LOG(DC_COMMON_LOG_ERROR, "\t at");                  \
        return ret;                                         \
    }                                                       \
}while (0)
```
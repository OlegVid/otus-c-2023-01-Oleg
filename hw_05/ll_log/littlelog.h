#pragma once

typedef enum {
    LL_INFO, LL_EVENT, LL_DEBUG, LL_WARNING, LL_ERROR
} llog_level_t;

#define LOG_SEPARATOR  (';')

#define BACK_TRACE_DEPTH  (10)

#define ll_info(format, ...) llog_write(LL_INFO,__FILE__,__func__,__LINE__,format, __VA_ARGS__)
#define ll_event(format, ...) llog_write(LL_EVENT,__FILE__,__func__,__LINE__,format, __VA_ARGS__)
#define ll_debug(format, ...) llog_write(LL_DEBUG,__FILE__,__func__,__LINE__,format, __VA_ARGS__)
#define ll_warning(format, ...) llog_write(LL_WARNING,__FILE__,__func__,__LINE__,format, __VA_ARGS__)
#define ll_error(format, ...) llog_write(LL_ERROR,__FILE__,__func__,__LINE__,format, __VA_ARGS__)
#define ll_log(cat,format, ...) llog_write(cat,__FILE__,__func__,__LINE__,format, __VA_ARGS__)

int  llog_init(char *file_name);
void llog_write(llog_level_t lvl, const char *file,  const char *func, int line_no, char *format, ...);
void llog_close(void);

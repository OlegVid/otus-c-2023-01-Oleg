#include <stdio.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <time.h>
#include <pthread.h>
#include "littlelog.h"

#define FLAG_LOG_STARTED (0xAA05) //флаг того, что инициализировали лог, нет 1, - чтобы не ошибится или случайная запись в память ложно не активизировала

int fileno(FILE *stream);  // прототип функции fileno - она есть в стандартной библиотеке, но почуму то не внесена в ее хидеры, нужно явно указывать прототип
static FILE *log_file;      // файл лога
static int is_started = 0;  // флаг - начали ли запись

static size_t msg_global_num = 0;  // глобальный счетчик сколько всего всех видов сообщений записано
// ниже индивидуальный счетчики для каждого сообщения. - цель счетчиков - дополнительная инфорация облегчающая ориентацию в логе.
static size_t msg_info_num = 0;
static size_t msg_event_num = 0;
static size_t msg_warning_num = 0;
static size_t msg_debug_num = 0;
static size_t msg_error_num = 0;
pthread_mutex_t mutex;   // мьютекс
/**
 * time_ms() - возвращает время с момента включения в мс.
 */
static unsigned int time_ms(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec) * 1000 + ts.tv_nsec / 1000000;
}

/*
 * Инициализация- в случае успеха открытия файла - лог становится активным
 */
int llog_init(char *file_name) {
    log_file = fopen(file_name, "w");
    if (log_file == NULL) {
        return -1;
    }
    pthread_mutex_init(&mutex, NULL);

    is_started = FLAG_LOG_STARTED;

    fprintf(log_file,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(log_file,"<log>\n");
    return 0;
}

/*
 *  Функция логирования, пишет сообщения в файл, не используя предварительные буферы, это позволяет избегать malloc
 *  что хорошо в аварийных ситуациях. Пишется в формате XML, - одно событе - одна строка, но если тип события Error
 *  тогда вставляется backtrace_fd, он разбивает форматирование, но XML валидный и открывается сторонними форматерами и обработчиками
 */
void llog_write(llog_level_t lvl, const char *file, const char *func, int line_no, char *format, ...) {
if (is_started != FLAG_LOG_STARTED){
	return;
}
pthread_mutex_lock(&mutex);
	time_t unixtime = time(NULL);
	struct tm* ttm = localtime(&unixtime);

    switch (lvl) {
        case LL_INFO:
            fprintf(log_file, "<info key = \"%zu\" num = \"%zu\">",msg_global_num,msg_info_num);
            msg_info_num++;
            break;
        case LL_EVENT:
            fprintf(log_file, "<event key = \"%zu\" num = \"%zu\">",msg_global_num,msg_event_num);
            msg_event_num++;
            break;
        case LL_DEBUG:
            fprintf(log_file, "<debug key = \"%zu\" num = \"%zu\">",msg_global_num,msg_debug_num);
            msg_debug_num++;
            break;
        case LL_WARNING:
            fprintf(log_file, "<warning key = \"%zu\" num = \"%zu\">",msg_global_num,msg_warning_num);
            msg_warning_num++;
            break;
        case LL_ERROR:
            fprintf(log_file, "<error key = \"%zu\" num = \"%zu\">",msg_global_num,msg_error_num);
            msg_error_num++;
            break;
        default:
           fprintf(log_file, "<other key = \"%zu\" num = \"%d\">",msg_global_num,0);
            break;
    }

    fprintf(log_file, "<description>");
    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    fprintf(log_file, "</description>");
    fprintf(log_file, "<propertySet>");
	    fprintf(log_file, "<DataTime>%04d-%02d-%02d %02d:%02d:%02d:%04d</DataTime>",ttm->tm_year + 1900, ttm->tm_mon + 1, ttm->tm_mday, ttm->tm_hour, ttm->tm_min, ttm->tm_sec,time_ms());
	    fprintf(log_file, "<FileName>%s</FileName>",file);
	    fprintf(log_file, "<FuncName>%s</FuncName>",func);
	    fprintf(log_file, "<LineNo>%d</LineNo>",line_no);
    fprintf(log_file, "</propertySet>");

    	if (lvl == LL_ERROR){
		  void *array[BACK_TRACE_DEPTH];
		  size_t size = backtrace (array, BACK_TRACE_DEPTH);
		  fprintf(log_file, "<backTrace depth = \"%zu\" maxDepth = \"%d\">",size,BACK_TRACE_DEPTH);
		  fseek(log_file, 0, SEEK_END);
		  backtrace_symbols_fd(array, size, fileno(log_file));
		  fprintf(log_file, "</backTrace>");
	 }

    switch (lvl) {
        case LL_INFO:
            fprintf(log_file, "</info>\n");
            msg_info_num++;
            break;
        case LL_EVENT:
            fprintf(log_file, "</event>\n");
            msg_event_num++;
            break;
        case LL_DEBUG:
            fprintf(log_file, "</debug>\n");
            msg_debug_num++;
            break;
        case LL_WARNING:
            fprintf(log_file, "</warning>\n");
            msg_warning_num++;
            break;
        case LL_ERROR:
            fprintf(log_file, "</error>\n");
            msg_error_num++;
            break;
        default:
           fprintf(log_file, "</other>\n");
            break;
    }
  msg_global_num++;
pthread_mutex_unlock(&mutex);
}

void llog_close(void) {
	if (is_started == FLAG_LOG_STARTED){
	    fprintf(log_file,"</log>\n");
	    fclose(log_file);
            pthread_mutex_destroy(&mutex);
	}
}

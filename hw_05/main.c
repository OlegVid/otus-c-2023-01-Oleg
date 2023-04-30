/* homework for course c-2023-01 lesson No12
 *author: Oleg Videnin
 *date: 30/04/2023
 *
 *
*/
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "./ll_log/littlelog.h"

#define LOG_FILE_NAME_BUF_LEN  (1000)

char def_log_file[] = "./test_log.xml";

void foo_ffo(void) {
    ll_error("Error No %d", 14);
}

void *test_thread_one(void *param) {
    if (param) {}// avoid warning

    for (int i = 0; i < 100; i++) {
        ll_event("Thread one my Log mesage event  Number%d and substr %s", i, "substr");
        ll_info("", NULL); // test with void param
        sleep(0.010);
    }
    return (void *) 0;
}

void *test_thread_two(void *param) {
    if (param) {}// avoid warning
    for (int i = 0; i < 100; i++) {
        ll_event("Thread tri my Log mesage event  Number%d and substr %s", i, "substr");
        ll_info("Thread two my Log info  Number%d", i); // test with void param
        ll_error("", NULL); // test with void param
        ll_debug("", NULL); // test with void param
        sleep(0.010);
    }
    return (void *) 0;
}

void *test_thread_three(void *param) {
    if (param) {}// avoid warning
    for (int i = 0; i < 100; i++) {
        ll_event("Thread tri my Log mesage event  Number%d and substr %s", i, "substr");
        ll_info("", NULL); // test with void param
        ll_error("", NULL); // test with void param
        ll_debug("", NULL); // test with void param
        ll_warning("", NULL); // test with void param
        foo_ffo();
        sleep(0.010);
    }
    return (void *) 0;
}

static pthread_t ntid_1;
static pthread_t ntid_2;
static pthread_t ntid_3;

int main(int argc, char **argv) {
    char log_file_name[LOG_FILE_NAME_BUF_LEN];
    strcpy(log_file_name, def_log_file);

    if (argc > 1) {
        strncpy(log_file_name, argv[1], LOG_FILE_NAME_BUF_LEN - 1);
        log_file_name[LOG_FILE_NAME_BUF_LEN - 1] = '\0';
    }

    if (llog_init(log_file_name)) {
        printf("can not initialize log file %s\n", log_file_name);
        return -1;
    }
    printf("start\n");
    pthread_create(&ntid_1, NULL, test_thread_one, NULL);
    pthread_create(&ntid_2, NULL, test_thread_two, NULL);
    pthread_create(&ntid_3, NULL, test_thread_three, NULL);
    sleep(3);
    llog_close();
    printf("Finish, log was writed to file: %s\n", log_file_name);
    return 0;
}


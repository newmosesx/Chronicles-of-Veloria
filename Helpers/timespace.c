// file: timespace.c

#include <time.h> //https://www.tutorialspoint.com/c_standard_library/time_h.htm

// for universal level
int get_sys_hour(void){
    time_t raw_time;
    struct tm *local_time;
    int hour;
    
    time(&raw_time); // get the time
    local_time = localtime(&raw_time); // break it down, into a format
    hour = local_time->tm_hour;

    return hour;
}

int get_sys_day(void){
    time_t raw_time;
    struct tm *local_time;
    int day;
    
    time(&raw_time); // get the time
    local_time = localtime(&raw_time); // break it down, into a format
    day = local_time->tm_mday;

    return day;
}

int get_sys_min(void){
    time_t raw_time;
    struct tm *local_time;
    int min;
    
    time(&raw_time); // get the time
    local_time = localtime(&raw_time); // break it down, into a format
    min = local_time->tm_min;

    return min;
}
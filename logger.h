// file: logger.h

#ifndef LOGGER_H
#define LOGGER_H

#include <pthread.h>
#include <time.h> // For time_t

#define MAX_LOG_ENTRIES 256 // The max number of log messages to store
#define LOG_MESSAGE_LENGTH 256 // Max length of a single message
#define LOG_TTL_SECONDS 300 // Time-to-live for a log message (5 minutes)

// A struct to hold a single log message and its creation time
typedef struct {
    time_t timestamp;
    char message[LOG_MESSAGE_LENGTH];
} LogEntry;

// --- GLOBAL LOGGER DATA ---
// A ring buffer to store all recent log entries
extern LogEntry g_log_entries[MAX_LOG_ENTRIES];
// Mutex to protect the entire log system
extern pthread_mutex_t g_event_log_mutex;
// Index where the SIMULATION will write the next message
extern int g_log_write_index;
// Index from where the GUI should start reading messages
extern int g_log_read_index;


// --- FUNCTION PROTOTYPES ---
void init_logger(void);
void destroy_logger(void);
void log_event(const char* format, ...);
void clear_old_log_entries(void); // New function to prune old logs
void log_multiline_event(const char *report_string);

#endif // LOGGER_H
// file: logger.c

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "../logger.h"

// Definition of the global variables from the header
LogEntry g_log_entries[MAX_LOG_ENTRIES];
pthread_mutex_t g_event_log_mutex;
int g_log_write_index = 0;
int g_log_read_index = 0; // Start both at the same place

void init_logger(void) {
    if (pthread_mutex_init(&g_event_log_mutex, NULL) != 0) {
        printf("Logger mutex init has failed\n");
    }
    // Clear the entire buffer on startup
    for (int i = 0; i < MAX_LOG_ENTRIES; i++) {
        g_log_entries[i].timestamp = 0;
        g_log_entries[i].message[0] = '\0';
    }
}

void destroy_logger(void) {
    pthread_mutex_destroy(&g_event_log_mutex);
}

void log_multiline_event(const char *report_string) {
    if (!report_string) return;

    // Lock the mutex to ensure the entire report is logged as an atomic block,
    // preventing other log messages from appearing in the middle of it.
    pthread_mutex_lock(&g_event_log_mutex);

    const char *line_start = report_string;
    const char *line_end;

    // Loop through the entire report string, processing it line by line.
    while ((line_end = strchr(line_start, '\n')) != NULL) {
        // Calculate the length of the current line.
        size_t line_length = line_end - line_start;

        // Only process non-empty lines.
        if (line_length > 0) {
            // Get the next available slot in our circular buffer.
            int write_idx = g_log_write_index;

            // Prevent buffer overflow by ensuring we don't copy more than the buffer can hold.
            if (line_length >= LOG_MESSAGE_LENGTH) {
                line_length = LOG_MESSAGE_LENGTH - 1;
            }

            // Copy the line into the message buffer and null-terminate it.
            strncpy(g_log_entries[write_idx].message, line_start, line_length);
            g_log_entries[write_idx].message[line_length] = '\0';
            g_log_entries[write_idx].timestamp = time(NULL); // Or your timestamp mechanism

            // Advance the write index for the next message.
            g_log_write_index = (write_idx + 1) % MAX_LOG_ENTRIES;
        }
        
        // Move the start pointer to the beginning of the next line.
        line_start = line_end + 1;
    }

    // Handle the last line if the report doesn't end with a newline.
    if (*line_start != '\0') {
        int write_idx = g_log_write_index;
        strncpy(g_log_entries[write_idx].message, line_start, LOG_MESSAGE_LENGTH - 1);
        g_log_entries[write_idx].message[LOG_MESSAGE_LENGTH - 1] = '\0'; // Ensure null termination
        g_log_entries[write_idx].timestamp = time(NULL);
        g_log_write_index = (write_idx + 1) % MAX_LOG_ENTRIES;
    }

    // Unlock the mutex now that the entire operation is complete.
    pthread_mutex_unlock(&g_event_log_mutex);
}

// Rewritten to use the ring buffer
void log_event(const char* format, ...) {
    char temp_message[LOG_MESSAGE_LENGTH];
    va_list args;

    va_start(args, format);
    vsnprintf(temp_message, sizeof(temp_message), format, args);
    va_end(args);

    pthread_mutex_lock(&g_event_log_mutex);

    // Get current time
    time_t now = time(NULL);
    
    // Write the new log entry into the ring buffer at the current write position
    g_log_entries[g_log_write_index].timestamp = now;
    strncpy(g_log_entries[g_log_write_index].message, temp_message, LOG_MESSAGE_LENGTH - 1);
    g_log_entries[g_log_write_index].message[LOG_MESSAGE_LENGTH - 1] = '\0'; // Ensure null termination

    // Move the write index to the next spot, wrapping around if necessary
    g_log_write_index = (g_log_write_index + 1) % MAX_LOG_ENTRIES;

    // IMPORTANT: If the write index has caught up to the read index, it means
    // we have overwritten an unread message. We must push the read index forward
    // to prevent the GUI from reading the same message twice.
    if (g_log_write_index == g_log_read_index) {
        g_log_read_index = (g_log_read_index + 1) % MAX_LOG_ENTRIES;
    }

    pthread_mutex_unlock(&g_event_log_mutex);
}

// function to clear entries older than LOG_TTL_SECONDS
void clear_old_log_entries(void) {
    pthread_mutex_lock(&g_event_log_mutex);

    time_t now = time(NULL);

    for (int i = 0; i < MAX_LOG_ENTRIES; i++) {
        // Check if the message is valid (timestamp > 0) and old
        if (g_log_entries[i].timestamp > 0 && difftime(now, g_log_entries[i].timestamp) > LOG_TTL_SECONDS) {
            // "Clear" the message by setting its timestamp to 0.
            // We don't need to erase the text, this is faster.
            g_log_entries[i].timestamp = 0;
        }
    }

    pthread_mutex_unlock(&g_event_log_mutex);
}
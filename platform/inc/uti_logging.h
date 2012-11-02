#ifndef __UTI_LOGGING_H__
#define __UTI_LOGGING_H__



#include "platform.h"

//
#ifdef __cplusplus
extern "C" {
#endif //:__cplusplus

#ifdef _DEBUG_LOGGING
#define LOG_LOCATION_FILE	__FILE__
#define LOG_LOCATION_LINE	__LINE__
#else
#define TRACE_LOCATION_FILE	NULL
#define TRACE_LOCATION_LINE	0
#endif


/*--------------------------------------------------------------------*/
//	interface defination
#define LOG_PREFIX_DEBUG            "DEBUG"
#define LOG_PREFIX_INFO             "INFO"
#define LOG_PREFIX_WARNING          "WARNING"
#define LOG_PREFIX_ERROR            "ERROR"
#define LOG_PREFIX_CRITICAL         "CRITICAL"

// log level define 
typedef enum LogLevel_e 
{
    logDebug = 1,
	logInfo ,
	logWarning,
	logError,
	logCritical
}ELogLevel;
// log handle function
typedef int (*log_handler)(ELogLevel, const char* , unsigned int);

#ifdef _DEBUG_LOGGING  /* debug and release control */

/* init log system, should be called before any other functional call ,also you can use the MACRO 
* below, to indirect call this function
*/
void log_sys_init(ELogLevel sys_level, log_handler plog_handler, 
                    char* file_path, char* filename_prefix, size_t file_size);
/* log message maker and call the log handler */
int log_printf(ELogLevel level, const char* file_name, unsigned int line_num, const char *format, ...);
#endif // #_DEBUG_LOGGING

/* store log to local file */
int local_file_log(ELogLevel, const char* , unsigned int);
int file_log_agent(ELogLevel level, const char *log_msg, unsigned int len);
void file_log_close();
int default_log_agent(ELogLevel level, const char * log_msg, unsigned int len);

/* LOG Interface Marco */
#ifdef _DEBUG_LOGGING
#define LOGGING_BASIC_INIT(sys_level, log_agent)  \
    log_sys_init(sys_level, log_agent, NULL, NULL, 0);

#define LOGGING_FILE_INIT(sys_level, file_path, file_prefix, file_size)  \
    log_sys_init(sys_level, file_log_agent, file_path, file_prefix, file_size);
#define LOGGING_FILE_END()  \
    file_log_close();

#define LOGGING_MSG(level, fmt, ...)    \
    log_printf(level, LOG_LOCATION_FILE, LOG_LOCATION_LINE, fmt, ## __VA_ARGS__);
//
#define LOGGING_PRINTF(fmt, ...)    \
    log_printf(logInfo, LOG_LOCATION_FILE, LOG_LOCATION_LINE, fmt, ## __VA_ARGS__);

#define LOGGING_DEBUG(fmt, ...)    \
    log_printf(logDebug, LOG_LOCATION_FILE, LOG_LOCATION_LINE, fmt, ## __VA_ARGS__);

#define LOGGING_INFO(fmt, ...)    \
    log_printf(logInfo, LOG_LOCATION_FILE, LOG_LOCATION_LINE, fmt, ## __VA_ARGS__);

#define LOGGING_WARN(fmt, ...)    \
    log_printf(logWarning, LOG_LOCATION_FILE, LOG_LOCATION_LINE, fmt, ## __VA_ARGS__);

#define LOGGING_ERROR(fmt, ...)    \
    log_printf(logError, LOG_LOCATION_FILE, LOG_LOCATION_LINE, fmt, ## __VA_ARGS__);

#define LOGGING_CRITICAL(fmt, ...)    \
    log_printf(logCritical, LOG_LOCATION_FILE, LOG_LOCATION_LINE, fmt, ## __VA_ARGS__);

#else
#define LOGGING_BASIC_INIT(sys_level, log_agent)       
#define LOGGING_FILE_INIT(sys_level, file_path, file_prefix, file_size) 
#define LOGGING_FILE_END()  
#define LOGGING_MSG(level, fmt, ...)       
//
#define LOGGING_PRINTF(fmt, ...) 
#define LOGGING_DEBUG(fmt, ...)
#define LOGGING_INFO(fmt, ...)        
#define LOGGING_WARN(fmt, ...)        
#define LOGGING_ERROR(fmt, ...)       
#define LOGGING_CRITICAL(fmt, ...)    
#endif // #_DEBUG_LOGGING



/*--------------------------------------------------------------------*/
//
#ifdef __cplusplus
}
#endif //:__cplusplus

#endif //:__UTI_LOGGING_H__


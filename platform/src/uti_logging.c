
#include <unistd.h>

#include "uti_logging.h"
#include "uti_time.h"
#include "uti_file.h"
#include "uti_thread.h"

/*
*   log message -> log handler  -> show/store
*/
// buffer size define 
#define DEBUG_LOG_MAX_TRACE_LEN     (1024)
#define DEBUG_LOG_PREFIX_BUFF_LEN   (100)
#define DEBUG_LOG_TIME_PREFIX       (26)
#define DEBUG_LOG_LINE_NUM_PREFIX   (10)
#define DEBUG_LOG_LEVEL_PREFIX      (8)

#define LOG_FILE_DEFAULT_SIZE   (1024*1024)         /* 1M */
#define LOG_FILE_MAX_SIZE       (1024*1024*1024)    /* 1G */ 
//
#define LOG_COLOR_NUM           9
/* global setting of log module */
#define LOGGING_AGENT_DEFAULT_INIT {1, NULL, {INVALID_FILE_DESC, NULL, NULL, 0}}
//
typedef struct _log_agent_s
{
    ELogLevel     log_level;          /* system log level */
    log_handler     plog_handler;       /* log handler*/
    struct {        
        file_desc_t     file_desc;
        char           *pfile_name_prefix;
        char           *pfile_path;
        size_t          file_size;
    }file_log_setting;
}log_agent_t;

log_agent_t g_DefaultTrace = LOGGING_AGENT_DEFAULT_INIT;

pthread_mutex_t g_LogMutex = PTHREAD_MUTEX_INITIALIZER;
static inline int logging_lock() {
    return pthread_mutex_lock(&g_LogMutex);
}

static inline int logging_try_lock() {
    return pthread_mutex_trylock(&g_LogMutex);
}

static inline int logging_unlock() {
    return pthread_mutex_unlock(&g_LogMutex);
}

static const char *g_log_color[LOG_COLOR_NUM] = {
    "\033[7m\033[31m", // reverse red
    "\033[1m\033[31m", // light red
    "\033[1m\033[33m", // yellow
    
    "\033[1m\033[35m", // magenta
    "\033[31m",        // red
    "\033[36m",        // cyan
    
    "\033[32m",        // green
    "\033[0m",         // normal
    "",                // null
};
#define LOG_COLOR_MODE_ESC  "\033[0m"

static inline const char* log_level_to_color(ELogLevel level)
{
    switch (level)
    {
    case logDebug:
        return g_log_color[7];
        
    case logInfo :
        return g_log_color[6];

    case logWarning:
        return g_log_color[2];

    case logError:
        return g_log_color[1];

    case logCritical:
        return g_log_color[0];

    default:
        //normal 
        return g_log_color[7];
    }
}
/*
 *  
*/
static inline const char* log_level_to_prefix(ELogLevel level)
{
    switch (level)
    {
    case logDebug:
        return LOG_PREFIX_DEBUG;
    case logInfo :
        return LOG_PREFIX_INFO;

    case logWarning:
        return LOG_PREFIX_WARNING;

    case logError:
        return LOG_PREFIX_ERROR;

    case logCritical:
        return LOG_PREFIX_CRITICAL;

    default:
        //error
        return "Unknown level";
    }
}

#ifdef _DEBUG_LOGGING
/* init log system, should be called before any other functional call */
void log_sys_init(ELogLevel sys_level, log_handler plog_handler, 
                    char* file_path, char* filename_prefix, size_t file_size)
{
    /*log system basic setting*/
    g_DefaultTrace.plog_handler = plog_handler;
    g_DefaultTrace.log_level = sys_level;
    /* file log setting */
    g_DefaultTrace.file_log_setting.file_desc = INVALID_FILE_DESC;
    g_DefaultTrace.file_log_setting.pfile_name_prefix = filename_prefix;
    //Note : current path format like "/usr/log_dir" , the last charater should not be '/' or '\\'
    g_DefaultTrace.file_log_setting.pfile_path = file_path;
    /* make log file size (0, 1G], default size 1M*/
    if((0 == file_size)||(file_size>LOG_FILE_MAX_SIZE))
    {
        g_DefaultTrace.file_log_setting.file_size = LOG_FILE_DEFAULT_SIZE;
    }
    else
    {
        g_DefaultTrace.file_log_setting.file_size = file_size;
    }
}

/* log message maker and call the log handler */
int log_printf(ELogLevel level, const char* file_name, unsigned int line_num, const char *format, ...)
{
    int  prefix_len;
    int  message_len;
    int  log_len;
    UtiTime_t cur_time;
    char buff[DEBUG_LOG_MAX_TRACE_LEN + 1];
    va_list ap;

    // if the log level is less than the sys log level, then this log will not logged.
    if(level < g_DefaultTrace.log_level) {
        //system level control
        return 1;
    }

    if((NULL == file_name)||(0 == line_num)||('\0' == *file_name)) {
        //error
        return -1;
    }

    //build prefix
    uti_get_time_t(&cur_time);

    // leave 10 more blank space size
    if(strlen(file_name)>(DEBUG_LOG_PREFIX_BUFF_LEN
                          - DEBUG_LOG_TIME_PREFIX- DEBUG_LOG_LINE_NUM_PREFIX- DEBUG_LOG_LEVEL_PREFIX - 10))
        return -1;

    //   time, file, line, LEVEL prefix, message
    prefix_len = sprintf(buff, "%s%d-%02d-%02d %02d:%02d:%02d.%06d %s:%d [%s:]",
                         log_level_to_color(level),
                         cur_time.year, cur_time.month, cur_time.day,
                         cur_time.hour, cur_time.minute, cur_time.second, cur_time.microsecond,
                         file_name, line_num,
                         log_level_to_prefix(level));

    if(prefix_len <= 0) {
        //error
        return -1;
    }

    //build log message
    va_start(ap, format);
    message_len = vsnprintf(buff + prefix_len, DEBUG_LOG_MAX_TRACE_LEN-prefix_len, format, ap);
    va_end(ap);

    if(message_len <= 0) {
        //error
        return -1;
    }

    //add \n
    log_len = prefix_len + message_len;
    strcpy(buff+log_len, LOG_COLOR_MODE_ESC"\n");
    log_len = strlen(buff);
    logging_lock();

    if (g_DefaultTrace.plog_handler) {      
        g_DefaultTrace.plog_handler(level, buff, log_len);
    } else {
        //default action, print log to screen
        default_log_agent(level, buff, log_len);
    }

    logging_unlock();
    return log_len;
}

#endif //: _DEBUG_LOGGING

/*-------------------------------------------------------------------------------------
*                                   log agent define here                                                                        *
--------------------------------------------------------------------------------------*/

/* store log to local file and make file management
*  single file size and file numbers, file name rules
*/
int local_file_log(ELogLevel level, const char *log_msg, unsigned int len)
{
    //:TODO: implement  local log file management.
    return 1;
}

/* output log to stdout */
#define FILE_NAME_LEN           (255)
int g_ThreadExitFlag = 0;

/* local file manager
*  Create thread to deal with log files *
*/
static inline file_desc_t log_file_open()
{
    int name_len;
    int path_len = 0;
    UtiTime_t cur_time;
    char file_name_buff[FILE_NAME_LEN+1];

    //get current time
    uti_get_time_t(&cur_time);
    //make log file path
    if(g_DefaultTrace.file_log_setting.pfile_path)
    {
        path_len = sprintf(file_name_buff, "%s/", g_DefaultTrace.file_log_setting.pfile_path);
        if(path_len<0)
        {
            return INVALID_FILE_DESC;
        }
    }
    
    //make log file name 
    name_len = snprintf(file_name_buff+path_len, FILE_NAME_LEN, 
            "%s_%d-%02d-%02d_%02d:%02d:%02d_%06d.log",
            (g_DefaultTrace.file_log_setting.pfile_name_prefix) ? 
                g_DefaultTrace.file_log_setting.pfile_name_prefix : "log_file",
            cur_time.year, cur_time.month, cur_time.day, 
            cur_time.hour, cur_time.minute, cur_time.second, cur_time.microsecond);
    if(name_len<0)
    {
        //create file name failed, 
        return INVALID_FILE_DESC;
    }
    
    //create and open file and set file mode 644
    return open(file_name_buff, O_WRONLY|O_CREAT|O_TRUNC, 0644);    
}

/* control the exit of thread */
int file_log_agent(ELogLevel level, const char *log_msg, unsigned int len)
{
    long file_size;
    //
    file_desc_t *pfile_desc = &(g_DefaultTrace.file_log_setting.file_desc);

    if(*pfile_desc < 0)
    {
        /* Open log file and write log */        
        *pfile_desc = log_file_open();    
        if(INVALID_FILE_DESC == *pfile_desc)
        {
            return -1;
        }

        //write log 
        if(-1 == os_write_file_datagram(*pfile_desc, log_msg, len))
        {
            //write failed
            return -1;
        }

        return 1;
    }
    else
    {           
        //check file size 
        file_size = os_stat_file_size(*pfile_desc);
        if(file_size<0)
        {
            //file stat is error, maybe file is deleted
            return -1;
        }

        //control log file size 
        if((unsigned int)file_size>=g_DefaultTrace.file_log_setting.file_size)
        {
            //close file 
            os_close_file(*pfile_desc);
            //open a new log file
            *pfile_desc = log_file_open();
            if(INVALID_FILE_DESC == *pfile_desc)
            {
                return -1;
            }
        }

        //write log 
        if(-1 == os_write_file_datagram(*pfile_desc, log_msg, len))
        {
            //write failed
            return -1;
        }

        return 1;
    }

    //
}

void file_log_close() 
{
    if(g_DefaultTrace.file_log_setting.file_desc != INVALID_FILE_DESC)
    {
        os_close_file(g_DefaultTrace.file_log_setting.file_desc);
        g_DefaultTrace.file_log_setting.file_desc = INVALID_FILE_DESC;
    }
}

int default_log_agent(ELogLevel level, const char* log_msg, unsigned len)
{
    if((0 == len)||(NULL == log_msg))
    {
        return -1;
    }
    //
    fprintf(stderr, "%s\n", log_msg);

    return 1;
}




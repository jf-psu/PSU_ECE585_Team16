#define E_INVALID_OP_CODE -1000

#define LOG_ERROR 0
#define LOG_NORMAL 1
#define LOG_INFO 2
#define LOG_DEBUG 3
#define LOG_NONE 100

int log_level = LOG_NORMAL;


//https://stackoverflow.com/questions/1516370/wrapper-printf-function-that-filters-according-to-user-preferences
#define log(priority,format,args...)        \
                 if (priority <= log_level) { \
                      printf(format, ## args);    \
                  }

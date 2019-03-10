#ifndef LOG_H

#define LOG_H
#define E_INVALID_OP_CODE -1000

#define LOG_NONE -1
#define LOG_ERROR 0
#define LOG_NORMAL 1
#define LOG_INFO 2
#define LOG_DEBUG 3

extern int log_level;
//https://stackoverflow.com/questions/1516370/wrapper-printf-function-that-filters-according-to-user-preferences
#define log(priority,format,args...)        \
                 if (priority <= log_level) { \
                      printf(format, ## args);    \
                  }

#endif /* LOG_H */ 
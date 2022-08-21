#ifndef __USER_IO_H_
#define __USER_IO_H_

#include <string>
#include <iostream>

enum LogLevel {
    Error=3,
    Warning=2,
    Info=1,
    Debug=0,
};

class Logger {
public:
    // logging functions
    void error(std::string);
    void warning(std::string);
    void info(std::string);
    void debug(std::string);

    // configuration options
    void set_log_level(LogLevel level) {_log_level = level;}
    const LogLevel log_level() const {return _log_level;}

private:
    LogLevel _log_level = Info;
};

#endif

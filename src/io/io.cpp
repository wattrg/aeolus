#include "io.h"

void Logger::error(std::string msg){
    std::cout << "Error: " << msg << "\n";
}

void Logger::warning(std::string msg){
    if (_log_level >= Warning) {
        std::cout << "Warning: " << msg << "\n";
    }
}

void Logger::info(std::string msg){
    if (_log_level >= Info){
        std::cout << msg << "\n";
    }
}

void Logger::debug(std::string msg){
    if (_log_level >= Debug){
        std::cout << msg << "\n"; 
    }
}

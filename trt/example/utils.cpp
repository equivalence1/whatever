#include "utils.h"

#include <sstream>
#include <string>
#include <stdexcept>


void ensureCondition(bool cond, const char* condStr, const char* file, uint32_t line, const std::string& msg, bool addMsg) {
    if (cond) {
        return;
    }
    
    std::stringstream ss;
    ss << file << ":" << line << ": Failed condition " << condStr;

    if (addMsg) {
        ss << ", " << msg;
    }

    throw std::runtime_error(ss.str());
}

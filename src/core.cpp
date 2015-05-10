

#include "ant/core/core.hpp"
#include "sys/time.h"

// Symbolic name for visibility("default") attribute.
#define EXPORT __attribute__((visibility("default")))
namespace ant { 
    
    
unsigned GetMillisCount() {
#ifdef WINDOWS
    return GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    return unsigned((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
#endif
}
    
    
std::map<std::string, std::string> command_line_options(const char* argv[], int argc) {
    std::map<std::string, std::string> res;
    for (Index i = 0; i < argc; ++i) {
        if (argv[i][0] == '-') {
            std::string key(argv[i]+1);
            res[key] = "";
            if (i+1 < argc && argv[i+1][0] != '-') {
                res[key] = std::string(argv[i+1]);
                ++i;
            } 
        }
    }
    return res;
} 


int atoi(char* str, Int n) {
    return atoi(str, str+n);
}


int atoi(char* first, char *last) {
    char ch = *last;
    *last = '\0';
    int r = std::atoi(first);
    *last = ch;
    return r;
}
    
}
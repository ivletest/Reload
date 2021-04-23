//
// Created by ivan on 15.4.21.
//

#ifndef RELOAD_STR_H
#define RELOAD_STR_H

#include "Common.h"

class Str {
public:

    static inline bool CompareI(std::string s1, std::string s2) {
        transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
        transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
        return s1 == s2;
    }

    static inline void Replace(std::string & str, const std::string & from, const std::string & to) {
        size_t startPos = str.find(from);
        if(startPos != std::string::npos) {
            str.replace(startPos, from.length(), to);
        }
    }

    static inline void TrimExtension(std::string &str) {
        str = str.substr(0, str.find_last_of('.'));
    }
};
#endif //RELOAD_STR_H

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>

namespace alog
{
    //全局时间
    auto nowTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm* timeInfo = std::localtime(&nowTime);
    
    enum MessageSign {
        aSUCCESS,
        aWARNING,
        aERROR,
        aCRITICAL,
        aFATAL,
        aINFO     
    };

    std::string MessageSignToString(MessageSign flag) {
        if (flag == aSUCCESS) {
            return std::string("\e[32mSUCCESS\e[0m");
        }
        else if (flag == aWARNING) {
            return std::string("\e[33mWARNING\e[0m");
        }
        else if (flag == aERROR) {
            return std::string("\x1b[1m\e[31mERROR\e[0m");
        }
        else if (flag == aCRITICAL) {
            return std::string("\e[36mCRITICAL\e[0m");
        }
        else if (flag == aFATAL) {
            return std::string("\e[31mFATAL\e[0m");
        }
        else if (flag == aINFO) {
            return std::string("\e[37mINFO\e[0m");
        }
        else {
            return std::string("NONE");
        }
    }

    void log(MessageSign level, std::string message) {
        std::string msgFlag = MessageSignToString(level);

        printf("[%02d:%02d:%02d] [%s] \033[1;36manote\e[0m | \033[0;33m%s\e[0m \n",
            timeInfo->tm_hour,
            timeInfo->tm_min,
            timeInfo->tm_sec,
            msgFlag.c_str(),
            message.c_str()
        );
    }
    
} 


#endif
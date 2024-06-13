#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

#include "logger.hpp"
#include "anoteBase.hpp"

using namespace std;

int main(int __paramCount, char* __paramArgs[])
{
    int commandCount = 4;

    if (commandCount > 0) {
        string command = "search";
        if (anote::commandMap.count(command)) {

            anote::Command* cmd = anote::commandMap[command];
            if (commandCount-1 == cmd->size) {
                vector<string> args(cmd->size);
                args[0] = "tag";
                args[1] = ".";
                args[2] = "ËÑË÷";
                cmd->call(args);
            }

            else if (commandCount-1 > cmd->size) {
                alog::log(alog::aERROR,"Too many parameters.");
            }
            else {
                alog::log(alog::aERROR,"Too few parameters.");
            }
        }
        else {
            alog::log(alog::aERROR,"The command does not exist.");
        }
    } 
    else {
        alog::log(alog::aERROR,"Command parameters not specified.");
    }


    return 0;
}
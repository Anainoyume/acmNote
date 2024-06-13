#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

#include "logger.hpp"
#include "anoteBase.hpp"

using namespace std;

int main(int __paramCount, char* __paramArgs[])
{
    int commandCount = 2;

    if (commandCount > 0) {
        string command = "create";
        if (anote::commandMap.count(command)) {

            anote::Command* cmd = anote::commandMap[command];
            if (commandCount-1 == cmd->size) {
                vector<string> args(cmd->size);
                args[0] = "./config.json";
                cmd->call(args);
            }

            else if (commandCount-1 > cmd->size) {
                alog::log(alog::ERROR,"Too many parameters.");
            }
            else {
                alog::log(alog::ERROR,"Too few parameters.");
            }
        }
        else {
            alog::log(alog::ERROR,"The command does not exist.");
        }
    } 
    else {
        alog::log(alog::ERROR,"Command parameters not specified.");
    }


    return 0;
}
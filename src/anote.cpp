#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

#include "logger.hpp"
#include "anoteBase.hpp"

using namespace std;

int main(int __paramCount, char* __paramArgs[])
{
    int commandCount{__paramCount - 1};
    if (commandCount > 0) {
        string command = __paramArgs[1];
        if (anote::commandMap.count(command)) {

            anote::Command* cmd = anote::commandMap[command];
            if (commandCount-1 == cmd->size) {
                vector<string> args(cmd->size);
                for (int i = 2; i < 2+cmd->size; ++i) {
                    args[i-2] = __paramArgs[i];
                }
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
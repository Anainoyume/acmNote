#ifndef ANOTEBASE_H
#define ANOTEBASE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <fstream>
#include <Windows.h>

#include "nlohmann/json.hpp"
#include "logger.hpp"

using namespace std;
using json = nlohmann::json;

namespace fs = filesystem;

namespace anote
{
    struct Command
    {
        size_t size;
        function<void(vector<string>)> call;

        Command(size_t __size, const function<void(const vector<string>&)>& __call)
        : size(__size), call(__call)
        {}
    };

    struct noteEntry
    {
        string title;
        string description;
        string path;
    };

    std::string ConvertGBKToUTF8(const std::string& gbkString) {
        int utf8Length = MultiByteToWideChar(CP_ACP, 0, gbkString.c_str(), -1, NULL, 0);
        wchar_t* wstrBuffer = new wchar_t[utf8Length];
        MultiByteToWideChar(CP_ACP, 0, gbkString.c_str(), -1, wstrBuffer, utf8Length);

        int utf8Length2 = WideCharToMultiByte(CP_UTF8, 0, wstrBuffer, -1, NULL, 0, NULL, NULL);
        char* utf8Buffer = new char[utf8Length2];
        WideCharToMultiByte(CP_UTF8, 0, wstrBuffer, -1, utf8Buffer, utf8Length2, NULL, NULL);

        std::string utf8String(utf8Buffer);

        delete[] wstrBuffer;
        delete[] utf8Buffer;

        return utf8String;
    }

    std::string ConvertUTF8ToGBK(const std::string& utf8String) {
        int wstrLength = MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, NULL, 0);
        wchar_t* wstrBuffer = new wchar_t[wstrLength];
        MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1, wstrBuffer, wstrLength);

        int gbkLength = WideCharToMultiByte(CP_ACP, 0, wstrBuffer, -1, NULL, 0, NULL, NULL);
        char* gbkBuffer = new char[gbkLength];
        WideCharToMultiByte(CP_ACP, 0, wstrBuffer, -1, gbkBuffer, gbkLength, NULL, NULL);

        std::string gbkString(gbkBuffer);

        delete[] wstrBuffer;
        delete[] gbkBuffer;

        return gbkString;
    }

    vector<fs::path> findJsonFiles(const fs::path& directory) {
        vector<fs::path> jsonFiles;

        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                jsonFiles.push_back(entry.path());
            }
        }

        return jsonFiles;
    }

    string simplePath(const string& __path) {
        string path = __path;
        if (path == ".") {
            vector<fs::path> result = findJsonFiles("./");
            if (result.size() > 1) {
                alog::log(alog::aERROR,"There are multiple configuration files in the current folder.");
                exit(1);
            }
            else if (result.size() == 0) {
                alog::log(alog::aERROR,"The current folder does not have a JSON file!");
                exit(1);
            }
            else {
                path = result[0].string();
            }
        }
        return path;
    }

    void createNote(const string& mainPage, const string& noteName, const string& __textPath, const string& __codePath, const string& url, const vector<string>& tags, const string& rating) {
        string textPath = __textPath.empty() ? ("./" + noteName + ".md") : __textPath;
        string codePath = __codePath.empty() ? ("./" + noteName + ".cpp") : __codePath;
        
        string contentText = "none.";
        string contentCode = "none.";
        string Url = url.empty() ? "https://www.luogu.com.cn/problem/list" : url;
        string Rating = rating.empty() ? "<none>" : rating;

        if (!textPath.empty()) {
            ifstream infile(textPath);
            if (!infile.is_open()) {
                alog::log(alog::aERROR,"The file cannot be opened.");
                exit(1);
            }

            stringstream buffer;
            buffer << infile.rdbuf();
            contentText = buffer.str();
            infile.close();
        }

        if (!codePath.empty()) {
            ifstream infile(codePath);
            if (!infile.is_open()) {
                alog::log(alog::aERROR,"The file cannot be opened.");
                exit(1);
            }

            stringstream buffer;
            buffer << infile.rdbuf();
            contentCode = buffer.str();
            infile.close();
        }

        string newContent = "#### [\u25c0 \u8fd4\u56de\u76ee\u5f55](./../" + mainPage + ".md)\n---\n"
                          + "**\u9898\u76ee\u6765\u6e90: [\u70b9\u6211\u4f20\u9001](" + Url + ")**\n"
                          + "**rating: " + Rating + "**\n"
                          + "**Tags:**";
        if (tags.empty()) {
            newContent += "`\u6682\u65e0`\n";
        }
        else {
            for (auto&& _tag : tags) {
                string tag = _tag;
                newContent += "`" + tag + "` ";
            }
            newContent += "\n";
        }

        newContent += "\n---\n\n"
                   + contentText
                   + "\n\n\u4ee3\u7801\u5982\u4e0b:\n"
                   + "```cpp\n"
                   + contentCode
                   + "\n```\n";

        fs::path dirPath("./noteConfig");
        if (!fs::exists(dirPath)) create_directories(dirPath);
        
        ofstream outfile("./noteConfig/" + noteName + ".md");
        if (!outfile.is_open()) {
            alog::log(alog::aERROR,"The file cannot be opened.");
            exit(1);
        }

        outfile << newContent;
        outfile.close();
    }

    void create(const vector<string>& args) {
        string path = simplePath(args[0]);
        fs::remove_all("./noteConfig");
        if (!fs::exists(path)) {
            alog::log(alog::aERROR,"The JSON file path does not exist!");
            exit(1);
            return;
        }

        ifstream infile(path);
        if (!infile.is_open()) {
            alog::log(alog::aERROR,"The file cannot be opened!");
            exit(1);
            return;
        }
        else {
            json metaData = json::parse(infile);
            string mainPage = metaData["name"];
            string title = metaData["title"];

            fs::remove("./"+mainPage+".md");

            ofstream outfile("./" + mainPage + ".md");
            string outContent = "# " + title + "\n\n";

            for (auto&& pages : metaData["contents"]) {
                string pageTitle = pages.count("title") ? pages["title"] : "";
                string des = pages.count("description") ? pages["description"] : "";
                string name = pages.count("name") ? pages["name"] : "";
                string Url = pages.count("url") ? pages["url"] : "";
                string text = pages.count("text") ? pages["text"] : "";
                string code = pages.count("code") ? pages["code"] : "";
                string rating = pages.count("rating") ? pages["rating"] : "";
                vector<string> tags; if (pages.count("tags")) { for (auto&& tag : pages["tags"]) { tags.push_back((string)tag); } }
                createNote(mainPage,name,text,code,Url,tags,rating);

                outContent += "#### [" + pageTitle + "](./noteConfig/" + name + ".md)\n"
                           + des + "\n**tag:**";
                
                if (tags.empty()) {
                    outContent += "`\u6682\u65e0`\n\n";
                }
                else {
                    for (auto&& _tag : tags) {
                        string tag = _tag;
                        outContent += "`" + tag + "` ";
                    }
                    outContent += "\n\n";
                }
            }

            outfile << outContent;
            outfile.close();

            alog::log(alog::aSUCCESS,"Successfully generated notes!");

        }
    }
    
    void search(const vector<string>& args) {
        string subCommand = args[0];
        string path = simplePath(args[1]);
        string target = ConvertGBKToUTF8(args[2]);

        if (subCommand == "tag") {
            fs::path searchPath = path;
            if (fs::exists(searchPath)) {
                if (fs::is_regular_file(searchPath) && (searchPath.extension() == ".json")) {
                    ifstream infile(path);
                    json metaData = json::parse(infile);
                    vector<noteEntry> result;
                    for (auto&& entry : metaData["contents"]) {
                        if (entry.count("tags")) {
                            for (auto&& __tag : entry["tags"]) {
                                string tag = __tag;
                                if (tag == target) {
                                    result.push_back({entry["title"],entry["description"],"\033[1;34m" + fs::current_path().string() + "\\noteConfig\\" + string(entry["name"]) + ".md\033[0m"});
                                    break;
                                }
                            }
                        }
                    }
                    if (result.empty()) {
                        alog::log(alog::aINFO,"No matching files found.");
                    }
                    else {
                        alog::log(alog::aSUCCESS,"Successfully found " + to_string(result.size()) + " matching files:\n");
                        for (auto&& [title, des, path] : result) {
                            title = ConvertUTF8ToGBK(title);
                            des = ConvertUTF8ToGBK(des);
                            path = ConvertUTF8ToGBK(path);
                            cout << title << "\n" << des << "\n" << path << "\n" << endl;
                        }
                    }
                }
                else {
                    alog::log(alog::aERROR,"The path is not a JSON file!");
                    exit(1);
                }
            }
            else {
                alog::log(alog::aERROR,"The path does not exist!");
                exit(1);
            }
        }
        else {
            alog::log(alog::aERROR,"The parameter does not exist under the search command.");
            exit(1);
        }
    }

    
    
    unordered_map<string,Command*> commandMap = {
        {"create",new Command(1,create)},
        {"search",new Command(3,search)}
    };
} 


#endif
#ifndef ANOTEBASE_H
#define ANOTEBASE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <fstream>

#include "nlohmann/json.hpp"
#include "logger.hpp"

using namespace std;
using json = nlohmann::json;

namespace fs = std::filesystem;

namespace anote
{
    void createNote(const string& mainPage, const string& noteName, const string& textPath, const string& codePath, const string& url, const vector<string>& tags, const string& rating) {
        string contentText = "none.";
        string contentCode = "none.";
        string Url = url.empty() ? "https://www.luogu.com.cn/problem/list" : url;
        string Rating = rating.empty() ? "<none>" : rating;

        if (!textPath.empty()) {
            ifstream infile(textPath);
            if (!infile.is_open()) {
                alog::log(alog::ERROR,"The file cannot be opened.");
            }

            stringstream buffer;
            buffer << infile.rdbuf();
            contentText = buffer.str();
            infile.close();
        }

        if (!codePath.empty()) {
            ifstream infile(codePath);
            if (!infile.is_open()) {
                alog::log(alog::ERROR,"The file cannot be opened.");
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
            alog::log(alog::ERROR,"The file cannot be opened.");
        }

        outfile << newContent;
        outfile.close();
    }


    void create(const vector<string>& args) {
        string path = args[0];
        fs::remove_all("./noteConfig");
        if (!fs::exists(path)) {
            alog::log(alog::ERROR,"The JSON file path does not exist!");
            return;
        }

        ifstream infile(path);
        if (!infile.is_open()) {
            alog::log(alog::ERROR,"The file cannot be opened!");
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

                outContent += "### [" + pageTitle + "](./noteConfig/" + name + ".md)\n"
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

        }
    }


    struct Command
    {
        size_t size;
        function<void(vector<string>)> call;

        Command(size_t __size, const function<void(const vector<string>&)>& __call)
        : size(__size), call(__call)
        {}
    };
    
    unordered_map<string,Command*> commandMap = {
        {"create",new Command(1,create)}
    };
} 


#endif
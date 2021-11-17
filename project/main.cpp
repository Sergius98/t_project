#include <iostream>
#include <string>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <cassert>

#include <algorithm> 

bool registerUser(std::string args){
    std::cout << "inside registerUser()" << std::endl;
    std::cout << "args = " << args << std::endl;

    return true;
}
bool unregisterUser(std::string args){
    std::cout << "inside unregisterUser()" << std::endl;
    std::cout << "args = " << args << std::endl;

    return true;
}
bool setName(std::string args){
    std::cout << "inside setName()" << std::endl;
    std::cout << "args = " << args << std::endl;

    return true;
}
bool callUser(std::string args){
    std::cout << "inside callUser()" << std::endl;
    std::cout << "args = " << args << std::endl;

    return true;
}
bool callEnd(std::string args){
    std::cout << "inside callEnd()" << std::endl;
    std::cout << "args = " << args << std::endl;

    return true;
}
bool answer(std::string args){
    std::cout << "inside answer()" << std::endl;
    std::cout << "args = " << args << std::endl;

    return true;
}
bool reject(std::string args){
    std::cout << "inside reject()" << std::endl;
    std::cout << "args = " << args << std::endl;

    return true;
}


typedef bool (*CommandFunctionType)(std::string); 

struct UserInterface{
    std::map<std::string,CommandFunctionType> commands;

    void insert(std::string command_key, CommandFunctionType function){
        commands.insert(std::make_pair(command_key, function));
    }

    bool searchAndCall(std::string command_key, std::string args){
        auto mapIter = commands.find(command_key);
        /*chk if not end*/
        if  (mapIter != commands.end()){
            auto mapFun = mapIter->second;

            return mapFun(args);
        } else {
            std::cout << "CMD not found" << std::endl;
            return false;
        }
    }
};

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}
// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (copy)
static inline std::string trim(std::string s) {
    ltrim(s);
    rtrim(s);
    return s;
}

int main(){
    UserInterface ui;
    ui.insert("register", registerUser);
    ui.insert("unregister", unregisterUser);
    ui.insert("setName", setName);
    ui.insert("call", callUser);
    ui.insert("callEnd", callEnd);
    ui.insert("answer", answer);
    ui.insert("reject", reject);

    bool ex_flag = true;
    std::string command, args;
    while (ex_flag){
        bool success_flag = false;

        std::cout << ">";
        std::cin >> command;
        if (command == "exit") {
            ex_flag = false;
        } else {
            getline(std::cin, args);
            success_flag = ui.searchAndCall(command, trim(args));
            if (success_flag == false) {
                std::cout << "CMD failed to execute" << std::endl;
            }
        }
    }
    return 0;
}
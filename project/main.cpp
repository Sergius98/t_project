#include <iostream>
#include <string>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <cassert>

#include <algorithm> 

#include <vector> 

#include "MobileClient.hpp"
#include "NetConfAgent.hpp"
//#include "PrintInterface.hpp"
//#include "StringInterface.hpp"


//using stringInterface::StrInt;
//using printInterface::PrInt;

mobileClient::MobileClient client;


std::vector<std::string> splitArgs (std::string raw_args){
    std::vector<std::string> args_vector;
    std::istringstream ss(raw_args);
    std::string word;
    while (ss >> word){
        args_vector.push_back(word);
    }
    return args_vector;
}


bool getArgs(long unsigned int num, std::string raw_args, std::vector<std::string> *args = nullptr){
    if ((args == nullptr)&&(num != 0)){
        PrInt::logln("function expects a vector for the requested arguments");
        throw "you need to pass a container for the args";
    }
    std::vector<std::string> args_vector = splitArgs(raw_args);
    if (args_vector.size() != num) {
        PrInt::println(StrInt::format("function expects {} args, but only {} was passed", {num, args_vector.size()}));
        return false;
    }
    if (args != nullptr){
        *args = args_vector;
    }
    return true;
}


// register number
bool cmdRegister(std::string raw_args){
    PrInt::logln("inside cmdRegister()");
    std::vector<std::string> args_vector;
    if (getArgs(2, raw_args, &args_vector) == false){
        return false;
    }
    client.setName(args_vector[1]);
    client.reg(args_vector[0]);

    return true;
}

// unregister
bool unregister(std::string raw_args){
    PrInt::logln("inside unregister()");
    if (getArgs(0, raw_args) == false){
        return false;
    }
    client.unReg();
    return true;
}
// setName name
bool setName(std::string raw_args){
    PrInt::logln("inside setName()");
    std::vector<std::string> args_vector;
    if (getArgs(1, raw_args, &args_vector) == false){
        return false;
    }
    client.setName(args_vector[0]);

    return true;
}

// call number
bool call(std::string raw_args){
    PrInt::logln("inside call()");

    std::vector<std::string> args_vector;
    if (getArgs(1, raw_args, &args_vector) == false){
        return false;
    }

    if (!client.call(args_vector[0])){
        return false;
    }

    PrInt::println("the number is called");

    return true;
}

// callEnd
bool callEnd(std::string raw_args){
    PrInt::logln("inside callEnd()");
    if (getArgs(0, raw_args) == false){
        return false;
    }

    if (!client.endCall()){
        return false;
    }

    PrInt::println("the call is ended");

    return true;
}

// answer
bool answer(std::string raw_args){
    PrInt::logln("inside answer()");
    if (getArgs(0, raw_args) == false){
        return false;
    }


    if (!client.answer()){
        return false;
    }

    PrInt::println("the call is answered");

    return true;
}

// reject
bool reject(std::string raw_args){
    PrInt::logln("inside reject()");
    if (getArgs(0, raw_args) == false){
        return false;
    }

    if (!client.reject()){
        return false;
    }

    PrInt::println("the call is rejected");

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
        if  (mapIter != commands.end()){
            auto mapFun = mapIter->second;

            return mapFun(args);
        } else {
            PrInt::println("CMD not found");
            return false;
        }
    }
};

int main(){

    PrInt::setLog(true);


    UserInterface ui;
    ui.insert("register", cmdRegister);
    ui.insert("unregister", unregister);
    ui.insert("setName", setName);
    ui.insert("call", call);
    ui.insert("callEnd", callEnd);
    ui.insert("answer", answer);
    ui.insert("reject", reject);


    bool ex_flag = true;
    std::string command, args;
    while (ex_flag){
        bool success_flag = false;

        PrInt::printInputPointer();
        std::cin >> command;
        if (command == "exit") {
            ex_flag = false;
        } else if (command == "log") {
            std::cin >> command;
            if (command == "on"){
                PrInt::setLog(true);
                PrInt::println("logs turned on");
            } else if (command == "off"){
                PrInt::setLog(false);
                PrInt::println("logs turned off");
            }
        } else {
            getline(std::cin, args);
            success_flag = ui.searchAndCall(command, args);
            if (success_flag == false) {
                PrInt::println("CMD failed to execute");
            }
        }
    }
    return 0;
}
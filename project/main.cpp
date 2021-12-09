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
#include "PrintInterface.hpp"
#include "StringInterface.hpp"


extern PrintInterface prInt;
extern StringInterface strInt;

MobileCli::MobileClient client;


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
        prInt.logln("function expects a vector for the requested arguments");
        throw "you need to pass a container for the args";
    }
    std::vector<std::string> args_vector = splitArgs(raw_args);
    if (args_vector.size() != num) {
        prInt.println(strInt.format("function expects {} args, but only {} was passed", {num, args_vector.size()}));
        //std::cout << "function expects " << num << 
        //    "args, but only " << args_vector.size() << " was passed" << std::endl;
        return false;
    }
    if (args != nullptr){
        *args = args_vector;
    }
    return true;
}


// register number
bool cmdRegister(std::string raw_args){
    prInt.logln("inside cmdRegister()");
    std::vector<std::string> args_vector;
    if (getArgs(2, raw_args, &args_vector) == false){
        return false;
    }
    client.setName(args_vector[1]);
    client.reg(args_vector[0]);
    //agent.subscribeForModelChanges();

    return true;
}

// unregister
bool unregister(std::string raw_args){
    prInt.logln("inside unregister()");
    if (getArgs(0, raw_args) == false){
        return false;
    }
    client.unReg();
    return true;
}
// setName name
bool setName(std::string raw_args){
    prInt.logln("inside setName()");
    std::vector<std::string> args_vector;
    if (getArgs(1, raw_args, &args_vector) == false){
        return false;
    }
    client.setName(args_vector[0]);

    return true;
}

// call number
bool call(std::string raw_args){
    prInt.logln("inside call()");

    std::vector<std::string> args_vector;
    if (getArgs(1, raw_args, &args_vector) == false){
        return false;
    }

    if (!client.call(args_vector[0])){
        return false;
    }

    prInt.println("the number is called");

    return true;
}

// callEnd
bool callEnd(std::string raw_args){
    prInt.logln("inside callEnd()");
    if (getArgs(0, raw_args) == false){
        return false;
    }

    if (!client.endCall()){
        return false;
    }

    prInt.println("the call is ended");

    return true;
}

// answer
bool answer(std::string raw_args){
    prInt.logln("inside answer()");
    if (getArgs(0, raw_args) == false){
        return false;
    }


    if (!client.answer()){
        return false;
    }

    prInt.println("the call is answered");

    return true;
}

// reject
bool reject(std::string raw_args){
    prInt.logln("inside reject()");
    if (getArgs(0, raw_args) == false){
        return false;
    }

    if (!client.reject()){
        return false;
    }

    prInt.println("the call is rejected");

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
            prInt.println("CMD not found");
            //std::cout << "CMD not found" << std::endl;
            return false;
        }
    }
};

/******/
//delete later
bool fetchData(std::string raw_args){
    std::vector<std::string> args_vector;
    std::string value;
    if (getArgs(1, raw_args, &args_vector) == false){
        prInt.println("wrong number of arguments");
        return false;
    } else {
        client.fetchData(args_vector[0], value); 
        prInt.logln({"value:", value});
    }
    return true;
}
bool changeData(std::string raw_args){
    std::vector<std::string> args_vector;
    std::string value;
    if (getArgs(2, raw_args, &args_vector) == false){
        prInt.println("wrong number of arguments");
        return false;
    } else {
        client.changeData(args_vector[0], args_vector[1]); 
        client.fetchData(args_vector[0], value); 
        prInt.logln({"value:", value});
    }
    return true;
}
/******/
int main(){

    prInt.setLog(true);


    UserInterface ui;
    ui.insert("register", cmdRegister);
    ui.insert("unregister", unregister);
    ui.insert("setName", setName);
    ui.insert("call", call);
    ui.insert("callEnd", callEnd);
    ui.insert("answer", answer);
    ui.insert("reject", reject);


    /******/
    //delete later
    ui.insert("fetch", fetchData);
    ui.insert("change", changeData);
    /******/



    bool ex_flag = true;
    std::string command, args;
    while (ex_flag){
        bool success_flag = false;

        prInt.printInputPointer();
        //std::cout << ">";
        std::cin >> command;
        if (command == "exit") {
            ex_flag = false;
        } else if (command == "log") {
            std::cin >> command;
            if (command == "on"){
                prInt.setLog(true);
            } else if (command == "off"){
                prInt.setLog(false);
            }
        } else {
            getline(std::cin, args);
            success_flag = ui.searchAndCall(command, args);
            if (success_flag == false) {
                prInt.println("CMD failed to execute");
                //std::cout << "CMD failed to execute" << std::endl;
            }
        }
    }
    return 0;
}
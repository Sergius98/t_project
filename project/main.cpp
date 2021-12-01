#include <iostream>
#include <string>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <cassert>

#include <algorithm> 

#include <vector> 

#include "NetConfAgent.hpp"
#include "PrintInterface.hpp"
#include "StringInterface.hpp"


StringInterface si;
PrintInterface pi;
NetConfAgent agent(pi);


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
    std::vector<std::string> args_vector = splitArgs(raw_args);
    if (args_vector.size() != num) {
        pi.println(si.format("function expects {} args, but only {} was passed", {num, args_vector.size()}));
        //std::cout << "function expects " << num << 
        //    "args, but only " << args_vector.size() << " was passed" << std::endl;
        return false;
    }
    if (args != nullptr){
        *args = args_vector;
    }
    return true;
}


// register name number
bool cmdRegister(std::string raw_args){
    std::vector<std::string> args_vector;
    std::cout << "inside cmdRegister()" << std::endl;
    if (getArgs(2, raw_args, &args_vector) == false){
        std::cout << "raw_args =>>" << raw_args << "<<" << std::endl;
        std::cout << "wrong number of arguments" << std::endl;
        return false;
    }
    std::cout << "args_vector:" << std::endl;
    for (auto arg: args_vector)
        std::cout << ">>" << arg << "<<" << std::endl;

    agent.subscribeForModelChanges();

    return true;
}

// unregister
bool unregister(std::string raw_args){
    std::cout << "inside unregister()" << std::endl;
    if (getArgs(0, raw_args) == false){
        std::cout << "wrong number of arguments" << std::endl;
        return false;
    }
    return true;
}
// setName name
bool setName(std::string raw_args){
    std::vector<std::string> args_vector;
    std::cout << "inside setName()" << std::endl;
    if (getArgs(1, raw_args, &args_vector) == false){
        std::cout << "wrong number of arguments" << std::endl;
        return false;
    }

    return true;
}

// call number
bool call(std::string raw_args){
    std::vector<std::string> args_vector;
    std::cout << "inside call()" << std::endl;
    if (getArgs(1, raw_args, &args_vector) == false){
        std::cout << "wrong number of arguments" << std::endl;
        return false;
    }

    if (args_vector[0] == "aaa"){
        args_vector[0] = "/testmodel:sports/person[name='Mike']/name";
    }
    std::string str;
    agent.fetchData(args_vector[0], str);
    std::cout << "str = " << str << std::endl;

    return true;
}

// callEnd
bool callEnd(std::string raw_args){
    std::cout << "inside callEnd()" << std::endl;
    if (getArgs(0, raw_args) == false){
        std::cout << "wrong number of arguments" << std::endl;
        return false;
    }

    return true;
}

// answer
bool answer(std::string raw_args){
    std::cout << "inside answer()" << std::endl;
    if (getArgs(0, raw_args) == false){
        std::cout << "wrong number of arguments" << std::endl;
        return false;
    }

    return true;
}

// reject
bool reject(std::string raw_args){
    std::cout << "inside reject()" << std::endl;
    if (getArgs(0, raw_args) == false){
        std::cout << "wrong number of arguments" << std::endl;
        return false;
    }

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

bool regOpData(std::string raw_args){
    std::vector<std::string> args_vector;
    if (getArgs(3, raw_args, &args_vector) == false){
        if (getArgs(2, raw_args, &args_vector) == false){
            std::cout << "wrong number of arguments" << std::endl;
            return false;
        } else {
            agent.registerOperData("commutator", args_vector[0], args_vector[0], args_vector[1]);
        }
    } else {
        agent.registerOperData("commutator", args_vector[0], args_vector[1], args_vector[2]); 
    }
    std::cout<<"Success"<<std::endl;
    return true;
}
bool fetchData(std::string raw_args){
    std::vector<std::string> args_vector;
    std::string value;
    if (getArgs(1, raw_args, &args_vector) == false){
        std::cout << "wrong number of arguments" << std::endl;
        return false;
    } else {
        agent.fetchData(args_vector[0], value); 
        std::cout << "value: " << value << std::endl;
    }
    return true;
}
bool changeData(std::string raw_args){
    std::vector<std::string> args_vector;
    std::string value;
    if (getArgs(2, raw_args, &args_vector) == false){
        std::cout << "wrong number of arguments" << std::endl;
        return false;
    } else {
        agent.changeData(args_vector[0], args_vector[1]); 
        std::cout << "value: " << value << std::endl;
    }
    return true;
}
int main(){
    UserInterface ui;
    ui.insert("register", cmdRegister);
    ui.insert("unregister", unregister);
    ui.insert("setName", setName);
    ui.insert("call", call);
    ui.insert("callEnd", callEnd);
    ui.insert("answer", answer);
    ui.insert("reject", reject);


    ui.insert("reg", regOpData);
    ui.insert("fetch", fetchData);
    ui.insert("change", changeData);



    bool ex_flag = true;
    std::string command, args;
    while (ex_flag){
        bool success_flag = false;

        pi.printInputPointer();
        //std::cout << ">";
        std::cin >> command;
        if (command == "exit") {
            ex_flag = false;
        } else {
            getline(std::cin, args);
            success_flag = ui.searchAndCall(command, args);
            if (success_flag == false) {
                pi.println("CMD failed to execute");
                //std::cout << "CMD failed to execute" << std::endl;
            }
        }
    }
    return 0;
}
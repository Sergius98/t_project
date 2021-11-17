#include <iostream>
#include <string>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <cassert>

bool register_user(std::string args){
    std::cout << "inside register_user()" << std::endl;
    std::cout << "args = " << args << std::endl;

    return true;
}
bool unregister_user(std::string args){
    std::cout << "inside unregister_user()" << std::endl;
    std::cout << "args = " << args << std::endl;

    return true;
}
bool setName(std::string args){
    std::cout << "inside setName()" << std::endl;
    std::cout << "args = " << args << std::endl;

    return true;
}
bool call_user(std::string args){
    std::cout << "inside call_user()" << std::endl;
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


typedef void (*voidFunctionType)(void); 

struct UserInterface{
    std::map<std::string,std::pair<voidFunctionType,std::type_index>> commands;

    template<typename T>
    void insert(std::string command_key, T function){
        auto tt = std::type_index(typeid(function));
        commands.insert(std::make_pair(command_key,
                        std::make_pair((voidFunctionType)function, tt)));
    }

    template<typename T>
    T searchAndCall(std::string command_key, std::string args){
        auto mapIter = commands.find(command_key);
        /*chk if not end*/
        if  (mapIter != commands.end()){
            auto mapVal = mapIter->second;

            // auto typeCastedFun = reinterpret_cast<T(*)(Args ...)>(mapVal.first); 
            auto typeCastedFun = (T(*)(std::string))(mapVal.first); 

            //compare the types is equal or not
            assert(mapVal.second == std::type_index(typeid(typeCastedFun)));
            return typeCastedFun(args);
        } else {
            std::cout << "CMD not found" << std::endl;
            return 0;
        }
    }
};


int main(){
    UserInterface ui;
    ui.insert("register", register_user);
    ui.insert("unregister", unregister_user);
    ui.insert("setName", setName);
    ui.insert("call", call_user);
    ui.insert("callEnd", callEnd);
    ui.insert("answer", answer);
    ui.insert("reject", reject);

    bool ex_flag = true;
    std::string command, args;
    while (ex_flag){
        std::cout << ">";
        std::cin >> command;
        if (command == "exit") {
            ex_flag = false;
        } else {
            getline(std::cin, args);
            ui.searchAndCall<bool>(command, args);
        }
    }
    return 0;
}
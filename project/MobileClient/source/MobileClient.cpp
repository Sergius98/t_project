#include "MobileClient.hpp"
#include "NetConfAgent.hpp"


/*****/
// delete later
bool MobileClient::fetchData(std::string path, std::string &str){
    return _agent->fetchData(path, str);
}
bool MobileClient::changeData(std::string path, std::string value){
    return _agent->changeData(path, value);
}
/*****/


MobileClient::MobileClient() {
    _agent = std::make_unique<NetConfAgent>();
}


void MobileClient::setName(std::string name) {
    _name = name;
}

void MobileClient::setState(State state) {
    _state = state;
}

std::string MobileClient::getName() {
    return _name;
}

bool MobileClient::reg(std::string number) {
    std::string path = makePath(number, Leaf::number);
    std::string empty = "";

    if (!(_agent->fetchData(path, empty))){
        _number = number;
        // todo: create a file lock with the number as name
        _agent->changeData(path, _number);
        _agent->subscribeForModelChanges(makePath(_number, Leaf::none), this);
        setState(State::idleReg);
        return true;
    }
    prInt.print({"Number ",number," is already taken"});
    return false;
}

std::string MobileClient::makePath(std::string key, Leaf leaf) {
    std::string path = strInt.format(leafPathPattern, {module_name, container_path, key_name, key, leafs.find(leaf)->second});
    prInt.logln(path);
    return path;
}

void MobileClient::handleModuleChange(std::string path, std::string value){
    prInt.logln({"hundle: ",path ," -> ", value});
}
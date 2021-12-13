#include "MobileClient.hpp"
#include "NetConfAgent.hpp"

namespace MobileCli{


MobileClient::MobileClient() {
    _agent = std::make_unique<NetConfAgent>();
}

MobileClient::~MobileClient() {
    if (_state == State::active){
        std::string destinationStatePath = makePath(_routingNumber, Leaf::state);
        std::string destinationIncomingNumberPath = makePath(_routingNumber, Leaf::incomingNumber);
        std::string idleState = states.find(State::idle)->second;
        if (_state == State::active){
            _agent->changeData(destinationIncomingNumberPath, "");
            _agent->changeData(destinationStatePath, idleState);
            _agent->changeData(makePath(_number, Leaf::state), idleState);
            _state = State::idleReg;
            _routingNumber = "";
        }
    }
    if (_state == State::busy){
        endCall();
    }
    if (_state == State::activeIncoming){
        reject();
    }
    if (_state==State::idleReg){
        unReg();
    }
    _agent.reset();
}

void MobileClient::setName(std::string name) {
    _name = name;
}

void MobileClient::handleOperData(std::string &name) {
    name = _name;
}

bool MobileClient::reg(std::string number) {
    std::string path = makePath(number, Leaf::number);
    std::string empty = "";

    if (_state==State::idle){
        if (!(_agent->fetchData(path, empty))){
            _number = number;
            _agent->changeData(path, _number);
            _agent->subscribeForModelChanges(makePath(_number, Leaf::none), moduleName, this);
            _namePath = makePath(_number, Leaf::userName);
            _agent->registerOperData(_namePath, moduleName, this);
            _state = State::idleReg;
            return true;
        }
        prInt.println({"Number ",number," is already taken"});
    } else {
        prInt.println("You are already registered");
    }
    return false;
}
bool MobileClient::unReg() {
    std::string path = makePath(_number, Leaf::none);

    if (_state==State::idleReg){
        _number = "";
        _agent->closeSysrepo();
        _agent->deleteData(path);
        _state = State::idle;
        return true;
    } else {
        prInt.println("You are not idle or not registered");
    }
    return false;
}

bool MobileClient::call(std::string destination_number) {
    std::string sourceStatePath = makePath(_number, Leaf::state);
    std::string sourceIncomingNumberPath = makePath(_number, Leaf::incomingNumber);
    std::string destinationStatePath = makePath(destination_number, Leaf::state);
    std::string destinationIncomingNumberPath = makePath(destination_number, Leaf::incomingNumber);
    std::string destinationState = "";
    std::string idleState = states.find(State::idle)->second;
    std::string activeState = states.find(State::active)->second;
    std::string routingName = "";

    if (_state == State::idleReg){
        if (_agent->fetchData(destinationStatePath, destinationState)){
            if (destinationState == idleState){
                _routingNumber = destination_number;
                _agent->changeData(sourceStatePath, activeState);
                _agent->changeData(destinationStatePath, activeState);
                _agent->changeData(destinationIncomingNumberPath, _number);

                _agent->fetchData(makePath(_routingNumber, Leaf::userName), routingName);
                prInt.println({"\nyou have called ", routingName});
                return true;
            } else {
                prInt.println("the destination is not idle");
            }
        } else {
            prInt.println("the number requested does not exist");
        }
    } else {
        prInt.println({"you need to be idle and registered, but your state is: ", states.find(_state)->second});
    }
    return false;
}

bool MobileClient::answer() {
    std::string sourceStatePath = makePath(_number, Leaf::state);
    std::string destinationStatePath = "";
    std::string busyState = states.find(State::busy)->second;

    if (_state == State::activeIncoming){
        destinationStatePath = makePath(_routingNumber, Leaf::state);
            
        _agent->changeData(destinationStatePath, busyState);
        _agent->changeData(sourceStatePath, busyState);
        _state = State::busy;
        return true;
    } else {
        prInt.println({"you don't have an incoming call, but your state is: ", states.find(_state)->second});
    }
    return false;
}

bool MobileClient::reject() {
    std::string sourceStatePath = makePath(_number, Leaf::state);
    std::string sourceIncomingNumberPath = makePath(_number, Leaf::incomingNumber);
    std::string idleState = states.find(State::idle)->second;

    if (_state == State::activeIncoming){
        std::string destinationStatePath = makePath(_routingNumber, Leaf::state);
            
        _agent->changeData(destinationStatePath, idleState);
        _agent->changeData(sourceIncomingNumberPath, "");
        _agent->changeData(sourceStatePath, idleState);
        _state = State::idleReg;
        _routingNumber = "";
        return true;
    } else {
        prInt.println({"you don't have an incoming call, but your state is: ", states.find(_state)->second});
    }
    return false;
}


bool MobileClient::endCall() {
    std::string sourceStatePath = makePath(_number, Leaf::state);
    std::string destinationStatePath = "";
    std::string destinatonIncomingNumber = "";
    std::string idleState = states.find(State::idle)->second;

    if (_state == State::busy){
        if (_routingNumber!=""){
            destinationStatePath = makePath(_routingNumber, Leaf::state);
            destinatonIncomingNumber = makePath(_routingNumber, Leaf::incomingNumber);
            
            _agent->changeData(destinatonIncomingNumber, "");
            _agent->changeData(destinationStatePath, idleState);
            _agent->changeData(sourceStatePath, idleState);
            _state = State::idleReg;
            _routingNumber = "";
            return true;
        } else {
            prInt.println("you don't have an ongoing call, but your state is busy");
        }
    } else {
        prInt.println({"you need to be busy to end a call, but your state is: ", states.find(_state)->second});
    }
    return false;
}

std::string MobileClient::makePath(std::string key, Leaf leaf) {
    std::string path = strInt.format(leafPathPattern, {moduleName, containerPath, keyName, key, leafs.find(leaf)->second});
    return path;
}

void MobileClient::handleModuleChange(std::string path, std::string value){
    std::string statePath = makePath(_number, Leaf::state);
    std::string incomingNumberPath = makePath(_number, Leaf::incomingNumber);
    std::string idleState = states.find(State::idle)->second;
    std::string activeState = states.find(State::active)->second;
    std::string busyState = states.find(State::busy)->second;
    
    prInt.logln({"handle: ",path ," -> ", value});

    if (path == statePath){
        if (value == idleState){
            _state = State::idleReg;
            prInt.println("idle...");
        } else if (value == activeState){
            if (_state != State::activeIncoming){
                _state = State::active;
            }
            prInt.println("active...");
        } else if (value == busyState){
            _state = State::busy;
            prInt.println("busy...");
        } else {
            prInt.println({"unexpected state value: ", value});
        }
    } else if (path == incomingNumberPath){
        if (value != ""){
            _routingNumber = value;
            _state = State::activeIncoming;
            std::string incomingName = "";
            _agent->fetchData(makePath(_routingNumber, Leaf::userName), incomingName);
            prInt.println({"you have an incloming call from user ", incomingName, " with number ", _routingNumber});
        } else {
            _routingNumber = "";
            prInt.logln("incomingNumber value is made empty");
        }

    } else {
        prInt.logln({"unexpected path", path});
    }

    prInt.logln("exit handleModuleChange");
}

}
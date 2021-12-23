#include "MobileClient.hpp"
#include "NetConfAgent.hpp"

namespace mobileClient{


MobileClient::MobileClient(): 
    MobileClient(std::make_unique<NetConfAgent>()) {}
        
MobileClient::MobileClient(std::unique_ptr<INetConfAgent> agent): 
    _agent{std::move(agent)} {}

MobileClient::~MobileClient() {
    if (_state == State::active){
        const std::string destinationStatePath = makePath(_routingNumber, Leaf::state);
        const std::string destinationIncomingNumberPath = makePath(_routingNumber, 
                                                                   Leaf::incomingNumber);
        const std::string idleState = states.find(State::idle)->second;
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

void MobileClient::setName(const std::string &name) {
    _name = name;
}

void MobileClient::handleOperData(std::string &name) {
    name = _name;
}

bool MobileClient::reg(const std::string &number) {
    const std::string path = makePath(number, Leaf::number);
    std::string empty = "";

    if (_state==State::idle){
        if (!(_agent->fetchData(path, empty))){
            _number = number;
            if (_agent->changeData(path, _number)){
                _agent->subscribeForModelChanges(makePath(_number, Leaf::none), moduleName, this);
                _namePath = makePath(_number, Leaf::userName);
                _agent->registerOperData(_namePath, moduleName, this);
                _state = State::idleReg;
                return true;
            }
            PrInt::println({"Number ",_number," is not acceptable"});
        }
        PrInt::println({"Number ",number," is already taken"});
    } else {
        PrInt::println("You are already registered");
    }
    return false;
}
bool MobileClient::unReg() {
    const std::string path = makePath(_number, Leaf::none);

    if (_state==State::idleReg){
        _number = "";
        _agent->closeSysrepo();
        _agent->deleteData(path);
        _state = State::idle;
        return true;
    } else {
        PrInt::println("You are not idle or not registered");
    }
    return false;
}

bool MobileClient::call(const std::string &destination_number) {
    const std::string sourceStatePath = makePath(_number, Leaf::state);
    const std::string destinationStatePath = makePath(destination_number, Leaf::state);
    const std::string destinationIncomingNumberPath = makePath(destination_number, 
                                                               Leaf::incomingNumber);
    std::string destinationState = "";
    const std::string idleState = states.find(State::idle)->second;
    const std::string activeState = states.find(State::active)->second;
    std::string routingName = "";

    if (destination_number != _number){
        if (_state == State::idleReg){
            if (_agent->fetchData(destinationStatePath, destinationState)){
                if (destinationState == idleState){
                    _routingNumber = destination_number;
                    _agent->changeData(sourceStatePath, activeState);
                    _agent->changeData(destinationIncomingNumberPath, _number);

                    _agent->fetchData(makePath(_routingNumber, Leaf::userName), routingName);
                    PrInt::println({"\nyou have called ", routingName});
                    return true;
                } else {
                    PrInt::println("the destination is not idle");
                }
            } else {
                PrInt::println("the number requested does not exist");
            }
        } else {
            PrInt::println({"you need to be idle and registered, but your state is: ", 
                           states.find(_state)->second});
        }
    } else {
        PrInt::println("you can't call yourself");
    }
    return false;
}

bool MobileClient::answer() {
    if (_state == State::activeIncoming){
        const std::string sourceStatePath = makePath(_number, Leaf::state);
        const std::string destinationStatePath = makePath(_routingNumber, Leaf::state);
        const std::string busyState = states.find(State::busy)->second;
            
        _agent->changeData(destinationStatePath, busyState);
        _agent->changeData(sourceStatePath, busyState);
        //while (_state!=State::busy);

        size_t waitTime = 0;
        while (_state!=State::busy && waitTime < timeout){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            waitTime += 100;
        }
        if (_state != State::busy){
            PrInt::logln("couldn't notify because wait time runout before CallBack");
        } else {
            _agent->notifySysrepo(notificationPath, _notificationMap);
        }
        return true;
    } else {
        PrInt::println({"you don't have an incoming call, but your state is: ", 
                       states.find(_state)->second});
    }
    return false;
}

bool MobileClient::reject() {
    if (_state == State::activeIncoming){
        const std::string sourceStatePath = makePath(_number, Leaf::state);
        const std::string sourceIncomingNumberPath = makePath(_number, Leaf::incomingNumber);
        const std::string idleState = states.find(State::idle)->second;
        const std::string destinationStatePath = makePath(_routingNumber, Leaf::state);
            
        _agent->changeData(destinationStatePath, idleState);
        _agent->changeData(sourceIncomingNumberPath, "");
        _agent->changeData(sourceStatePath, idleState);
        //while (_state!=State::idleReg);
        return true;
    } else {
        PrInt::println({"you don't have an incoming call, but your state is: ", 
                       states.find(_state)->second});
    }
    return false;
}


bool MobileClient::endCall() {

    if (_state == State::busy){
        if (_routingNumber!=""){
            const std::string sourceStatePath = makePath(_number, Leaf::state);
            const std::string sourceIncomingNumber = makePath(_number, Leaf::incomingNumber);
            const std::string destinationStatePath = makePath(_routingNumber, Leaf::state);
            const std::string destinatonIncomingNumber = makePath(_routingNumber, 
                                                                  Leaf::incomingNumber);
            const std::string idleState = states.find(State::idle)->second;
            
            _agent->changeData(destinationStatePath, idleState);
            _agent->changeData(sourceStatePath, idleState);
            _agent->changeData(destinatonIncomingNumber, "");
            _agent->changeData(sourceIncomingNumber, "");

            size_t waitTime = 0;
            while (_state!=State::idleReg && waitTime < timeout){
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                waitTime += 100;
            }
            if (_state != State::idleReg){
                PrInt::logln("couldn't notify because wait time runout before CallBack");
            } else {
                time_t endTime = std::time(nullptr);
                size_t durationSec = std::lround(std::difftime(endTime, _startTime));
                _notificationMap[duration] = StrInt::format(timeDurationString, 
                                                            {durationSec/60, durationSec%60});
                _agent->notifySysrepo(notificationPath, _notificationMap);
            }
            return true;
        } else {
            PrInt::println("you don't have an ongoing call, but your state is busy");
        }
    } else {
        PrInt::println({"you need to be busy to end a call, but your state is: ", 
                       states.find(_state)->second});
    }
    return false;
}

std::string MobileClient::makePath(const std::string &key, Leaf leaf) {
    return StrInt::format(leafPathPattern, 
                         {moduleName, containerPath, keyName, key, leafs.find(leaf)->second});
}

void MobileClient::handleModuleChange(const std::string &path, const std::string &value){
    const std::string statePath = makePath(_number, Leaf::state);
    const std::string incomingNumberPath = makePath(_number, Leaf::incomingNumber);
    const std::string idleState = states.find(State::idle)->second;
    const std::string activeState = states.find(State::active)->second;
    const std::string busyState = states.find(State::busy)->second;
    
    PrInt::logln({"handle: ",path ," -> ", value});

    if (path == statePath){
        if (value == idleState){
            _state = State::idleReg;
            PrInt::println("idle...");
        } else if (value == activeState){
            if (_state != State::activeIncoming){
                _state = State::active;
            }
            PrInt::println("active...");
        } else if (value == busyState){
            if (_state == State::activeIncoming){
                _startTime = std::time(nullptr);
                _notificationMap[startTime] = std::asctime(std::localtime(&_startTime));
                _notificationMap[abonentA] = _routingNumber;
                _notificationMap[abonentB] = _number;
                _notificationMap[duration] = "";
            } else if (_state == State::active) {
                _startTime = std::time(nullptr);
                _notificationMap[startTime] = std::asctime(std::localtime(&_startTime));
                _notificationMap[abonentA] = _number;
                _notificationMap[abonentB] = _routingNumber;
                _notificationMap[duration] = "";
            }
            _state = State::busy;
            PrInt::println("busy...");
        } else {
            PrInt::println({"unexpected state value: ", value});
        }
    } else if (path == incomingNumberPath){
        if (value != ""){
            _routingNumber = value;
            _state = State::activeIncoming;
            std::string incomingName = "";
            _agent->fetchData(makePath(_routingNumber, Leaf::userName), incomingName);
            PrInt::println({"you have an incloming call from user ", 
                           incomingName, 
                           " with number ", 
                           _routingNumber});
        } else {
            _routingNumber = "";
            PrInt::logln("incomingNumber value is made empty");
        }

    } else {
        PrInt::logln({"unexpected path", path});
    }

    PrInt::logln("exit handleModuleChange");
}

}
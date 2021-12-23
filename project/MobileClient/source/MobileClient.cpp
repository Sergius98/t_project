#include "MobileClient.hpp"
#include "NetConfAgent.hpp"

namespace mobileClient{


MobileClient::MobileClient(): 
    MobileClient(std::make_unique<NetConfAgent>()) {}
        
MobileClient::MobileClient(std::unique_ptr<INetConfAgent> agent): 
    _agent{std::move(agent)} {}

void MobileClient::close() {
    if (_state == State::active){
        if (_state == State::active){
            _agent->changeData(makePath(_routingNumber, Leaf::state), 
                               states.find(State::idle)->second);
            _agent->changeData(makePath(_routingNumber, Leaf::incomingNumber), "");
            _agent->changeData(makePath(_number, Leaf::state), states.find(State::idle)->second);
            _state = State::idleReg;
            _routingNumber = "";
        }
    }
    if (_state == State::busy){
        endCall();
        _state = State::idleReg;
    }
    if (_state == State::activeIncoming){
        reject();
        _state = State::idleReg;
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
                _agent->subscribeForModelChanges(makePath(_number, Leaf::none), MODULE_NAME, this);
                _namePath = makePath(_number, Leaf::userName);
                _agent->registerOperData(_namePath, MODULE_NAME, this);
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
    std::string destinationState = "";
    std::string routingName = "";

    if (destination_number != _number){
        if (_state == State::idleReg){
            if (_agent->fetchData(makePath(destination_number, Leaf::state), destinationState)){
                if (destinationState == states.find(State::idle)->second){
                    _routingNumber = destination_number;
                    _agent->changeData(makePath(_number, Leaf::state), 
                                       states.find(State::active)->second);
                    _agent->changeData(makePath(destination_number, Leaf::incomingNumber), 
                                       _number);
                    _agent->changeData(makePath(destination_number, Leaf::state), 
                                       states.find(State::active)->second);

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
        _startTime = std::time(nullptr);
        _notificationMap[START_TIME] = std::asctime(std::localtime(&_startTime));
        _notificationMap[ABONENT_A] = _routingNumber;
        _notificationMap[ABONENT_B] = _number;
        _notificationMap[DURATION] = "";
        _agent->notifySysrepo(NOTIFICATION_PATH, _notificationMap);

        _agent->changeData(makePath(_routingNumber, Leaf::state), 
                           states.find(State::busy)->second);
        _agent->changeData(makePath(_number, Leaf::state), states.find(State::busy)->second);

        return true;
    } else {
        PrInt::println({"\nyou don't have an incoming call, but your state is: ", 
                       states.find(_state)->second});
    }
    return false;
}

bool MobileClient::reject() {
    if (_state == State::activeIncoming){
        _agent->changeData(makePath(_routingNumber, Leaf::state), 
                           states.find(State::idle)->second);
        _agent->changeData(makePath(_number, Leaf::incomingNumber), "");
        _agent->changeData(makePath(_number, Leaf::state), states.find(State::idle)->second);
        return true;
    } else {
        PrInt::println({"\nyou don't have an incoming call, but your state is: ", 
                       states.find(_state)->second});
    }
    return false;
}


bool MobileClient::endCall() {
    if (_state == State::busy){
        if (_routingNumber!=""){
            time_t endTime = std::time(nullptr);
            size_t durationSec = std::lround(std::difftime(endTime, _startTime));
            _notificationMap[DURATION] = StrInt::format(TIME_DURATION_STRING, 
                                                        {durationSec/60, durationSec%60});
            _agent->notifySysrepo(NOTIFICATION_PATH, _notificationMap);

            _agent->changeData(makePath(_routingNumber, Leaf::state), 
                               states.find(State::idle)->second);
            _agent->changeData(makePath(_number, Leaf::state), states.find(State::idle)->second);
            _agent->changeData(makePath(_routingNumber, Leaf::incomingNumber), "");
            _agent->changeData(makePath(_number, Leaf::incomingNumber), "");

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
    return StrInt::format(LEAF_PATH_PATTERN, 
                         {MODULE_NAME, CONTAINER_PATH, KEY_NAME, key, leafs.find(leaf)->second});
}

void MobileClient::handleModuleChange(const std::string &path, const std::string &value){
    PrInt::logln({"handle: ",path ," -> ", value});
    if (path == makePath(_number, Leaf::state)){
        if (value == states.find(State::idle)->second){
            _state = State::idleReg;
            PrInt::println("idle...");
        } else if (value == states.find(State::active)->second){
            if (_state != State::activeIncoming){
                _state = State::active;
            }
            PrInt::println("active...");
        } else if (value == states.find(State::busy)->second){
            if (_state == State::active) {
                _startTime = std::time(nullptr);
                _notificationMap[START_TIME] = std::asctime(std::localtime(&_startTime));
                _notificationMap[ABONENT_A] = _number;
                _notificationMap[ABONENT_B] = _routingNumber;
                _notificationMap[DURATION] = "";
            }
            _state = State::busy;
            PrInt::println("busy...");
        } else {
            PrInt::println({"unexpected state value: ", value});
        }
    } else if (path == makePath(_number, Leaf::incomingNumber)){
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
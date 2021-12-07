#ifndef MOBILECLIENT_HPP
#define MOBILECLIENT_HPP

#include <map>
#include <iostream>
#include <string>
#include <memory>
#include <string>

#include "PrintInterface.hpp"
#include "StringInterface.hpp"
extern PrintInterface prInt;
extern StringInterface strInt;


class NetConfAgent;

enum class State {
    idle,
    idleReg,
    busy,
    active,
    activeIncoming
};
const std::map<State, std::string> states = {
    { State::idle, "idle" },
    { State::idleReg, "idle" },
    { State::busy, "busy" },
    { State::active, "active" },
    { State::activeIncoming, "active" }
};

enum class Leaf {
    none=0,
    number,
    userName,
    incomingNumber,
    state,
    all
};
const std::map<Leaf, std::string> leafs = {
    { Leaf::number, "/number" },
    { Leaf::userName, "/userName" },
    { Leaf::incomingNumber, "/incomingNumber" },
    { Leaf::state, "/state" },
    { Leaf::all, "/*" },
    { Leaf::none, "" }
};


const std::string moduleName = "commutator";
const std::string containerPath = "subscribers/subscriber";
const std::string keyName = "number";
const std::string leafPathPattern = "/{}:{}[{}='{}']{}";

class MobileClient {
    public:
        MobileClient();
        void setName(std::string name);
        void handleOperData(std::string &name);
        bool reg(std::string number);
        bool call(std::string destination_number);
        bool answer();
        bool reject();
        bool endCall();
        std::string getPath();
        void handleModuleChange(std::string path, std::string value);

        /*****/
        // delete later
        bool fetchData(std::string path, std::string &str);
        bool changeData(std::string path, std::string value);
        /*****/
    private:
        std::string _name = "";
        std::string _number = "";
        std::string _routingNumber = "";
        State _state = State::idle;
        std::unique_ptr<NetConfAgent> _agent;
        std::string _namePath = "";
        std::string makePath(std::string key, Leaf leaf);
};

#endif
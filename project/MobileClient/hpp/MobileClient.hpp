#include <map>
#include <iostream>
#include <string>


#include "NetConfAgent.hpp"


enum class State {
    idle,
    idleReg,
    busy,
    active
};
std::map<State, std::string> states = {
    { State::idle, "idle" },
    { State::idleReg, "idle" },
    { State::busy, "busy" },
    { State::active, "active" }
};

enum class Leaf {
    number,
    userName,
    incomingNumber,
    state,
    subscriber
};
std::map<Leaf, std::string> leafs = {
    { Leaf::number, "number" },
    { Leaf::userName, "userName" },
    { Leaf::incomingNumber, "incomingNumber" },
    { Leaf::state, "state" },
    { Leaf::subscriber, "subscriber" }
};


std::string _module_name = "commutator";
std::string _container_path = "subscribers/subscriber";
std::string _key_name = "number";

class MobileClient {
    public:
        MobileClient();
        void setName(std::string &name);
        bool reg(std::string &number);
        void setState(State &state);
    private:
        std::string makePath(Leaf node);
        std::string _name;
        std::string _number;
        std::string _incomingNumber;
        State _state;
        std::unique_ptr<NetConfAgent> _netConf;
        const std::string _leafPathPattern = "/{}:{}[{}='{}']/{}";
};
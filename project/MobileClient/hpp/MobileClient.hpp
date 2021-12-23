#ifndef MOBILECLIENT_HPP
#define MOBILECLIENT_HPP

#include <map>
#include <memory>
#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include <chrono>
#include <thread>

#include "INetConfAgent.hpp"
//#include "NetConfAgent.hpp"

#include "PrintInterface.hpp"
#include "StringInterface.hpp"

using stringInterface::StrInt;
using printInterface::PrInt;

namespace mobileClient{

const std::string moduleName = "commutator";
const std::string containerPath = "subscribers/subscriber";
const std::string keyName = "number";
const std::string leafPathPattern = "/{}:{}[{}='{}']{}";

const std::string startTime = "startTime";
const std::string abonentA = "abonentA";
const std::string abonentB = "abonentB";
const std::string duration = "duration";
const std::string notificationPath = "/commutator:connection";
const std::string timeDurationString = "{} minutes, {} seconds";
const size_t timeout = 1000;

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


class MobileClient {
    public:
        MobileClient();
        MobileClient(std::unique_ptr<INetConfAgent> agent);
        ~MobileClient();
        /**
         * @brief Stores the name string to be used as operData.
         *
         * @param[in] name string to be saved as name.
         */
        void setName(const std::string &name);
        /**
         * @brief Retrieves the name string to be used in operData.
         *
         * @param[out] name string to be set as name.
         */
        void handleOperData(std::string &name);
        /**
         * @brief Registers the subscriber in netopeer if the number is free.
         *
         * @param[in] number string to be stored as number.
         *
         * @return /true when the number is available, /false otherwise.
         */
        bool reg(const std::string &number);
        /**
         * @brief Unegisters the subscriber in netopeer if the subscriber is registered and idle.
         *
         * @return /true when the state is idleReg, /false otherwise.
         */
        bool unReg();
        /**
         * @brief send a call request to the subscriber with number destination_number.
         *
         * @param[in] destination_number string to use as the key for the subscriber's config.
         *
         * @return /true when the user is idleReg and the subscriber exist and is idle, /false otherwise.
         */
        bool call(const std::string &destination_number);
        /**
         * @brief establishes the connection with the incoming caller, whose number is stored in _routingNumber.
         *
         * @return /true when the user is activeIncoming, /false otherwise.
         */
        bool answer();
        /**
         * @brief refuses the connection with the incoming caller, whose number is stored in _routingNumber.
         *
         * @return /true when the user is activeIncoming, /false otherwise.
         */
        bool reject();
        /**
         * @brief ends the connection with the other subscriber, whose number is stored in _routingNumber.
         *
         * @return /true when the user is busy and the _routingNumber exists, /false otherwise.
         */
        bool endCall();
        /**
         * @brief handles the change-pairs recieved from the NetConfAgent.
         *
         * @param[in] path path to the leaf that was changed.
         *
         * @param[in] value value of the leaf that was changed.
         *
         * @return /true when the user is busy and the _routingNumber exists, /false otherwise.
         */
        void handleModuleChange(const std::string &path, const std::string &value);
    private:
        std::time_t _startTime;
        std::map<std::string,std::string> _notificationMap = {{ startTime, "" },
                                                              { abonentA, "" },
                                                              { abonentB, "" },
                                                              { duration, "" }
                                                             };
        std::string _name = "";
        std::string _number = "";
        std::string _routingNumber = "";
        State _state = State::idle;
        std::unique_ptr<INetConfAgent> _agent;
        std::string _namePath = "";
        /**
         * @brief formats path-string to the requested leaf.
         *
         * @param[in] key key (number) of the requested subscriber.
         *
         * @param[in] leaf code of the requested leaf.
         *
         * @return /true when the user is busy and the _routingNumber exists, /false otherwise.
         */
        std::string makePath(const std::string &key, Leaf leaf);
};


}

#endif

#ifndef NETCONFAGENT_HPP
#define NETCONFAGENT_HPP

#include <optional>
#include <atomic>
#include <string>
#include <iostream>
//#include <libyang-cpp/DataNode.hpp>
//#include <libyang-cpp/utils/exception.hpp>
#include <sysrepo-cpp/Connection.hpp>
#include <sysrepo-cpp/Subscription.hpp>
#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/exception.hpp>

#include "PrintInterface.hpp"
#include "StringInterface.hpp"

class MobileClient;

extern PrintInterface prInt;
extern StringInterface strInt;

class NetConfAgent {
    public:
        NetConfAgent();
        void init();
        void closeSysrepo();
        bool subscribeForModelChanges(std::string path, MobileClient *mobileClient);
        bool fetchData(std::string path, std::string &str);
        void registerOperData(std::string moduleName, std::string xPath, std::string &nodePath, std::string &value);
        bool changeData(std::string path, std::string value);
    private:
        std::unique_ptr<sysrepo::Connection> _conn;
        std::optional<sysrepo::Session> _sess;
        std::optional<sysrepo::Subscription> _subModuleChange;
        std::optional<sysrepo::Subscription> _subOperData;
};

#endif

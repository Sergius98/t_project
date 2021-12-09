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


extern PrintInterface prInt;
extern StringInterface strInt;

namespace MobileCli{
class MobileClient;


class NetConfAgent {
    public:
        NetConfAgent();
        void closeSysrepo();
        bool subscribeForModelChanges(std::string path, std::string modelName, MobileClient *mobileClient);
        bool fetchData(std::string path, std::string &str);
        bool deleteData(std::string path);
        void registerOperData(std::string &path, std::string modelName, MobileClient *mobileClient);
        bool changeData(std::string path, std::string value);
    private:
        std::unique_ptr<sysrepo::Connection> _conn;
        std::optional<sysrepo::Session> _sess;
        std::optional<sysrepo::Subscription> _subModuleChange;
        std::optional<sysrepo::Subscription> _subOperData;
};

}

#endif

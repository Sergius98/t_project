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


class NetConfAgent {
    public:
        NetConfAgent();
        NetConfAgent(PrintInterface printInterface);
        void init();
        void setPrintingInterface(PrintInterface printInterface);
        void closeSysrepo();
        bool subscribeForModelChanges();
        bool fetchData(std::string path, std::string &str);
        void registerOperData(std::string moduleName, std::string xPath, std::string &nodePath, std::string &value);
        bool changeData(std::string path, std::string value);
    private:
        std::unique_ptr<sysrepo::Connection> _conn;
        std::optional<sysrepo::Session> _sess;
        std::optional<sysrepo::Subscription> _subModuleChange;
        std::optional<sysrepo::Subscription> _subOperData;

        std::optional<PrintInterface> _printInterface;
};

#endif

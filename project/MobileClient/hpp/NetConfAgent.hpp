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

#include "INetConfAgent.hpp"

#include "PrintInterface.hpp"
#include "StringInterface.hpp"

using stringInterface::StrInt;
using printInterface::PrInt;

namespace mobileClient{


class NetConfAgent: public INetConfAgent {
    public:
        NetConfAgent();
        /**
        * @brief reset subscriptions
        */
        void closeSysrepo();
        /**
        * @brief Subscribe for changes made in the model with modelName
        * @param[in] path path of subscription
        * @param[in] modelName name of the model
        * @param[in] mobileClient pointer to the client for callback
        * @return /true when successful, /false otherwise.
        */
        bool subscribeForModelChanges(const std::string &path, const std::string &modelName, MobileClient *mobileClient);
        /**
        * @brief fetch data in str from path
        * @param[in] path path of data
        * @param[out] str variable for return
        * @return /true when successful, /false otherwise.
        */
        bool fetchData(const std::string &path, std::string &str);
        /**
        * @brief delete data with path in config
        * @param[in] path path of data
        * @return /true when successful, /false otherwise.
        */
        bool deleteData(const std::string &path);
        /**
        * @brief subscribe for Oper Data in path in modelName and call back mobileClient
        * @param[in] path path of data
        * @param[in] modelName name of data
        * @param[in] mobileClient client for callback
        * @return /true when successful, /false otherwise.
        */
        void registerOperData(const std::string &path, const std::string &modelName, MobileClient *mobileClient);
        /**
        * @brief set data in path as value
        * @param[in] path path of data
        * @param[in] value value to set
        * @return /true when successful, /false otherwise.
        */
        bool changeData(const std::string &path, const std::string &value);
        ~NetConfAgent();
    private:
        std::unique_ptr<sysrepo::Connection> _conn;
        std::optional<sysrepo::Session> _sess;
        std::optional<sysrepo::Subscription> _subModuleChange;
        std::optional<sysrepo::Subscription> _subOperData;
};

}

#endif

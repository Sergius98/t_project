#ifndef INETCONFAGENT_HPP
#define INETCONFAGENT_HPP

#include <optional>
#include <string>

namespace mobileClient{

class MobileClient;

class INetConfAgent {
    public:
        /**
        * @brief reset subscriptions
        */
        virtual void closeSysrepo()=0;

        /**
        * @brief Subscribe for changes made in the model with modelName
        * @param[in] path path of subscription
        * @param[in] modelName name of the model
        * @param[in] mobileClient pointer to the client for callback
        * @return /true when successful, /false otherwise.
        */
        virtual bool subscribeForModelChanges(const std::string &path, const std::string &modelName, MobileClient *mobileClient)=0;
        /**
        * @brief fetch data in str from path
        * @param[in] path path of data
        * @param[out] str variable for return
        * @return /true when successful, /false otherwise.
        */
        virtual bool fetchData(const std::string &path, std::string &str)=0;
        /**
        * @brief delete data with path in config
        * @param[in] path path of data
        * @return /true when successful, /false otherwise.
        */
        virtual bool deleteData(const std::string &path)=0;
        /**
        * @brief subscribe for Oper Data in path in modelName and call back mobileClient
        * @param[in] path path of data
        * @param[in] modelName name of data
        * @param[in] mobileClient client for callback
        * @return /true when successful, /false otherwise.
        */
        virtual void registerOperData(const std::string &path, const std::string &modelName, MobileClient *mobileClient)=0;
        /**
        * @brief set data in path as value
        * @param[in] path path of data
        * @param[in] value value to set
        * @return /true when successful, /false otherwise.
        */
        virtual bool changeData(const std::string &path, const std::string &value)=0;
        virtual ~INetConfAgent()=default;
};

}

#endif
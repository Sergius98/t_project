#ifndef INETCONFAGENT_HPP
#define INETCONFAGENT_HPP

#include <optional>
#include <string>

namespace MobileCli{

class MobileClient;

class INetConfAgent {
    public:
        virtual void closeSysrepo()=0;
        virtual bool subscribeForModelChanges(std::string path, std::string modelName, MobileClient *mobileClient)=0;
        virtual bool fetchData(std::string path, std::string &str)=0;
        virtual bool deleteData(std::string path)=0;
        virtual void registerOperData(std::string &path, std::string modelName, MobileClient *mobileClient)=0;
        virtual bool changeData(std::string path, std::string value)=0;
        virtual ~INetConfAgent()=default;
};

}

#endif
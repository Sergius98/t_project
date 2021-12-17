#ifndef NETCONFAGENTMOCK_HPP
#define NETCONFAGENTMOCK_HPP
#include <gmock/gmock.h>
#include "INetConfAgent.hpp"

class NetConfAgentMock: public MobileCli::INetConfAgent{
    public:
        MOCK_METHOD0(closeSysrepo, void());
        MOCK_METHOD3(subscribeForModelChanges, bool(std::string path, std::string modelName, MobileCli::MobileClient *mobileClient));
        MOCK_METHOD2(fetchData, bool(std::string path, std::string &str));
        MOCK_METHOD1(deleteData, bool(std::string path));
        MOCK_METHOD3(registerOperData, void(std::string &path, std::string modelName, MobileCli::MobileClient *mobileClient));
        MOCK_METHOD2(changeData, bool(std::string path, std::string value));
};

#endif
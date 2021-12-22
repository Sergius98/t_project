#ifndef NETCONFAGENTMOCK_HPP
#define NETCONFAGENTMOCK_HPP
#include <gmock/gmock.h>
#include "INetConfAgent.hpp"

namespace mock{

class NetConfAgentMock: public mobileClient::INetConfAgent{
    public:
        MOCK_METHOD0(closeSysrepo, void());
        MOCK_METHOD3(subscribeForModelChanges, bool(const std::string &path, const std::string &modelName, mobileClient::MobileClient *mobileClient));
        MOCK_METHOD2(fetchData, bool(const std::string &path, std::string &str));
        MOCK_METHOD1(deleteData, bool(const std::string &path));
        MOCK_METHOD3(registerOperData, void(const std::string &path, const std::string &modelName, mobileClient::MobileClient *mobileClient));
        MOCK_METHOD2(changeData, bool(const std::string &path, const std::string &value));
};

}

#endif

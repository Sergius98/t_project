#include <gtest/gtest.h>
#include "MobileClient.hpp"
#include "NetConfAgentMock.hpp"

TEST(MobileClient, UseSetName){
    MobileCli::MobileClient mobCli;
    NetConfAgentMock agentMock;
    std::string testName="TestNamen";
    std::string nameValue="";
    mobCli.setName(testName);
    mobCli.handleOperData(nameValue);
    ASSERT_EQ(testName, nameValue);
}
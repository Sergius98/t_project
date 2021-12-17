#include <gtest/gtest.h>

#include "MobileClient.hpp"
#include "NetConfAgentMock.hpp"
#include "StringInterface.hpp"
extern StringInterface strInt;

using testing::Sequence;
using testing::Return;
using testing::SetArgReferee;
using testing::_;


namespace test{

std::string testingPathPatern = "/commutator:subscribers/subscriber[number='{}']{}";

class MobileClientTest: public testing::Test{
    protected:
    void SetUp() override{
        auto tempMock = std::make_unique<testing::StrictMock<NetConfAgentMock>>();
        _mockAgent = tempMock.get();
        _client = std::make_unique<MobileCli::MobileClient>(std::move(tempMock));
    }/*
    void TearDown() override{
        EXPECT_CALL
        _client.reset();
    }*/
    testing::StrictMock <NetConfAgentMock> *_mockAgent;
    std::unique_ptr<MobileCli::MobileClient> _client;
};
TEST_F(MobileClientTest, SetNameAndHandleOperData){
    std::string testName="TestNamen";
    std::string nameValue="";
    _client->setName(testName);
    _client->handleOperData(nameValue);
    ASSERT_EQ(testName, nameValue);
}
TEST_F(MobileClientTest, RegisterTrue){
    std::string number = "010101";
    std::string name = "TestNamen";
    std::string numberPath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::number)->second});
    std::string namePath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::userName)->second});
    std::string subscribePath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::none)->second});

    _client->setName(name);
    EXPECT_CALL(*_mockAgent, fetchData(numberPath, _)).WillOnce(Return(false));
    EXPECT_CALL(*_mockAgent, changeData(numberPath, number)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, subscribeForModelChanges(subscribePath, MobileCli::moduleName, _)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, registerOperData(namePath, MobileCli::moduleName, _)).Times(1);
    EXPECT_TRUE(_client->reg(number));
}
TEST_F(MobileClientTest, RegisterFalseNumberIsTaken){
    std::string number = "010101";
    std::string numberPath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::number)->second});
    EXPECT_CALL(*_mockAgent, fetchData(numberPath, _)).WillOnce(Return(true));
    EXPECT_FALSE(_client->reg(number));
}
TEST_F(MobileClientTest, RegisterFalseStateIsNotIdle){
    std::string number = "010101";
    std::string name = "TestNamen";
    std::string numberPath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::number)->second});
    std::string namePath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::userName)->second});
    std::string subscribePath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::none)->second});
    std::string statePath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::state)->second});

    _client->setName(name);
    EXPECT_CALL(*_mockAgent, fetchData(numberPath, _)).WillOnce(Return(false));
    EXPECT_CALL(*_mockAgent, changeData(numberPath, number)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, subscribeForModelChanges(subscribePath, MobileCli::moduleName, _)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, registerOperData(namePath, MobileCli::moduleName, _)).Times(1);
    EXPECT_TRUE(_client->reg(number));

    EXPECT_FALSE(_client->reg(number));
    _client->handleModuleChange(statePath, MobileCli::states.find(MobileCli::State::active)->second);
    EXPECT_FALSE(_client->reg(number));
    _client->handleModuleChange(statePath, MobileCli::states.find(MobileCli::State::busy)->second);
    EXPECT_FALSE(_client->reg(number));
}
TEST_F(MobileClientTest, UnRegisterTrue){
    std::string number = "010101";
    std::string name = "TestNamen";
    std::string numberPath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::number)->second});
    std::string namePath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::userName)->second});
    std::string subscribePath = strInt.format(testingPathPatern, {number, MobileCli::leafs.find(MobileCli::Leaf::none)->second});

    _client->setName(name);
    EXPECT_CALL(*_mockAgent, fetchData(numberPath, _)).WillOnce(Return(false));
    EXPECT_CALL(*_mockAgent, changeData(numberPath, number)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, subscribeForModelChanges(subscribePath, MobileCli::moduleName, _)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, registerOperData(namePath, MobileCli::moduleName, _)).Times(1);
    EXPECT_TRUE(_client->reg(number));
    EXPECT_CALL(*_mockAgent, closeSysrepo()).Times(1);
    EXPECT_CALL(*_mockAgent, deleteData(subscribePath)).Times(1);
    EXPECT_TRUE(_client->unReg());
}
TEST_F(MobileClientTest, UnRegisterFalseNotRegistered){
    EXPECT_FALSE(_client->unReg());
}
}
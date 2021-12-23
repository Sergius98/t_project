#include <gtest/gtest.h>

#include "MobileClient.hpp"
#include "NetConfAgentMock.hpp"
#include "StringInterface.hpp"
using stringInterface::StrInt;

using testing::InSequence;
using testing::DoAll;
using testing::Return;
using testing::SetArgReferee;
using testing::_;


namespace test{

const std::string testNumber = "010101";
const std::string routingNumber = "232323";
const std::string testName = "TestNamen";
const std::string routingName = "RoutingNamen";

const std::string testingPathPatern = "/commutator:subscribers/subscriber[number='{}']{}";

const std::string numberPath =
    StrInt::format(testingPathPatern,
                  {testNumber, mobileClient::leafs.find(mobileClient::Leaf::number)->second});
const std::string routingNumberPath =
    StrInt::format(testingPathPatern,
                  {routingNumber, mobileClient::leafs.find(mobileClient::Leaf::number)->second});

const std::string incomingNumberPath =
    StrInt::format(testingPathPatern,
                  {testNumber, mobileClient::leafs.find(mobileClient::Leaf::incomingNumber)->second});
const std::string routingIncomingNumberPath =
    StrInt::format(testingPathPatern,
                  {routingNumber, mobileClient::leafs.find(mobileClient::Leaf::incomingNumber)->second});

const std::string namePath = 
    StrInt::format(testingPathPatern, 
                  {testNumber, mobileClient::leafs.find(mobileClient::Leaf::userName)->second});
const std::string routingNamePath = 
    StrInt::format(testingPathPatern,  
                  {routingNumber, mobileClient::leafs.find(mobileClient::Leaf::userName)->second});

const std::string subscribePath = 
    StrInt::format(testingPathPatern,  
                  {testNumber, mobileClient::leafs.find(mobileClient::Leaf::none)->second});
const std::string routingSubscribePath = 
    StrInt::format(testingPathPatern,  
                  {routingNumber, mobileClient::leafs.find(mobileClient::Leaf::none)->second});

const std::string statePath = 
    StrInt::format(testingPathPatern,  
                  {testNumber, mobileClient::leafs.find(mobileClient::Leaf::state)->second});
const std::string routingStatePath = 
    StrInt::format(testingPathPatern,  
                  {routingNumber, mobileClient::leafs.find(mobileClient::Leaf::state)->second});

const std::string idleState = mobileClient::states.find(mobileClient::State::idle)->second;
const std::string idleRegState = mobileClient::states.find(mobileClient::State::idleReg)->second;
const std::string activeState = mobileClient::states.find(mobileClient::State::active)->second;
const std::string activeIncomingState = 
    mobileClient::states.find(mobileClient::State::activeIncoming)->second;
const std::string busyState = mobileClient::states.find(mobileClient::State::busy)->second;


class MobileClientTest: public testing::Test{

    protected:
    void expectCallsRegisterTrue(){
        EXPECT_CALL(*_mockAgent, fetchData(numberPath, _)).WillOnce(Return(false));
        EXPECT_CALL(*_mockAgent, changeData(numberPath, testNumber)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, 
                    subscribeForModelChanges(subscribePath, mobileClient::moduleName, _)
                   ).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, registerOperData(namePath, mobileClient::moduleName, _)).Times(1);
    }

    void expectCallsUnregister(){
        EXPECT_CALL(*_mockAgent, closeSysrepo()).Times(1);
        EXPECT_CALL(*_mockAgent, deleteData(subscribePath)).WillOnce(Return(true));
    }

    void expectCallsCall(){
        EXPECT_CALL(*_mockAgent, fetchData(routingStatePath, _)).
            WillOnce(DoAll(SetArgReferee<1>(idleState), Return(true)));
        EXPECT_CALL(*_mockAgent, changeData(statePath, activeState)).
            WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(routingIncomingNumberPath, testNumber)).
            WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, fetchData(routingNamePath, _)).
            WillOnce(DoAll(SetArgReferee<1>(routingName), Return(true)));
    }

    void expectCallsEndCall(){
        EXPECT_CALL(*_mockAgent, changeData(routingStatePath, idleState)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(statePath, idleState)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(routingIncomingNumberPath, "")).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(incomingNumberPath, "")).WillOnce(Return(true));
        //EXPECT_CALL(*_mockAgent, notifySysrepo(_,_)).WillOnce(Return(true));
    }

    void expectCallsAnswer(){
        EXPECT_CALL(*_mockAgent, changeData(routingStatePath, busyState)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(statePath, busyState)).WillOnce(Return(true));
        //EXPECT_CALL(*_mockAgent, notifySysrepo(_,_)).WillOnce(Return(true));
    }

    void expectCallsDestructorRegect(){
        EXPECT_CALL(*_mockAgent, changeData(routingStatePath, idleState)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(incomingNumberPath, "")).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(statePath, idleState)).WillOnce(Return(true));
    }

    void expectCallsDestructorIdleReg(){
        expectCallsUnregister();
    }

    void expectCallsDestructorActive(){
        EXPECT_CALL(*_mockAgent, changeData(routingIncomingNumberPath, "")).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(routingStatePath, idleState)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(statePath, idleState)).WillOnce(Return(true));
        expectCallsDestructorIdleReg();
    }

    void expectCallsDestructorActiveIncoming(){
        expectCallsDestructorRegect();
        _client->handleModuleChange(statePath, idleState);
        expectCallsDestructorIdleReg();
    }

    void expectCallsDestructorBusy(){
        expectCallsEndCall();
        _client->handleModuleChange(statePath, idleState);
        expectCallsDestructorIdleReg();
    }

    void mockIncomingCall(){
        EXPECT_CALL(*_mockAgent, fetchData(routingNamePath, _)).
            WillOnce(DoAll(SetArgReferee<1>(routingName), Return(true)));
        _client->handleModuleChange(incomingNumberPath, routingNumber);
    }

    void SetUp() override{
        auto tempMock = std::make_unique<testing::StrictMock<mock::NetConfAgentMock>>();
        _mockAgent = tempMock.get();
        _client = std::make_unique<mobileClient::MobileClient>(std::move(tempMock));
    }

    void TearDown() override{
        _client.reset();
    }

    testing::StrictMock <mock::NetConfAgentMock> *_mockAgent;
    std::unique_ptr<mobileClient::MobileClient> _client;
};

TEST_F(MobileClientTest, SetNameAndHandleOperData){
    std::string nameValue = "";
    _client->setName(testName);
    _client->handleOperData(nameValue);
    ASSERT_EQ(testName, nameValue);
}

TEST_F(MobileClientTest, RegisterTrue){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));
    expectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, RegisterFalseNumberIsTaken){
    EXPECT_CALL(*_mockAgent, fetchData(numberPath, _)).WillOnce(Return(true));
    EXPECT_FALSE(_client->reg(testNumber));
}
TEST_F(MobileClientTest, RegisterFalseNumberIsNotAcceptable){
    EXPECT_CALL(*_mockAgent, fetchData(numberPath, _)).WillOnce(Return(false));
    EXPECT_CALL(*_mockAgent, changeData(numberPath, _)).WillOnce(Return(false));
    EXPECT_FALSE(_client->reg(testNumber));
}

TEST_F(MobileClientTest, RegisterFalseStateIsNotIdle){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    EXPECT_FALSE(_client->reg(testNumber));
    expectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, UnRegisterTrue){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    expectCallsUnregister();
    EXPECT_TRUE(_client->unReg());
}

TEST_F(MobileClientTest, UnRegisterFalseNotRegistered){
    EXPECT_FALSE(_client->unReg());
}

TEST_F(MobileClientTest, CallTrue){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    expectCallsCall();
    EXPECT_TRUE(_client->call(routingNumber));

    _client->handleModuleChange(statePath, activeState);
    expectCallsDestructorActive();
}

TEST_F(MobileClientTest, CallFalseNotRegistered){
    EXPECT_FALSE(_client->call(routingNumber));
}

TEST_F(MobileClientTest, CallFalseNotIdle){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    mockIncomingCall();    

    EXPECT_FALSE(_client->call(routingNumber));

    expectCallsDestructorActiveIncoming();
}

TEST_F(MobileClientTest, CallFalseDestinationDoNotExist){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    EXPECT_CALL(*_mockAgent, fetchData(routingStatePath, _)).
        WillOnce(DoAll(SetArgReferee<1>(""), Return(false)));
    EXPECT_FALSE(_client->call(routingNumber));

    expectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, CallFalseDestinationIsNotIdle){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    EXPECT_CALL(*_mockAgent, fetchData(routingStatePath, _)).
        WillOnce(DoAll(SetArgReferee<1>(activeState), Return(true)));
    EXPECT_FALSE(_client->call(routingNumber));

    expectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, AnswerTrue){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    mockIncomingCall();    

    EXPECT_CALL(*_mockAgent, changeData(routingStatePath, busyState)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(statePath, busyState)).WillOnce(Return(true));
    EXPECT_TRUE(_client->answer());
    _client->handleModuleChange(statePath, busyState);

    expectCallsDestructorBusy();
}

TEST_F(MobileClientTest, AnswerFalseStateIsNotActiveIncoming){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    expectCallsCall();
    EXPECT_TRUE(_client->call(routingNumber));
    _client->handleModuleChange(statePath, activeState);

    EXPECT_FALSE(_client->answer());

    expectCallsDestructorActive();
}

TEST_F(MobileClientTest, RejectTrue){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    mockIncomingCall();    

    EXPECT_CALL(*_mockAgent, changeData(routingStatePath, idleState)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(incomingNumberPath, "")).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(statePath, idleState)).WillOnce(Return(true));
    EXPECT_TRUE(_client->reject());
    _client->handleModuleChange(statePath, idleState);

    expectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, RejectFalseStateIsNotActiveIncoming){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    expectCallsCall();
    EXPECT_TRUE(_client->call(routingNumber));
    _client->handleModuleChange(statePath, activeState);

    EXPECT_FALSE(_client->reject());

    expectCallsDestructorActive();
}

TEST_F(MobileClientTest, endCallTrue){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    mockIncomingCall();    

    expectCallsAnswer();
    EXPECT_TRUE(_client->answer());
    _client->handleModuleChange(statePath, busyState);


    expectCallsEndCall();
    EXPECT_TRUE(_client->endCall());
    _client->handleModuleChange(statePath, idleState);

    expectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, endCallFalseNotBusy){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    EXPECT_FALSE(_client->endCall());

    expectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, endCallFalseNoNumber){
    _client->setName(testName);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    _client->handleModuleChange(incomingNumberPath, "");
    _client->handleModuleChange(statePath, busyState);

    EXPECT_FALSE(_client->endCall());

    _client->handleModuleChange(statePath, idleRegState);
    expectCallsDestructorIdleReg();
}

}

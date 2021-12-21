#include <gtest/gtest.h>

#include "MobileClient.hpp"
#include "NetConfAgentMock.hpp"
#include "StringInterface.hpp"
extern StringInterface strInt;

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
    strInt.format(testingPathPatern,
                  {testNumber, MobileCli::leafs.find(MobileCli::Leaf::number)->second});
const std::string routingNumberPath =
    strInt.format(testingPathPatern,
                  {routingNumber, MobileCli::leafs.find(MobileCli::Leaf::number)->second});

const std::string incomingNumberPath =
    strInt.format(testingPathPatern,
                  {testNumber, MobileCli::leafs.find(MobileCli::Leaf::incomingNumber)->second});
const std::string routingIncomingNumberPath =
    strInt.format(testingPathPatern,
                  {routingNumber, MobileCli::leafs.find(MobileCli::Leaf::incomingNumber)->second});

std::string namePath = 
    strInt.format(testingPathPatern, 
                  {testNumber, MobileCli::leafs.find(MobileCli::Leaf::userName)->second});
std::string routingNamePath = 
    strInt.format(testingPathPatern,  
                  {routingNumber, MobileCli::leafs.find(MobileCli::Leaf::userName)->second});

const std::string subscribePath = 
    strInt.format(testingPathPatern,  
                  {testNumber, MobileCli::leafs.find(MobileCli::Leaf::none)->second});
const std::string routingSubscribePath = 
    strInt.format(testingPathPatern,  
                  {routingNumber, MobileCli::leafs.find(MobileCli::Leaf::none)->second});

const std::string statePath = 
    strInt.format(testingPathPatern,  
                  {testNumber, MobileCli::leafs.find(MobileCli::Leaf::state)->second});
const std::string routingStatePath = 
    strInt.format(testingPathPatern,  
                  {routingNumber, MobileCli::leafs.find(MobileCli::Leaf::state)->second});

const std::string idleState = MobileCli::states.find(MobileCli::State::idle)->second;
const std::string idleRegState = MobileCli::states.find(MobileCli::State::idleReg)->second;
const std::string activeState = MobileCli::states.find(MobileCli::State::active)->second;
const std::string activeIncomingState = 
    MobileCli::states.find(MobileCli::State::activeIncoming)->second;
const std::string busyState = MobileCli::states.find(MobileCli::State::busy)->second;


class MobileClientTest: public testing::Test{

    protected:
    void ExpectCallsRegisterTrue(){
        EXPECT_CALL(*_mockAgent, fetchData(numberPath, _)).WillOnce(Return(false));
        EXPECT_CALL(*_mockAgent, changeData(numberPath, testNumber)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, 
                    subscribeForModelChanges(subscribePath, MobileCli::moduleName, _)
                   ).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, registerOperData(namePath, MobileCli::moduleName, _)).Times(1);
    }

    void ExpectCallsUnregister(){
        EXPECT_CALL(*_mockAgent, closeSysrepo()).Times(1);
        EXPECT_CALL(*_mockAgent, deleteData(subscribePath)).WillOnce(Return(true));
    }

    void ExpectCallsCall(){
        EXPECT_CALL(*_mockAgent, fetchData(routingStatePath, _)).WillOnce(DoAll(SetArgReferee<1>(idleState), Return(true)));
        EXPECT_CALL(*_mockAgent, changeData(statePath, activeState)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(routingStatePath, activeState)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(routingIncomingNumberPath, testNumber)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, fetchData(routingNamePath, _)).WillOnce(DoAll(SetArgReferee<1>(routingName), Return(true)));
    }

    void ExpectCallsEndCall(){
        EXPECT_CALL(*_mockAgent, changeData(routingIncomingNumberPath, "")).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(routingStatePath, idleState)).WillOnce(Return(true));
        //EXPECT_CALL(*_mockAgent, changeData(incomingNumberPath, "")).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(statePath, idleState)).WillOnce(Return(true));
    }

    void ExpectCallsDestructorRegect(){
        EXPECT_CALL(*_mockAgent, changeData(routingStatePath, idleState)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(incomingNumberPath, "")).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(statePath, idleState)).WillOnce(Return(true));
    }

    void ExpectCallsDestructorIdleReg(){
        ExpectCallsUnregister();
    }

    void ExpectCallsDestructorActive(){
        EXPECT_CALL(*_mockAgent, changeData(routingIncomingNumberPath, "")).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(routingStatePath, idleState)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(statePath, idleState)).WillOnce(Return(true));
        ExpectCallsDestructorIdleReg();
    }

    void ExpectCallsDestructorActiveIncoming(){
        ExpectCallsDestructorRegect();
        ExpectCallsDestructorIdleReg();
    }

    void ExpectCallsDestructorBusy(){
        ExpectCallsEndCall();
        ExpectCallsDestructorIdleReg();
    }

    void SetUp() override{
        auto tempMock = std::make_unique<testing::StrictMock<mock::NetConfAgentMock>>();
        _mockAgent = tempMock.get();
        _client = std::make_unique<MobileCli::MobileClient>(std::move(tempMock));
    }

    void TearDown() override{
        _client.reset();
    }

    testing::StrictMock <mock::NetConfAgentMock> *_mockAgent;
    std::unique_ptr<MobileCli::MobileClient> _client;
};

TEST_F(MobileClientTest, SetNameAndHandleOperData){
    std::string nameValue = "";
    _client->setName(testName);
    _client->handleOperData(nameValue);
    ASSERT_EQ(testName, nameValue);
}

TEST_F(MobileClientTest, RegisterTrue){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));
    ExpectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, RegisterFalseNumberIsTaken){
    EXPECT_CALL(*_mockAgent, fetchData(numberPath, _)).WillOnce(Return(true));
    EXPECT_FALSE(_client->reg(testNumber));
}

TEST_F(MobileClientTest, RegisterFalseStateIsNotIdle){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    EXPECT_FALSE(_client->reg(testNumber));
    ExpectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, UnRegisterTrue){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    ExpectCallsUnregister();
    EXPECT_TRUE(_client->unReg());
}

TEST_F(MobileClientTest, UnRegisterFalseNotRegistered){
    EXPECT_FALSE(_client->unReg());
}

TEST_F(MobileClientTest, CallTrue){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    ExpectCallsCall();
    EXPECT_TRUE(_client->call(routingNumber));

    _client->handleModuleChange(statePath, activeState);
    ExpectCallsDestructorActive();
}

TEST_F(MobileClientTest, CallFalseNotRegistered){
    EXPECT_FALSE(_client->call(routingNumber));
}

TEST_F(MobileClientTest, CallFalseNotIdle){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    // mocking incoming call
    _client->handleModuleChange(statePath, activeState);
    EXPECT_CALL(*_mockAgent, fetchData(routingNamePath, _)).WillOnce(DoAll(SetArgReferee<1>(routingName), Return(true)));
    _client->handleModuleChange(incomingNumberPath, routingNumber);

    EXPECT_FALSE(_client->call(routingNumber));

    ExpectCallsDestructorActiveIncoming();
}

TEST_F(MobileClientTest, CallFalseDestinationDoNotExist){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    EXPECT_CALL(*_mockAgent, fetchData(routingStatePath, _)).WillOnce(DoAll(SetArgReferee<1>(""), Return(false)));
    EXPECT_FALSE(_client->call(routingNumber));

    ExpectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, CallFalseDestinationIsNotIdle){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    EXPECT_CALL(*_mockAgent, fetchData(routingStatePath, _)).WillOnce(DoAll(SetArgReferee<1>(activeState), Return(true)));
    EXPECT_FALSE(_client->call(routingNumber));

    ExpectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, AnswerTrue){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    // mocking incoming call
    //_client->handleModuleChange(statePath, activeState); // todo in call: you only need to set the number
    EXPECT_CALL(*_mockAgent, fetchData(routingNamePath, _)).
        WillOnce(DoAll(SetArgReferee<1>(routingName), Return(true)));
    _client->handleModuleChange(incomingNumberPath, routingNumber);

    EXPECT_CALL(*_mockAgent, changeData(routingStatePath, busyState)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(statePath, busyState)).WillOnce(Return(true));
    EXPECT_TRUE(_client->answer());
    _client->handleModuleChange(statePath, busyState);

    ExpectCallsDestructorBusy();
}

TEST_F(MobileClientTest, AnswerFalseStateIsNotActiveIncoming){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    ExpectCallsCall();
    EXPECT_TRUE(_client->call(routingNumber));
    _client->handleModuleChange(statePath, activeState);

    EXPECT_FALSE(_client->answer());

    ExpectCallsDestructorActive();
}

TEST_F(MobileClientTest, RejectTrue){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    // mocking incoming call
    //_client->handleModuleChange(statePath, activeState); // todo in call: you only need to set the number
    EXPECT_CALL(*_mockAgent, fetchData(routingNamePath, _)).
        WillOnce(DoAll(SetArgReferee<1>(routingName), Return(true)));
    _client->handleModuleChange(incomingNumberPath, routingNumber);

    EXPECT_CALL(*_mockAgent, changeData(routingStatePath, idleState)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(incomingNumberPath, "")).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(statePath, idleState)).WillOnce(Return(true));
    EXPECT_TRUE(_client->reject());
    _client->handleModuleChange(statePath, idleState);

    ExpectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, RejectFalseStateIsNotActiveIncoming){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    ExpectCallsCall();
    EXPECT_TRUE(_client->call(routingNumber));
    _client->handleModuleChange(statePath, activeState);

    EXPECT_FALSE(_client->reject());

    ExpectCallsDestructorActive();
}

TEST_F(MobileClientTest, endCallTrue){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    // mocking incoming call
    //_client->handleModuleChange(statePath, activeState); // todo in call: you only need to set the number
    EXPECT_CALL(*_mockAgent, fetchData(routingNamePath, _)).
        WillOnce(DoAll(SetArgReferee<1>(routingName), Return(true)));
    _client->handleModuleChange(incomingNumberPath, routingNumber);

    EXPECT_CALL(*_mockAgent, changeData(routingStatePath, busyState)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(statePath, busyState)).WillOnce(Return(true));
    EXPECT_TRUE(_client->answer());
    _client->handleModuleChange(statePath, busyState);


    EXPECT_CALL(*_mockAgent, changeData(routingIncomingNumberPath, "")).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(routingStatePath, idleState)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(statePath, idleState)).WillOnce(Return(true));
    EXPECT_TRUE(_client->endCall());
    _client->handleModuleChange(statePath, idleState);

    ExpectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, endCallFalseNotBusy){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    EXPECT_FALSE(_client->endCall());

    ExpectCallsDestructorIdleReg();
}

TEST_F(MobileClientTest, endCallFalseNoNumber){
    _client->setName(testName);
    ExpectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(testNumber));

    _client->handleModuleChange(incomingNumberPath, "");
    _client->handleModuleChange(statePath, busyState);

    EXPECT_FALSE(_client->endCall());

    _client->handleModuleChange(statePath, idleRegState);
    ExpectCallsDestructorIdleReg();
}

}

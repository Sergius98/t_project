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


namespace{

const std::string TEST_NUMBER = "010101";
const std::string ROUTING_NUMBER = "232323";
const std::string TEST_NAME = "TEST_NAMEn";
const std::string ROUTING_NAMEN = "ROUTING_NAMENn";

const std::string TESTING_PATH_PATERN = "/commutator:subscribers/subscriber[number='{}']{}";

const std::string NUMBER_PATH =
    StrInt::format(TESTING_PATH_PATERN,
                  {TEST_NUMBER, mobileClient::leafs.find(mobileClient::Leaf::number)->second});
const std::string ROUTING_NUMBER_PATH =
    StrInt::format(TESTING_PATH_PATERN,
                  {ROUTING_NUMBER, mobileClient::leafs.find(mobileClient::Leaf::number)->second});

const std::string INCOMING_NUMBER_PATH =
    StrInt::format(TESTING_PATH_PATERN,
                  {TEST_NUMBER, mobileClient::leafs.find(mobileClient::Leaf::incomingNumber)->second});
const std::string ROUTING_INCOMING_NUMBER_PATH =
    StrInt::format(TESTING_PATH_PATERN,
                  {ROUTING_NUMBER, mobileClient::leafs.find(mobileClient::Leaf::incomingNumber)->second});

const std::string NAME_PATH = 
    StrInt::format(TESTING_PATH_PATERN, 
                  {TEST_NUMBER, mobileClient::leafs.find(mobileClient::Leaf::userName)->second});
const std::string ROUTING_NAME_PATH = 
    StrInt::format(TESTING_PATH_PATERN,  
                  {ROUTING_NUMBER, mobileClient::leafs.find(mobileClient::Leaf::userName)->second});

const std::string SUBSCRIBE_PATH = 
    StrInt::format(TESTING_PATH_PATERN,  
                  {TEST_NUMBER, mobileClient::leafs.find(mobileClient::Leaf::none)->second});
const std::string ROUTING_SUBSCRIBE_PATH = 
    StrInt::format(TESTING_PATH_PATERN,  
                  {ROUTING_NUMBER, mobileClient::leafs.find(mobileClient::Leaf::none)->second});

const std::string STATE_PATH = 
    StrInt::format(TESTING_PATH_PATERN,  
                  {TEST_NUMBER, mobileClient::leafs.find(mobileClient::Leaf::state)->second});
const std::string ROUTING_STATE_PATH = 
    StrInt::format(TESTING_PATH_PATERN,  
                  {ROUTING_NUMBER, mobileClient::leafs.find(mobileClient::Leaf::state)->second});

const std::string IDLE_STATE = mobileClient::states.find(mobileClient::State::idle)->second;
const std::string IDLE_REG_STATE = mobileClient::states.find(mobileClient::State::idleReg)->second;
const std::string ACTIVE_STATE = mobileClient::states.find(mobileClient::State::active)->second;
const std::string ACTIVE_INCOMING_STATE = 
    mobileClient::states.find(mobileClient::State::activeIncoming)->second;
const std::string BUSY_STATE = mobileClient::states.find(mobileClient::State::busy)->second;
}

namespace test{

class MobileClientTest: public testing::Test{

    protected:
    void expectCallsRegisterTrue(){
        EXPECT_CALL(*_mockAgent, fetchData(NUMBER_PATH, _)).WillOnce(Return(false));
        EXPECT_CALL(*_mockAgent, changeData(NUMBER_PATH, TEST_NUMBER)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, 
                    subscribeForModelChanges(SUBSCRIBE_PATH, mobileClient::MODULE_NAME, _)
                   ).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, registerOperData(NAME_PATH, mobileClient::MODULE_NAME, _)).Times(1);
    }

    void expectCallsUnregister(){
        EXPECT_CALL(*_mockAgent, closeSysrepo()).Times(1);
        EXPECT_CALL(*_mockAgent, deleteData(SUBSCRIBE_PATH)).WillOnce(Return(true));
    }

    void expectCallsCall(){
        EXPECT_CALL(*_mockAgent, fetchData(ROUTING_STATE_PATH, _)).
            WillOnce(DoAll(SetArgReferee<1>(IDLE_STATE), Return(true)));
        EXPECT_CALL(*_mockAgent, changeData(ROUTING_STATE_PATH, ACTIVE_STATE)).
            WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(STATE_PATH, ACTIVE_STATE)).
            WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(ROUTING_INCOMING_NUMBER_PATH, TEST_NUMBER)).
            WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, fetchData(ROUTING_NAME_PATH, _)).
            WillOnce(DoAll(SetArgReferee<1>(ROUTING_NAMEN), Return(true)));
    }

    void expectCallsEndCall(){
        EXPECT_CALL(*_mockAgent, changeData(ROUTING_STATE_PATH, IDLE_STATE)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(STATE_PATH, IDLE_STATE)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(ROUTING_INCOMING_NUMBER_PATH, "")).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(INCOMING_NUMBER_PATH, "")).WillOnce(Return(true));
    }

    void expectCallsAnswer(){
        EXPECT_CALL(*_mockAgent, notifySysrepo(_,_)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(ROUTING_STATE_PATH, BUSY_STATE)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(STATE_PATH, BUSY_STATE)).WillOnce(Return(true));
    }

    void expectCallsDestructorRegect(){
        EXPECT_CALL(*_mockAgent, changeData(ROUTING_STATE_PATH, IDLE_STATE)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(INCOMING_NUMBER_PATH, "")).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(STATE_PATH, IDLE_STATE)).WillOnce(Return(true));
    }

    void expectCallsDestructorActive(){
        EXPECT_CALL(*_mockAgent, changeData(ROUTING_INCOMING_NUMBER_PATH, "")).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(ROUTING_STATE_PATH, IDLE_STATE)).WillOnce(Return(true));
        EXPECT_CALL(*_mockAgent, changeData(STATE_PATH, IDLE_STATE)).WillOnce(Return(true));
        expectCallsUnregister();
    }

    void expectCallsDestructorActiveIncoming(){
        expectCallsDestructorRegect();
        expectCallsUnregister();
    }

    void expectCallsDestructorBusy(){
        expectCallsEndCall();
        expectCallsUnregister();
    }

    void incomingCall(){
        EXPECT_CALL(*_mockAgent, fetchData(ROUTING_NAME_PATH, _)).
            WillOnce(DoAll(SetArgReferee<1>(ROUTING_NAMEN), Return(true)));
        _client->handleModuleChange(INCOMING_NUMBER_PATH, ROUTING_NUMBER);
    }

    void SetUp() override{
        auto tempMock = std::make_unique<testing::StrictMock<mock::NetConfAgentMock>>();
        _mockAgent = tempMock.get();
        _client = std::make_unique<mobileClient::MobileClient>(std::move(tempMock));
    }

    void TearDown() override{
        _client->close();
    }

    testing::StrictMock <mock::NetConfAgentMock> *_mockAgent;
    std::unique_ptr<mobileClient::MobileClient> _client;
};

TEST_F(MobileClientTest, SetNameAndHandleOperData){
    std::string nameValue = "";
    _client->setName(TEST_NAME);
    _client->handleOperData(nameValue);
    ASSERT_EQ(TEST_NAME, nameValue);
}

TEST_F(MobileClientTest, RegisterTrue){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));
    expectCallsUnregister();
}

TEST_F(MobileClientTest, RegisterFalseNumberIsTaken){
    EXPECT_CALL(*_mockAgent, fetchData(NUMBER_PATH, _)).WillOnce(Return(true));
    EXPECT_FALSE(_client->reg(TEST_NUMBER));
}
TEST_F(MobileClientTest, RegisterFalseNumberIsNotAcceptable){
    EXPECT_CALL(*_mockAgent, fetchData(NUMBER_PATH, _)).WillOnce(Return(false));
    EXPECT_CALL(*_mockAgent, changeData(NUMBER_PATH, TEST_NUMBER)).WillOnce(Return(false));
    EXPECT_FALSE(_client->reg(TEST_NUMBER));
}

TEST_F(MobileClientTest, RegisterFalseStateIsNotIdle){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    EXPECT_FALSE(_client->reg(TEST_NUMBER));
    expectCallsUnregister();
}

TEST_F(MobileClientTest, UnRegisterTrue){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    expectCallsUnregister();
    EXPECT_TRUE(_client->unReg());
}

TEST_F(MobileClientTest, UnRegisterFalseNotRegistered){
    EXPECT_FALSE(_client->unReg());
}

TEST_F(MobileClientTest, CallTrue){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    expectCallsCall();
    EXPECT_TRUE(_client->call(ROUTING_NUMBER));

    _client->handleModuleChange(STATE_PATH, ACTIVE_STATE);
    expectCallsDestructorActive();
}

TEST_F(MobileClientTest, CallFalseNotRegistered){
    EXPECT_FALSE(_client->call(ROUTING_NUMBER));
}

TEST_F(MobileClientTest, CallFalseNotIdle){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    incomingCall();    

    EXPECT_FALSE(_client->call(ROUTING_NUMBER));

    expectCallsDestructorActiveIncoming();
}

TEST_F(MobileClientTest, CallFalseDestinationDoNotExist){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    EXPECT_CALL(*_mockAgent, fetchData(ROUTING_STATE_PATH, _)).
        WillOnce(DoAll(SetArgReferee<1>(""), Return(false)));
    EXPECT_FALSE(_client->call(ROUTING_NUMBER));

    expectCallsUnregister();
}

TEST_F(MobileClientTest, CallFalseDestinationIsNotIdle){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    EXPECT_CALL(*_mockAgent, fetchData(ROUTING_STATE_PATH, _)).
        WillOnce(DoAll(SetArgReferee<1>(ACTIVE_STATE), Return(true)));
    EXPECT_FALSE(_client->call(ROUTING_NUMBER));

    expectCallsUnregister();
}

TEST_F(MobileClientTest, AnswerTrue){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    incomingCall();    

    EXPECT_CALL(*_mockAgent, notifySysrepo(_,_)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(ROUTING_STATE_PATH, BUSY_STATE)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(STATE_PATH, BUSY_STATE)).WillOnce(Return(true));
    EXPECT_TRUE(_client->answer());
    _client->handleModuleChange(STATE_PATH, BUSY_STATE);

    EXPECT_CALL(*_mockAgent, notifySysrepo(_,_)).WillOnce(Return(true));
    expectCallsDestructorBusy();
}

TEST_F(MobileClientTest, AnswerFalseStateIsNotActiveIncoming){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    expectCallsCall();
    EXPECT_TRUE(_client->call(ROUTING_NUMBER));
    _client->handleModuleChange(STATE_PATH, ACTIVE_STATE);

    EXPECT_FALSE(_client->answer());

    expectCallsDestructorActive();
}

TEST_F(MobileClientTest, RejectTrue){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    incomingCall();    

    EXPECT_CALL(*_mockAgent, changeData(ROUTING_STATE_PATH, IDLE_STATE)).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(INCOMING_NUMBER_PATH, "")).WillOnce(Return(true));
    EXPECT_CALL(*_mockAgent, changeData(STATE_PATH, IDLE_STATE)).WillOnce(Return(true));
    EXPECT_TRUE(_client->reject());
    _client->handleModuleChange(STATE_PATH, IDLE_STATE);

    expectCallsUnregister();
}

TEST_F(MobileClientTest, RejectFalseStateIsNotActiveIncoming){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    expectCallsCall();
    EXPECT_TRUE(_client->call(ROUTING_NUMBER));
    _client->handleModuleChange(STATE_PATH, ACTIVE_STATE);

    EXPECT_FALSE(_client->reject());

    expectCallsDestructorActive();
}

TEST_F(MobileClientTest, endCallTrue){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    incomingCall();    

    expectCallsAnswer();
    EXPECT_TRUE(_client->answer());
    _client->handleModuleChange(STATE_PATH, BUSY_STATE);


    EXPECT_CALL(*_mockAgent, notifySysrepo(_,_)).WillOnce(Return(true));
    expectCallsEndCall();
    EXPECT_TRUE(_client->endCall());
    _client->handleModuleChange(STATE_PATH, IDLE_STATE);

    expectCallsUnregister();
}

TEST_F(MobileClientTest, endCallFalseNotBusy){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    EXPECT_FALSE(_client->endCall());

    expectCallsUnregister();
}

TEST_F(MobileClientTest, endCallFalseNoNumber){
    _client->setName(TEST_NAME);
    expectCallsRegisterTrue();
    EXPECT_TRUE(_client->reg(TEST_NUMBER));

    _client->handleModuleChange(INCOMING_NUMBER_PATH, "");
    _client->handleModuleChange(STATE_PATH, BUSY_STATE);

    EXPECT_FALSE(_client->endCall());

    _client->handleModuleChange(STATE_PATH, IDLE_REG_STATE);
    expectCallsUnregister();
}

}

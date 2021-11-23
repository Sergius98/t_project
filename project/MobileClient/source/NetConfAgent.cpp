#include "../include/NetConfAgent.hpp"

#include <iostream>


void NetConfAgent::init(){
    if (!conn){
        conn = std::make_unique<sysrepo::Connection>();
    }
    if (!sess){
        sess = conn->sessionStart();
    }
}

void NetConfAgent::subscribe(){
    sess->copyConfig(sysrepo::Datastore::Startup, "testmodel"); // "commutator");

    sysrepo::ModuleChangeCb moduleChangeCb = [this] (auto, auto, auto, auto, auto, auto) -> sysrepo::ErrorCode {
        called++;
        std::cout << std::endl << "called:" << called << std::endl << ">"; 
        std::cout << std::endl << "sess->getChanges():" << std::endl;
        
        for (auto change: sess->getChanges()){
            /*std::cout << "operation==Modified: " << (change.operation == sysrepo::ChangeOperation::Modified) << std::endl;
            std::cout << "node: " << change.node.child()->child()->asTerm().valueStr() << std::endl;
            std::cout << "previousValue: " << change.previousValue.value_or("empty") << std::endl;
            std::cout << "previousList: " << change.previousList.value_or("empty") << std::endl;
            std::cout << "previousDefault: " << change.previousDefault << std::endl;*/
        }
        std::cout << std::endl << ">"; 
        return sysrepo::ErrorCode::Ok;
    };
    sub = sess->onModuleChange("testmodel", moduleChangeCb, nullptr, 0, sysrepo::SubscribeOptions::DoneOnly);
    //sess->applyChanges();
}

void NetConfAgent::testGetData(){
    std::cout << "/testmodel:sports/person/name" << std::endl; 
    auto data = sess->getData("/testmodel:sports/person/name");
    std::cout << "data:" << std::endl; 
    std::cout << data->child()->child()->asTerm().valueStr() << std::endl; 
    //std::cout << data->asTerm().valueStr() << std::endl;
}
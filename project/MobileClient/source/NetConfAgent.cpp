#include "../include/NetConfAgent.hpp"

#include <iostream>
#include <libyang-cpp/DataNode.hpp>


NetConfAgent::NetConfAgent(){
    _conn = std::make_unique<sysrepo::Connection>();
    _sess = _conn->sessionStart();
    _sess->copyConfig(sysrepo::Datastore::Running, "testmodel"); // "commutator");
}

bool NetConfAgent::subscribeForModelChanges(){
    sysrepo::ModuleChangeCb moduleChangeCb = [this] (sysrepo::Session session, auto, auto, auto, auto, auto) -> sysrepo::ErrorCode {
        std::cout << std::endl << "sess->getChanges():" << std::endl;
        for (auto change: session.getChanges("/testmodel:sports/*")){
            std::cout << "operation==Created: " << (change.operation == sysrepo::ChangeOperation::Created) << std::endl;
            std::cout << "operation==Modified: " << (change.operation == sysrepo::ChangeOperation::Modified) << std::endl;
            std::cout << "operation==Deleted: " << (change.operation == sysrepo::ChangeOperation::Deleted) << std::endl;
            std::cout << "operation==Moved: " << (change.operation == sysrepo::ChangeOperation::Moved) << std::endl;
            std::cout << "node: " << change.node.path() << std::endl;
            std::cout << "previousValue: " << change.previousValue.value_or("empty") << std::endl;
            std::cout << "previousList: " << change.previousList.value_or("empty") << std::endl;
            std::cout << "previousDefault: " << change.previousDefault << std::endl;
        }
        std::cout << ">"; 
        std::cout.flush(); 
        return sysrepo::ErrorCode::Ok;
    };
    try {
        _sub = _sess->onModuleChange("testmodel",
                                    moduleChangeCb, 
                                    nullptr, 
                                    0, 
                                    sysrepo::SubscribeOptions::DoneOnly
        );
    } catch (const std::exception& e) {
        _sub = std::nullopt;
        std::cout << "SUBSCRIBE ON MODULE CHANGE FAILED"<< std::endl;
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}


bool NetConfAgent::fetchData(std::string &data_str, std::string path){
    std::optional<libyang::DataNode> data;
    try{
        data = _sess->getData(path.c_str());
    } catch (const std::exception& e) {
        std::cout << "GET DATA FAILED"<< std::endl;
        std::cout << e.what() << std::endl;
    }
    if (data.has_value()){
        auto data_node = data->findPath(path.c_str());
        if (data_node.has_value()){
            data_str = data_node.value().asTerm().valueStr();
            return true;
        }
    }
    std::cout << "data not found" << std::endl; 
    return false;
}






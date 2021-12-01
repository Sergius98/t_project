#include "NetConfAgent.hpp"



NetConfAgent::NetConfAgent(){
    _conn = std::make_unique<sysrepo::Connection>();
    _sess = _conn->sessionStart();
    _sess->switchDatastore(sysrepo::Datastore::Operational);
    /*
    _sess->copyConfig(sysrepo::Datastore::Startup, "testmodel"); // "commutator");
    */
}


void NetConfAgent::registerOperData(std::string moduleName, std::string xPath, std::string &nodePath, std::string &value) {
    sysrepo::OperGetItemsCb operGetCb = [&] (sysrepo::Session session, auto, auto, auto, auto, auto, std::optional<libyang::DataNode>& parent) {
        std::cout << std::endl << "operGetCb():" << std::endl;
        parent = session.getContext().newPath(nodePath.c_str(), value.c_str());
        return sysrepo::ErrorCode::Ok;
    };
    _subOperData = _sess->onOperGetItems(moduleName.c_str(), operGetCb, xPath.c_str());
}

// std::string path
bool NetConfAgent::subscribeForModelChanges(){
    sysrepo::ModuleChangeCb moduleChangeCb = [this] (sysrepo::Session session, auto, auto, auto, auto, auto) -> sysrepo::ErrorCode {
        std::cout << std::endl << "sess->getChanges():" << std::endl;
        for (auto change: session.getChanges("//.")){
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
    std::string path = "/testmodel:sports/person[name='Likera']";
    _subModuleChange = _sess->onModuleChange("testmodel",
                                moduleChangeCb,
                                path.c_str(),
                                0,
                                sysrepo::SubscribeOptions::DoneOnly
    );
    
    return true;
}


bool NetConfAgent::fetchData(std::string path, std::string &data_str){
    std::optional<libyang::DataNode> data; //"/testmodel:sports/person[name='Mike']/name"

    _sess->switchDatastore(sysrepo::Datastore::Running);
    data = _sess->getData(path.c_str());
    std::cout << "path :" << path << std::endl;
    if (data.has_value()){
        std::cout << "data has_value" << std::endl; 
        auto data_node = data->findPath(path.c_str());
        if (data_node.has_value()){
            data_str = data_node->asTerm().valueStr();
            return true;
        }
    }
    std::cout << "data not found" << std::endl; 
    data_str = "";
    _sess->switchDatastore(sysrepo::Datastore::Operational);
    return false;
}

bool NetConfAgent::changeData(std::string path, std::string value) {
    _sess->setItem(path.c_str(), value.c_str());
    _sess->applyChanges();
    return true;
}




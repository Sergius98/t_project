#include "NetConfAgent.hpp"
#include "MobileClient.hpp"


NetConfAgent::NetConfAgent(){
    init();
}

void NetConfAgent::init(){
    _conn = std::make_unique<sysrepo::Connection>();
    _sess = _conn->sessionStart();
    //_sess->switchDatastore(sysrepo::Datastore::Operational);
    /*
    _sess->copyConfig(sysrepo::Datastore::Startup, "testmodel"); // "commutator");
    */
}

void NetConfAgent::closeSysrepo(){
    // todo: delete config
    _subOperData.reset();
    _subModuleChange.reset();
    _sess.reset();
    _conn.reset();
}


void NetConfAgent::registerOperData(std::string moduleName, std::string xPath, std::string &nodePath, std::string &value) {
    _subOperData.reset();
    sysrepo::OperGetItemsCb operGetCb = [&] (sysrepo::Session session, auto, auto, auto, auto, auto, std::optional<libyang::DataNode>& parent) {
        prInt.logln("\noperGetCb():");
        prInt.logInputPointer();
        //std::cout << std::endl << "operGetCb():" << std::endl;
        parent = session.getContext().newPath(nodePath.c_str(), value.c_str());
        return sysrepo::ErrorCode::Ok;
    };
    _subOperData = _sess->onOperGetItems(moduleName.c_str(), operGetCb, xPath.c_str());
}

// std::string path
bool NetConfAgent::subscribeForModelChanges(std::string path, MobileClient *mobileClient){
    _subModuleChange.reset();
    sysrepo::ModuleChangeCb moduleChangeCb = [mobileClient] (sysrepo::Session session, auto, auto, auto, auto, auto) -> sysrepo::ErrorCode {
        prInt.logln("\nmoduleChangeCb()");
        prInt.logInputPointer();
        //std::cout << std::endl << "sess->getChanges():" << std::endl;
        for (auto change: session.getChanges("//.")){
            if (change.operation == sysrepo::ChangeOperation::Created || change.operation == sysrepo::ChangeOperation::Modified){
                if (change.node.schema().nodeType() == libyang::NodeType::Leaf){
                    std::string change_path = std::string(change.node.path().get().get());
                    std::string change_value = std::string(change.node.asTerm().valueStr());
                    prInt.logln({"found change: ", change_path, change_value});
                    mobileClient->handleModuleChange(change_path, change_value);
                }
            }
        }
        return sysrepo::ErrorCode::Ok;
    };
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

    //_sess->switchDatastore(sysrepo::Datastore::Running);
    data = _sess->getData(path.c_str());
    prInt.log({"path :", path, "\n"});
    //std::cout << "path :" << path << std::endl;
    if (data.has_value()){
        auto data_node = data->findPath(path.c_str());
        if (data_node.has_value()){
            data_str = data_node->asTerm().valueStr();
            return true;
        }
    }
    prInt.println("data not found");
    //std::cout << "data not found" << std::endl; 
    data_str = "";
    //_sess->switchDatastore(sysrepo::Datastore::Operational);
    return false;
}

bool NetConfAgent::changeData(std::string path, std::string value) {
    _sess->setItem(path.c_str(), value.c_str());
    _sess->applyChanges();
    return true;
}




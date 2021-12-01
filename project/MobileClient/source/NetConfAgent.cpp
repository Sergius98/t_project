#include "NetConfAgent.hpp"



NetConfAgent::NetConfAgent(){
    init();
}
NetConfAgent::NetConfAgent(PrintInterface printInterface){
    NetConfAgent();
    setPrintingInterface(printInterface);
}

void NetConfAgent::setPrintingInterface(PrintInterface printInterface){
    _printInterface = printInterface;
}

void NetConfAgent::init(){
    _conn = std::make_unique<sysrepo::Connection>();
    _sess = _conn->sessionStart();
    _sess->switchDatastore(sysrepo::Datastore::Operational);
    /*
    _sess->copyConfig(sysrepo::Datastore::Startup, "testmodel"); // "commutator");
    */
}

void NetConfAgent::closeSysrepo(){
    _subOperData.reset();
    _subModuleChange.reset();
    _sess.reset();
    _conn.reset();
}


void NetConfAgent::registerOperData(std::string moduleName, std::string xPath, std::string &nodePath, std::string &value) {
    _subOperData.reset();
    sysrepo::OperGetItemsCb operGetCb = [&] (sysrepo::Session session, auto, auto, auto, auto, auto, std::optional<libyang::DataNode>& parent) {
        std::cout << std::endl << "operGetCb():" << std::endl;
        parent = session.getContext().newPath(nodePath.c_str(), value.c_str());
        return sysrepo::ErrorCode::Ok;
    };
    _subOperData = _sess->onOperGetItems(moduleName.c_str(), operGetCb, xPath.c_str());
}

// std::string path
bool NetConfAgent::subscribeForModelChanges(){
    _subModuleChange.reset();
    sysrepo::ModuleChangeCb moduleChangeCb = [this] (sysrepo::Session session, auto, auto, auto, auto, auto) -> sysrepo::ErrorCode {
        std::cout << std::endl << "sess->getChanges():" << std::endl;
        for (auto change: session.getChanges("//.")){
            if (change.operation == sysrepo::ChangeOperation::Created || change.operation == sysrepo::ChangeOperation::Modified){
                if (change.node.schema().nodeType() == libyang::NodeType::Leaf){

                }
            }
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




#include "NetConfAgent.hpp"
#include "MobileClient.hpp"


NetConfAgent::NetConfAgent(){
    _conn = std::make_unique<sysrepo::Connection>();
    _sess = _conn->sessionStart();
    //_sess->switchDatastore(sysrepo::Datastore::Operational);
    /*
    _sess->copyConfig(sysrepo::Datastore::Startup, "testmodel"); // "commutator");
    */
}

void NetConfAgent::init(){
}

void NetConfAgent::closeSysrepo(){
    // todo: delete config
    _subOperData.reset();
    _subModuleChange.reset();
}


void NetConfAgent::registerOperData(std::string &path, std::string modelName, MobileClient *mobileClient) {
    _subOperData.reset();
    sysrepo::OperGetItemsCb operGetCb = [mobileClient, path] (sysrepo::Session session, auto, auto, auto, auto, auto, std::optional<libyang::DataNode>& parent) {
        prInt.print("\n");
        prInt.logln("operGetCb():");
        std::string value = "";
        mobileClient->handleOperData(value);
        parent = session.getContext().newPath(path.c_str(), value.c_str());
        prInt.logInputPointer();
        return sysrepo::ErrorCode::Ok;
    };
    _subOperData = _sess->onOperGetItems(moduleName.c_str(), operGetCb, path.c_str());
}

// std::string path
bool NetConfAgent::subscribeForModelChanges(std::string path, std::string modelName, MobileClient *mobileClient){
    _subModuleChange.reset();
    sysrepo::ModuleChangeCb moduleChangeCb = [mobileClient] (sysrepo::Session session, auto, auto, auto, auto, auto) -> sysrepo::ErrorCode {
        prInt.print("\n");
        prInt.logln("moduleChangeCb()");
        //std::cout << std::endl << "sess->getChanges():" << std::endl;
        for (auto change: session.getChanges("//.")){
            if (change.operation == sysrepo::ChangeOperation::Created || change.operation == sysrepo::ChangeOperation::Modified){
                if (change.node.schema().nodeType() == libyang::NodeType::Leaf){
                    std::string changePath = std::string(change.node.path().get().get());
                    std::string changeValue = std::string(change.node.asTerm().valueStr());
                    prInt.logln({"found change: ", changePath," -into-> ", changeValue});
                    mobileClient->handleModuleChange(changePath, changeValue);
                }
            }
        }
        prInt.logInputPointer();
        return sysrepo::ErrorCode::Ok;
    };
    prInt.logln({"subscribed on: ", path});
    _subModuleChange = _sess->onModuleChange(modelName.c_str(),
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
    } else {
        prInt.logln("data not found in Running");
        _sess->switchDatastore(sysrepo::Datastore::Operational);
        data = _sess->getData(path.c_str());
        if (data.has_value()){
            auto data_node = data->findPath(path.c_str());
            if (data_node.has_value()){
                data_str = data_node->asTerm().valueStr();
                _sess->switchDatastore(sysrepo::Datastore::Running);
                return true;
            }
        }
        _sess->switchDatastore(sysrepo::Datastore::Running);

    }
    prInt.logln("data not found in Operational");
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




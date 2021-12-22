#include "NetConfAgent.hpp"
#include "MobileClient.hpp"


namespace mobileClient{

NetConfAgent::NetConfAgent(){
    _conn = std::make_unique<sysrepo::Connection>();
    _sess = _conn->sessionStart();
    /*
    _sess->copyConfig(sysrepo::Datastore::Startup, "commutator");
    */
}

NetConfAgent::~NetConfAgent(){
    closeSysrepo();
    _sess.reset();
    _conn.reset();
}


void NetConfAgent::closeSysrepo(){
    _subOperData.reset();
    _subModuleChange.reset();
}


void NetConfAgent::registerOperData(const std::string &path, 
                                    const std::string &modelName, 
                                    MobileClient *mobileClient) {
    _subOperData.reset();
    sysrepo::OperGetItemsCb operGetCb = [mobileClient, 
                                         path] (sysrepo::Session session, 
                                                auto, 
                                                auto, 
                                                auto, 
                                                auto, 
                                                auto, 
                                                std::optional<libyang::DataNode>& parent) {
        PrInt::logln("\noperGetCb():");
        std::string value = "";
        mobileClient->handleOperData(value);
        parent = session.getContext().newPath(path.c_str(), value.c_str());
        PrInt::logInputPointer();
        return sysrepo::ErrorCode::Ok;
    };
    _subOperData = _sess->onOperGetItems(modelName.c_str(), operGetCb, path.c_str());
}


bool NetConfAgent::subscribeForModelChanges(const std::string &path, 
                                            const std::string &modelName, 
                                            MobileClient *mobileClient){
    _subModuleChange.reset();
    sysrepo::ModuleChangeCb moduleChangeCb = [mobileClient] (sysrepo::Session session, 
                                                                auto, 
                                                                auto, 
                                                                auto, 
                                                                auto, 
                                                                auto) -> sysrepo::ErrorCode {
        PrInt::print("\n");
        PrInt::logln("moduleChangeCb()");
        for (auto change: session.getChanges("//.")){
            if (change.operation == sysrepo::ChangeOperation::Created || 
                change.operation == sysrepo::ChangeOperation::Modified){
                if (change.node.schema().nodeType() == libyang::NodeType::Leaf){
                    std::string changePath = std::string(change.node.path().get().get());
                    std::string changeValue = std::string(change.node.asTerm().valueStr());
                    PrInt::logln({"found change: ", changePath," -into-> ", changeValue});
                    mobileClient->handleModuleChange(changePath, changeValue);
                }
            }
        }
        PrInt::printInputPointer();
        return sysrepo::ErrorCode::Ok;
    };
    PrInt::logln({"subscribed on: ", path});
    _subModuleChange = _sess->onModuleChange(modelName.c_str(),
                                moduleChangeCb,
                                path.c_str(),
                                0,
                                sysrepo::SubscribeOptions::DoneOnly
    );
    return true;
}


bool NetConfAgent::fetchData(const std::string &path, std::string &data_str){
    std::optional<libyang::DataNode> data;

    data = _sess->getData(path.c_str());
    PrInt::log({"path :", path, "\n"});
    if (data.has_value()){
        auto data_node = data->findPath(path.c_str());
        if (data_node.has_value()){
            data_str = data_node->asTerm().valueStr();
            return true;
        }
    } else {
        PrInt::logln("data not found in Running");
        _sess->switchDatastore(sysrepo::Datastore::Operational);
        data = _sess->getData(path.c_str());
        _sess->switchDatastore(sysrepo::Datastore::Running);
        if (data.has_value()){
            auto data_node = data->findPath(path.c_str());
            if (data_node.has_value()){
                data_str = data_node->asTerm().valueStr();
                return true;
            }
        }

    }
    PrInt::logln("data not found in Operational");
    data_str = "";
    return false;
}

bool NetConfAgent::changeData(const std::string &path, const std::string &value) {
    _sess->setItem(path.c_str(), value.c_str());
    _sess->applyChanges();
    return true;
}

bool NetConfAgent::deleteData(const std::string &path) {
    _sess->deleteItem(path.c_str());
    _sess->applyChanges();
    return true;
}



}
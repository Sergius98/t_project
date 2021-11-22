#include "../include/NetConfAgent.hpp"
#include <atomic>
#include <iostream>


void NetConfAgent::init(){
    conn = std::make_unique<sysrepo::Connection>();
}

void NetConfAgent::subscribe(){
    sess = conn->sessionStart();
    sess->copyConfig(sysrepo::Datastore::Startup, "commutator");
    std::atomic<int> called = 0;

    sysrepo::ModuleChangeCb moduleChangeCb = [&called] (auto, auto, auto, auto, auto, auto) -> sysrepo::ErrorCode {
        called++;
        std::cout << "called:" << called << std::endl; 
        return sysrepo::ErrorCode::Ok;
    };
    sub = sess->onModuleChange("test_module", moduleChangeCb);
}
#ifndef NETCONFAGENT_HPP
#define NETCONFAGENT_HPP

#include <optional>
#include <atomic>
#include <string>

#include <sysrepo-cpp/Connection.hpp>
#include <sysrepo-cpp/Subscription.hpp>
#include <sysrepo-cpp/Session.hpp>


class NetConfAgent {
    public:
        NetConfAgent();
        bool subscribeForModelChanges();
        bool fetchData(std::string &str, std::string path);
    private:
        std::unique_ptr<sysrepo::Connection> _conn;
        std::optional<sysrepo::Session> _sess;
        std::optional<sysrepo::Subscription> _sub;
};

#endif

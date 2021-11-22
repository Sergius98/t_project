#ifndef NETCONFAGENT_HPP
#define NETCONFAGENT_HPP

#include <optional>
#include <sysrepo-cpp/Connection.hpp>
#include <sysrepo-cpp/Session.hpp>


class NetConfAgent {
    public:
        void init();
        void subscribe();
    private:
        std::unique_ptr<sysrepo::Connection> conn;
        std::optional<sysrepo::Session> sess;
        std::optional<sysrepo::Subscription> sub;
};

#endif
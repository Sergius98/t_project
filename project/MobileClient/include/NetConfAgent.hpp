#ifndef NETCONFAGENT_HPP
#define NETCONFAGENT_HPP

#include <optional>
#include <atomic>

#include <sysrepo-cpp/Connection.hpp>
#include <sysrepo-cpp/Subscription.hpp>
#include <sysrepo-cpp/Session.hpp>


class NetConfAgent {
    public:
        std::atomic<int> called = 0;

        void init();
        void subscribe();
        void testGetData();
    private:
        std::unique_ptr<sysrepo::Connection> conn;
        std::optional<sysrepo::Session> sess;
        std::optional<sysrepo::Subscription> sub;
};

#endif
#ifndef KEEPALIVE_H
#define KEEPALIVE_H

#include <string>
#include <atomic>
#include <thread>

class KeepAlive {
public:
    KeepAlive(const std::string& target, int intervalSec, int retryLimit);
    ~KeepAlive();

    void start();
    void stop();
    bool isRunning() const;

private:
    std::string target_;
    int intervalSec_;
    int retryLimit_;
    std::atomic<bool> running_{false};
    std::thread thread_;

    void loop();
};

#endif

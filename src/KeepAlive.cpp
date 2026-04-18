#include "KeepAlive.h"
#include "HTTPSClient.h"
#include "Logger.h"
#include <thread>
#include <chrono>

KeepAlive::KeepAlive(const std::string& target, int intervalSec, int retryLimit)
    : target_(target), intervalSec_(intervalSec), retryLimit_(retryLimit) {}

KeepAlive::~KeepAlive() {
    stop();
    if (thread_.joinable()) {
        thread_.join();
    }
}

void KeepAlive::start() {
    running_ = true;
    thread_ = std::thread(&KeepAlive::loop, this);
}

void KeepAlive::stop() {
    running_ = false;
}

bool KeepAlive::isRunning() const {
    return running_;
}

void KeepAlive::loop() {
    int errorCount = 0;
    while (running_) {
        try {
            HTTPSClient client;
            client.get(target_);
            errorCount = 0;
        } catch (const std::exception& e) {
            errorCount++;
            LOG_ERROR("KeepAlive request failed: " + std::string(e.what()));
            if (errorCount >= retryLimit_) {
                LOG_ERROR("KeepAlive retry limit exceeded, exiting.");
                break;
            }
        }
        // Sleep in 1-second chunks so stop() can be responsive
        for (int i = 0; i < intervalSec_ && running_; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    running_ = false;
}

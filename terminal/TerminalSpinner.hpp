#pragma once

#include <atomic>
#include <thread>
#include <string>

constexpr int DELAY = 35;

class Spinner {
    private:
        std::atomic<bool> thinking{false};
        std::thread spinner;
        void printSpinnerElement(const std::string& elementToPrint);
        bool detectEscapeToStop();
    public:
        void start(std::atomic<bool>& cancelRequesting);
        void stop();
};

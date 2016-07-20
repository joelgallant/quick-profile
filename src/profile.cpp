#include "profile.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

static uint32_t getTick() {
    struct timespec timespec_time;
    uint32_t theTick = 0;

    clock_gettime(CLOCK_MONOTONIC, &timespec_time);

    theTick = timespec_time.tv_nsec / 1000000;
    theTick += timespec_time.tv_sec * 1000;
    return theTick;
}

Profiler& Profiler::get() {
    static Profiler instance;

    return instance;
}

Profiler::Profiler() { }

void Profiler::startMain() {
    start("MAIN");
}

void Profiler::stopMain() {
    stop("MAIN");
}

void Profiler::start(std::string s) {
    started[s] = getTick();
}

void Profiler::stop(std::string s) {
    calls[s] += 1;
    totalTime[s] += getTick() - started[s];
    averageTime[s] = totalTime[s] / calls[s];
}

void Profiler::report(std::string s) {
    cout <<
        setw(30) << s <<
        setw(15) << "calls: " << calls[s] <<
        setw(15) << "callsPer: " << calls[s] / calls["MAIN"] <<
        setw(15) << "totalTime: " << totalTime[s] << "ms" <<
        "\ttime%: " << 100 * (float(totalTime[s]) / totalTime["MAIN"]) <<
        "\taverageTime: " << averageTime[s] << "ms" << endl;
}

void Profiler::report() {
    cout << "--- ---" << endl;
    for (auto i = started.begin(); i != started.end(); i++) {
        report(i->first);
    }
}

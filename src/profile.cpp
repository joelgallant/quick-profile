#include "profile.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

static uint32_t tick_micro() {
    struct timespec timespec_time;

    clock_gettime(CLOCK_MONOTONIC, &timespec_time);

    uint32_t tick = timespec_time.tv_nsec / 1000;
    tick += timespec_time.tv_sec * 1000000;
    return tick;
}

Profiler& Profiler::get() {
    static Profiler instance;

    return instance;
}

Profiler::Profiler() { }

void Profiler::startMain() {
    started["MAIN"] = tick_micro();
}

void Profiler::stopMain() {
    calls["MAIN"] += 1;
    total["MAIN"] += tick_micro() - started["MAIN"];
    average["MAIN"] = total["MAIN"] / calls["MAIN"];
}

void Profiler::start(std::string s) {
    auto tick = tick_micro();
    string parent;
    for (auto const &i : started) {
        if (i.second > 0 && i.first != s && i.first != "MAIN") {
            s = i.first + ":" + s;
            parent = i.first;
            break;
        }
    }

    started[s] = tick;

    // reduce parent of overhead from this method
    total[parent] -= tick_micro() - tick;
}

void Profiler::stop(std::string s) {
    auto tick = tick_micro();
    string parent;
    for (auto const &i : started) {
        if (i.second > 0 && i.first != s && i.first != "MAIN") {
            s = i.first + ":" + s;
            parent = i.first;
            break;
        }
    }

    calls[s] += 1;
    total[s] += tick_micro() - started[s];
    average[s] = total[s] / calls[s];
    started[s] = 0;

    // reduce parent of overhead from this method
    total[parent] -= tick_micro() - tick;
}

void Profiler::report(std::string s) {
    cout.width(30);
    cout << left << s;

    cout << "calls: ";
    cout.width(6);
    cout << left << calls[s];

    cout << "calls in MAIN: ";
    cout.width(4);
    cout << left << static_cast<float>(calls[s] / calls["MAIN"]);

    cout << "total: ";
    cout.width(7);
    cout << right << to_string(total[s] / 1000) + "ms" << "  / ";
    stringstream time;
    time << fixed << setprecision(2) <<
        float(total[s]) / total["MAIN"] * 100 << "%";
    cout.width(7);
    cout << right << time.str() << "   ";

    cout << "average: ";
    cout.width(8);
    cout << right << to_string(average[s]) + "µs";

    cout << endl;
}

void Profiler::report() {
    for (auto const &i : started) {
        report(i.first);
    }
    cout << endl;
}

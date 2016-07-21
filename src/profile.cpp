#include "profile.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

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

std::string Profiler::getUniqueID(std::string s) {
    auto tick = tick_micro();

    vector<pair<string, uint32_t>> parents;
    for (auto const &i : started) {
        auto id = i.first;
        if (i.second > 0 && id.find(s) == string::npos && id != "MAIN") {
            parents.push_back(i);
        }
    }
    sort(parents.begin(), parents.end(),
            [](pair<string, uint32_t> p, pair<string, uint32_t> p2)
                { return p.second > p2.second; });

    auto overhead = tick_micro() - tick;
    for (auto const &p : parents) {
        total[p.first] -= overhead;
    }

    if (parents.size() > 0) {
        return parents[0].first + ":" + s;
    } else {
        return s;
    }
}

void Profiler::startMain() {
    started["MAIN"] = tick_micro();
}

void Profiler::stopMain() {
    calls["MAIN"] += 1;
    total["MAIN"] += tick_micro() - started["MAIN"];
    average["MAIN"] = total["MAIN"] / calls["MAIN"];
}

void Profiler::start(std::string s) {
    auto id = getUniqueID(s);
    started[id] = tick_micro();
}

void Profiler::stop(std::string s) {
    auto id = getUniqueID(s);
    calls[id] += 1;
    total[id] += tick_micro() - started[id];
    average[id] = total[id] / calls[id];
    started[id] = 0;
}

void Profiler::report(std::string s) {
    cout.width(50);
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
    vector<pair<string, uint32_t>> times;
    for (auto const &i : total) {
        times.push_back(i);
    }
    sort(times.begin(), times.end(),
            [](pair<string, uint32_t> p, pair<string, uint32_t> p2)
                { return p.second > p2.second; });

    for (auto const &i : times) {
        report(i.first);
    }
    cout << endl;
}

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <vector>
#include <map>
#include <algorithm>

/*
 * Define QUICK_PROFILE to use no-overhead macros
 *
 * Feel free to simply call the Profiler::get().foo() versions if overhead
 * isn't an issue
 */

#ifdef QUICK_PROFILE
    #define START_PROFILE(x) (Profiler::get().start((x)))
    #define START_UNIQUE_PROFILE(x) (Profiler::get().startUnique((x)))
    #define STOP_PROFILE(x) (Profiler::get().stop((x)))
    #define STOP_UNIQUE_PROFILE(x) (Profiler::get().stopUnique((x)))
    #define START_MAIN_PROFILE() (Profiler::get().startMain())
    #define STOP_MAIN_PROFILE() (Profiler::get().stopMain())
    #define REPORT_PROFILE(x) (Profiler::get().report((x)))
    #define REPORT_ALL_PROFILE() (Profiler::get().report())
#else
    #define START_PROFILE(x)
    #define START_UNIQUE_PROFILE(x)
    #define STOP_PROFILE(x)
    #define STOP_UNIQUE_PROFILE(x)
    #define START_MAIN_PROFILE()
    #define STOP_MAIN_PROFILE()
    #define REPORT_PROFILE(x)
    #define REPORT_ALL_PROFILE()
#endif

class Profiler {
    std::map<std::string, uint32_t> started;
    std::map<std::string, uint32_t> calls;
    std::map<std::string, uint32_t> total;
    std::map<std::string, uint32_t> average;
    int main_loops = 0;
public:
    inline static Profiler& get() {
        static std::map<std::thread::id, Profiler> instances;

        auto id = std::this_thread::get_id();
        if (instances.find(id) == instances.end()) {
            instances.emplace(id, Profiler());
        }

        return instances.at(id);
    }

    inline void startMain() {
        // first loop to start
        if (main_loops == 0) {
            started["MAIN"] = tick_micro();
        }
        main_loops++;
    }

    inline void stopMain() {
        // first loop to start
        if (main_loops == 1) {
            calls["MAIN"] += 1;
            total["MAIN"] += tick_micro() - started["MAIN"];
            average["MAIN"] = total["MAIN"] / calls["MAIN"];
        }
        main_loops--;
    }

    inline void start(std::string s) {
        startUnique(getUniqueID(s));
    }

    inline void startUnique(std::string s) {
        started[s] = tick_micro();
    }

    inline void stop(std::string s) {
        stopUnique(getUniqueID(s));
    }

    inline void stopUnique(std::string s) {
        calls[s] += 1;
        total[s] += tick_micro() - started[s];
        average[s] = total[s] / calls[s];
        started[s] = 0;
    }

    inline void report(std::string s) {
        if (calls["MAIN"] == 0) {
            return;
        }

        int max = 30;
        for (auto const &i : total) {
            if (i.first.length() > max) {
                max = i.first.length() + 2;
            }
        }

        std::cout.width(max);
        std::cout << std::left << s;

        std::cout << "calls: ";
        std::cout.width(6);
        std::cout << std::left << calls[s];

        std::cout << "calls in MAIN: ";
        std::cout.width(4);
        std::cout << std::left << std::ceil(static_cast<float>(calls[s]) / calls["MAIN"]);

        std::cout << "total: ";
        std::cout.width(7);
        std::cout << std::right << std::to_string(total[s] / 1000) + "ms" << "  / ";
        std::stringstream time;
        time << std::fixed << std::setprecision(2) <<
            float(total[s]) / total["MAIN"] * 100 << "%";
        std::cout.width(7);
        std::cout << std::right << time.str() << "   ";

        std::cout << "average: ";
        std::cout.width(8);
        std::cout << std::right << std::to_string(average[s]) + "µs";

        std::cout << std::endl;
    }

    inline void report() {
        if (calls["MAIN"] == 0) {
            return;
        }

        std::vector<std::pair<std::string, uint32_t>> times;
        for (auto const &i : total) {
            times.push_back(i);
        }
        std::sort(times.begin(), times.end(),
            [](std::pair<std::string, uint32_t> p, std::pair<std::string, uint32_t> p2)
                    { return p.second > p2.second; });

        for (auto const &i : times) {
            report(i.first);
        }
        std::cout << std::endl;
    }

private:
    Profiler() {}

    inline static uint32_t tick_micro() {
        struct timespec timespec_time;

        clock_gettime(CLOCK_MONOTONIC, &timespec_time);

        uint32_t tick = timespec_time.tv_nsec / 1000;
        tick += timespec_time.tv_sec * 1000000;
        return tick;
    }

    inline std::string getUniqueID(std::string s) {
        auto tick = tick_micro();

        std::vector<std::pair<std::string, uint32_t>> parents;
        for (auto const &i : started) {
            auto id = i.first;
            if (i.second > 0 && id.find(s) == std::string::npos && id != "MAIN") {
                parents.push_back(i);
            }
        }
        sort(parents.begin(), parents.end(),
            [](std::pair<std::string, uint32_t> p, std::pair<std::string, uint32_t> p2)
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
};

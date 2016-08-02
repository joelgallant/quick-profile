#pragma once

#include <map>
#include <string>

class Profiler {
    std::map<std::string, uint32_t> started;
    std::map<std::string, uint32_t> calls;
    std::map<std::string, uint32_t> total;
    std::map<std::string, uint32_t> average;
public:
    static Profiler& get();

    void startMain();
    void stopMain();
    void start(std::string s);
    void startUnique(std::string s);
    void stop(std::string s);
    void stopUnique(std::string s);
    void report(std::string s);
    void report();
private:
    Profiler();

    std::string getUniqueID(std::string s);
};

#define START_PROFILE(x) (Profiler::get().start((x)))
#define START_UNIQUE_PROFILE(x) (Profiler::get().startUnique((x)))
#define STOP_PROFILE(x) (Profiler::get().stop((x)))
#define STOP_UNIQUE_PROFILE(x) (Profiler::get().stopUnique((x)))
#define START_MAIN_PROFILE() (Profiler::get().startMain())
#define STOP_MAIN_PROFILE() (Profiler::get().stopMain())
#define REPORT_PROFILE(x) (Profiler::get().report((x)))
#define REPORT_ALL_PROFILE() (Profiler::get().report())

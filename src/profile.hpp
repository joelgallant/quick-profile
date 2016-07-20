#pragma once

#include <map>
#include <string>

class Profiler {
    std::map<std::string, int> started;
    std::map<std::string, int> calls;
    std::map<std::string, int> totalTime;
    std::map<std::string, int> averageTime;
public:
    static Profiler& get();

    void startMain();
    void stopMain();
    void start(std::string s);
    void stop(std::string s);
    void report(std::string s);
    void report();
private:
    Profiler();
};

#define START_PROFILE(x) (Profiler::get().start((x)))
#define STOP_PROFILE(x) (Profiler::get().stop((x)))
#define START_MAIN_PROFILE() (Profiler::get().startMain())
#define STOP_MAIN_PROFILE() (Profiler::get().stopMain())
#define REPORT_PROFILE(x) (Profiler::get().report((x)))
#define REPORT_ALL_PROFILE() (Profiler::get().report())

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

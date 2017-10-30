#pragma once
#define INTERCEPT_SQF_STRTYPE_RSTRING
#include <chrono>
#include <client/headers/shared/types.hpp>

namespace chrono {
    using nanoseconds = std::chrono::duration<double, std::nano>;
    using microseconds = std::chrono::duration<double, std::micro>;
    using milliseconds = std::chrono::duration<double, std::milli>;
    using seconds = std::chrono::duration<double>;
}



enum class profileElementType {
    scope,
    log
};


class profileElement {
public:
    std::vector<std::shared_ptr<profileElement>> subelements;
    size_t curElement = 0; //Used for iterating over subelements
    profileElementType type;
    profileElement* parent = nullptr;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    profileElement(profileElementType _type) : type(_type){}

    virtual ~profileElement() {}
    virtual intercept::types::r_string getAsString() = 0;

    virtual std::chrono::high_resolution_clock::time_point getStartTime() { return start; }
    virtual chrono::microseconds getRunTime() = 0;

};

class profileScope : public profileElement {
public:
    ~profileScope() override;
    profileScope(uint64_t _scopeID) : profileElement(profileElementType::scope), scopeID(_scopeID) {}
    intercept::types::r_string getAsString() override;
    chrono::microseconds getRunTime() override {return runtime;}
    chrono::microseconds runtime;
    uint64_t scopeID;
    intercept::types::r_string name;
};

class profileLog : public profileElement {
public:
    profileLog(intercept::types::r_string _message) : profileElement(profileElementType::log), message(_message) {}
    ~profileLog() override;
    intercept::types::r_string getAsString() override;
    chrono::microseconds getRunTime() override { return chrono::microseconds(0); }
    intercept::types::r_string message;
};


class scriptProfiler {
public:
    scriptProfiler();
    ~scriptProfiler();
    void preStart();
    void preInit();
    //Starts new scope and returns it's assigned scopeID
    uint64_t startNewScope();
    void endScope(uint64_t scopeID, intercept::types::r_string name, chrono::microseconds runtime);
    void addLog(intercept::types::r_string msg);
    void iterateElementTree(std::function<void(profileElement*, size_t)>);
    intercept::types::r_string generateLog();
    std::vector<std::shared_ptr<profileElement>> elements; //tree structure
    std::map<uint64_t, std::shared_ptr<profileScope>> scopes; //map of scopes
    uint64_t lastScopeID = 1;
    profileScope* currentScope;
    std::chrono::high_resolution_clock::time_point frameStart;
};

inline scriptProfiler profiler{};

#include "scriptProfiler.h"
#include <client/headers/intercept.hpp>
#include <sstream>
#include <numeric>

using namespace intercept;
using namespace std::chrono_literals;
static sqf_script_type GameDataProfileScope_type;

class GameDataProfileScope : public game_data {

public:
    GameDataProfileScope() {}
    GameDataProfileScope(r_string _name) : name(_name), start(std::chrono::high_resolution_clock::now()), scopeID(profiler.startNewScope()) {}
    void lastRefDeleted() const override { delete this; }
    const sqf_script_type& type() const override { return GameDataProfileScope_type; }
    ~GameDataProfileScope() override {
        auto timeDiff = std::chrono::high_resolution_clock::now() - start;
        auto runtime = std::chrono::duration_cast<chrono::microseconds>(timeDiff);
        profiler.endScope(scopeID, name, runtime);
    }

    bool get_as_bool() const override { return true; }
    float get_as_number() const override { return 0.f; }
    const r_string& get_as_string() const override { return name; }
    game_data* copy() const override { return new GameDataProfileScope(*this); } //#TODO is copying scopes even allowed?!
    r_string to_string() const override { return name; }
    //virtual bool equals(const game_data*) const override;
    const char* type_as_string() const override { return "profileScope"; }
    bool is_nil() const override { return false; }
    bool can_serialize() override { return true; }//Setting this to false causes a fail in scheduled and global vars

    serialization_return serialize(param_archive& ar) override {
        game_data::serialize(ar); //This is fake. We can't be serialized. But I don't want errors.
        return serialization_return::no_error;
    }
    r_string name;
    std::chrono::high_resolution_clock::time_point start;
    uint64_t scopeID = -1;
};

game_data* createGameDataProfileScope(param_archive* ar) {
    //#TODO use armaAlloc
    auto x = new GameDataProfileScope();
    if (ar)
        x->serialize(*ar);
    return x;
}

game_value createProfileScope(game_value name) {
    return game_value(new GameDataProfileScope(name));
}

game_value profilerSleep() {
    std::this_thread::sleep_for(17ms);
    return {};
}

game_value profilerCaptureFrame() {
    profiler.record = true;
    profiler.forceCapture = true;
    return {};
}

game_value profilerCaptureSlowFrame(game_value threshold) {
    profiler.record = true;
    profiler.slowCheck = chrono::milliseconds(static_cast<float>(threshold));
    return {};
}

game_value profilerLog(game_value message) {
    if (profiler.record) {
        profiler.addLog(message);
    }
    return {};
}


scriptProfiler::scriptProfiler() {}


scriptProfiler::~scriptProfiler() {}


void scriptProfiler::preStart() {
    auto codeType = client::host::registerType("ProfileScope"sv, "ProfileScope"sv, "Dis is a profile scope. It profiles things."sv, "ProfileScope"sv, createGameDataProfileScope);
    GameDataProfileScope_type = codeType.second;
    static auto _createProfileScope = client::host::registerFunction("createProfileScope", "Creates a ProfileScope", userFunctionWrapper<createProfileScope>, codeType.first, GameDataType::STRING);
    static auto _profilerSleep = client::host::registerFunction("profilerBlockingSleep", "Pauses the engine for 17ms. Used for testing.", userFunctionWrapper<profilerSleep>, GameDataType::NOTHING);
    static auto _profilerCaptureFrame = client::host::registerFunction("profilerCaptureFrame", "Captures the next frame", userFunctionWrapper<profilerCaptureFrame>, GameDataType::NOTHING);
    static auto _profilerCaptureSlowFrame = client::host::registerFunction("profilerCaptureSlowFrame", "Captures the first frame that hits the threshold in ms", userFunctionWrapper<profilerCaptureSlowFrame>, GameDataType::NOTHING, GameDataType::SCALAR);
    static auto _profilerLog = client::host::registerFunction("profilerLog", "Logs message to capture", userFunctionWrapper<profilerLog>, GameDataType::NOTHING, GameDataType::STRING);
}

void scriptProfiler::preInit() {
    static auto EHHandle = client::addMissionEventHandler<client::eventhandlers_mission::Draw3D>([this]() {
        if (record && shouldCapture()) { //We always want to log if a capture is ready don't we?
            if (currentScope == nullptr)
                capture();
            else
                waitingForCapture = true; //Wait till we left all scopes
        }
        if (record && !waitingForCapture) {//If we are waiting for capture don't clear everything
            frameStart = std::chrono::high_resolution_clock::now();
            elements.clear(); //this is recursive but not a problem because only scopes can have sub elements > 1 deep. And they are still held alive by scopes array.
            scopes.clear(); //#TODO recursive...
        }

    });
}

uint64_t scriptProfiler::startNewScope() {
    if (!record) return -1;
    auto newScopeID = lastScopeID++;
    auto newScope = std::make_shared<profileScope>(newScopeID);
    scopes[newScopeID] = newScope;
    if (currentScope) {
        currentScope->subelements.push_back(newScope);
        newScope->parent = currentScope;
    } else {
        elements.push_back(newScope);
    }
    currentScope = newScope.get();
    return newScopeID;
}

void scriptProfiler::endScope(uint64_t scopeID, intercept::types::r_string name, chrono::microseconds runtime) {
    if (!record) return;
    auto& scope = scopes[scopeID];
    scope->name = name;
    scope->runtime = runtime;
    if (currentScope) {
        if (currentScope != scope.get()) __debugbreak(); //wut?
        currentScope = dynamic_cast<profileScope*>(scope->parent);
        if (scope->parent && !currentScope) __debugbreak(); //wutwatwut?!! 
    } else if (waitingForCapture) {
        capture();
    }
}

void scriptProfiler::addLog(intercept::types::r_string msg) {
    auto newLog = std::make_shared<profileLog>(msg);
    if (currentScope) {
        currentScope->subelements.push_back(newLog);
        newLog->parent = currentScope;
    } else {
        elements.push_back(newLog);
    }
}

void scriptProfiler::iterateElementTree(std::function<void(profileElement*, size_t)> func) {
    //https://stackoverflow.com/a/5988138
    for (auto& element : elements) {
        profileElement* node = element.get();
        size_t depth = 0;
        while (node) {
            if (node->curElement <= node->subelements.size()) {
                profileElement* prev = node;
                if (node->subelements[node->curElement]) {
                    node = node->subelements[node->curElement].get();
                }
                if (prev->curElement == 0) depth++;
                prev->curElement++;
            } else {
                func(node, depth);
                node->curElement = 0; // Reset counter for next traversal.
                node = node->parent;
                depth--;
            }
        }
    }



}

intercept::types::r_string scriptProfiler::generateLog() {
    //https://pastebin.com/raw/4gfJSwdB
    //#TODO don't really want scopes empty. Just doing it to use it as time reference
    if (elements.empty() || scopes.empty()) return r_string();
    std::stringstream output;
    auto baseTimeReference = scopes.begin()->second->start;
    chrono::milliseconds totalRuntime = std::chrono::duration_cast<chrono::milliseconds>(std::chrono::high_resolution_clock::now() - baseTimeReference);

    output << "* THREAD! YEAH!\n";
    output << "total; " << 0.0 << "; " << totalRuntime.count() << ";\"Frame " << sqf::diag_frameno() << "\"";


    auto iterateFunc = [&output, &baseTimeReference](profileElement* element, size_t depth) {
        for (size_t i = 0; i < depth + 1; ++i) {
            output << " ";
        }
        chrono::milliseconds startTime = std::chrono::duration_cast<chrono::milliseconds>(element->getStartTime() - baseTimeReference);
        switch (element->type) {

        case profileElementType::scope:
        {
            output << "scope; " << startTime.count() << "; " << std::chrono::duration_cast<chrono::milliseconds>(element->getRunTime()).count() << ";\"" << element->getAsString() << "\"";
        }
        break;
        case profileElementType::log:
        {
            output << "scope; " << startTime.count() << "; " << 0.0 << ";\"" << element->getAsString() << "\"";
        }
        break;
        }

    };

    iterateElementTree(iterateFunc);

    return r_string(output.str());
}

chrono::milliseconds scriptProfiler::totalScriptRuntime() {
    return std::accumulate(elements.begin(), elements.end(), chrono::milliseconds(0), [](const std::shared_ptr<profileElement>& element) -> chrono::milliseconds {
        if (element->type != profileElementType::scope) return chrono::milliseconds(0);
        return element->getRunTime();
    });
}

bool scriptProfiler::shouldCapture() {
    if (forceCapture) return true;
    if (elements.empty()) return false;
    if (slowCheck.count() != 0.0) return (totalScriptRuntime() > slowCheck);
    return false;
}

void scriptProfiler::capture() {
    auto log = generateLog();
    sqf::copy_to_clipboard(log);
    forceCapture = false;
    record = false;
    waitingForCapture = false;
}

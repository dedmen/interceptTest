#include "scriptProfiler.h"
#include <client/headers/intercept.hpp>
#include <sstream>

using namespace intercept;
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
        profiler.endScope(scopeID,name,runtime);    
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


profileScope::~profileScope() {}
intercept::types::r_string profileScope::getAsString() {
    return name;
}
profileLog::~profileLog() {}
intercept::types::r_string profileLog::getAsString() {
    return message;
}
scriptProfiler::scriptProfiler() {}


scriptProfiler::~scriptProfiler() {}


void scriptProfiler::preStart() {
    auto codeType = client::host::registerType("ProfileScope"sv, "ProfileScope"sv, "Dis is a profile scope. It profiles things."sv, "ProfileScope"sv, createGameDataProfileScope);
    GameDataProfileScope_type = codeType.second;
    static auto _createProfileScope = client::host::registerFunction("createProfileScope", "Creates a ProfileScope", userFunctionWrapper<createProfileScope>, codeType.first, GameDataType::STRING);



}
void scriptProfiler::preInit() {
    static auto EHHandle = client::addMissionEventHandler<client::eventhandlers_mission::Draw3D>([this]() {
        auto log = generateLog();
        sqf::copy_to_clipboard(log);
        frameStart = std::chrono::high_resolution_clock::now();
        elements.clear(); //this is recursive but not a problem because only scopes can have sub elements > 1 deep. And they are still held alive by scopes array.
        scopes.clear(); //#TODO recursive...
    });
}

uint64_t scriptProfiler::startNewScope() {
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
    auto& scope = scopes[scopeID];
    scope->name = name;
    scope->runtime = runtime;
    if (currentScope) {
        if (currentScope != scope.get()) __debugbreak(); //wut?
        currentScope = dynamic_cast<profileScope*>(scope->parent);
        if (scope->parent && !currentScope) __debugbreak(); //wutwatwut?!! 
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
                depth --;
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
    output << "total; " << 0.0 << "; " << totalRuntime.count() <<";\"Frame "<< sqf::diag_frameno() << "\"";


    auto iterateFunc = [&output, &baseTimeReference](profileElement* element, size_t depth) {
        for (size_t i = 0; i < depth+1; ++i) {
            output << " ";
        }
        chrono::milliseconds startTime = std::chrono::duration_cast<chrono::milliseconds>(element->getStartTime() - baseTimeReference);
        switch (element->type) {

            case profileElementType::scope: {
                output << "scope; " << startTime.count() << "; " << std::chrono::duration_cast<chrono::milliseconds>(element->getRunTime()).count() << ";\"" << element->getAsString() <<"\"";
            }
            break;
            case profileElementType::log: {
                output << "scope; " << startTime.count() << "; " << 0.0 << ";\"" << element->getAsString() << "\"";
            }
            break;
        }
        
    };

    iterateElementTree(iterateFunc);

    return r_string(output.str());
}

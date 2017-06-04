#include "LuaManager.h"
#include <client/headers/intercept.hpp>
using namespace intercept;

types::registered_sqf_function _execLua;
types::registered_sqf_function _compileLua;
types::registered_sqf_function _callLuaString;
types::registered_sqf_function _callLuaCodeArgs;
types::registered_sqf_function _callLuaCode;

//LuaEdit debugger
struct lua_State;

// LuaEdit API
void StartLuaEditRemoteDebugger(int aPort = 32201, lua_State* L = NULL);
void StopLuaEditRemoteDebugger();
const char* GetCurrentLuaVersion();
bool CheckLuaScriptSyntax(const char* aScript, const char* aScriptName, char* aErrBuf, int aErrBufLen);

static sqf_script_type GameDataLuaCode_type;

class GameDataLuaCode : public game_data {

public:
    explicit GameDataLuaCode(sol::protected_function f) : code(f) {}
    GameDataLuaCode(sol::protected_function f, r_string codeString) : code(f), code_string(codeString) {}
    GameDataLuaCode() {}
    void lastRefDeleted() const override { delete this; }
    const sqf_script_type& type() const override { return GameDataLuaCode_type; }
    ~GameDataLuaCode() override {};

    bool get_as_bool() const override { return true; }
    float get_as_number() const override { return 0.f; }
    r_string get_as_string() const override { return r_string(); }
    game_data* copy() const override { return new GameDataLuaCode(*this); }
    r_string to_string() const override { return code_string;}
    //virtual bool equals(const game_data*) const override;
    const char* type_as_string() const override { return "luaCode"; }
    bool is_nil() const override { return false; }

    sol::protected_function code;
    r_string code_string;
};

game_data* createGameDataLuaCode(void*) {
    return new GameDataLuaCode();
}



class lua_object {
public:
    lua_object(object o) : obj(o){}
    object obj;
    std::string getName() const {
        return sqf::name(obj);
    }
};

sol::object system_chat(sol::object a, sol::this_state s) {
    sol::state_view lua(s);
    if (a.is<std::string>()) {
        sqf::system_chat(a.as<std::string>());
    }
    return sol::make_object(lua, sol::nil);
}
sol::object player(sol::this_state s) {
    sol::state_view lua(s);
    return sol::object(lua, sol::in_place<lua_object>, lua_object(sqf::player()));
}
std::vector<sol::object> allPlayers(sol::this_state s) {
    sol::state_view lua(s);
    std::vector<sol::object> ret;
    for (auto& it : sqf::all_players()) {
        ret.emplace_back(sol::object(lua, sol::in_place<lua_object>, it));
    }
    return ret;
}


sol::object getVariable(sol::object a, sol::this_state s) {
    sol::state_view lua(s);
    std::vector<sol::object> ret;
    if (a.is<std::string>()) {
        return sol::object(lua, sol::in_place<lua_object>, sqf::get_variable(sqf::mission_namespace(),a.as<std::string>()));
    }
    return  sol::make_object(lua, sol::nil);
}

game_value executeLua(game_value leftArg, game_value rightArg) {
    //auto source = sqf::load_file(rightArg);

    lua.state["_this"] = sol::object(lua.state, sol::in_place, static_cast<std::string>(leftArg));
    try {
        auto result = lua.state.do_file(rightArg);
        std::string ret = result;
        return ret;
    } catch (sol::error err) {
        return err.what();
    }
}

game_value compileLua(game_value rightArg) {
    auto result = lua.state.load(rightArg);
    if (result.valid()) {
        return game_value(new GameDataLuaCode(result.get<sol::protected_function>(), rightArg));
    }
    return game_value();
}

game_value compileLuaFromFile(game_value rightArg) {
    std::string code(sqf::preprocess_file(rightArg));
    auto result = lua.state.load_buffer(code.c_str(),code.length(),(std::string("@")+static_cast<std::string>(rightArg)).c_str());
    if (result.valid()) {
        return game_value(new GameDataLuaCode(result.get<sol::protected_function>(),rightArg));
    }
    return game_value();
}   

game_value callLua_String(game_value leftArg, game_value rightArg) {
    std::string ret = lua.state[static_cast<std::string>(rightArg)](sol::object(lua.state, sol::in_place, static_cast<std::string>(leftArg)));
    return ret;
}

game_value callLua_Code(game_value leftArg, game_value rightArg) {
    auto code = static_cast<GameDataLuaCode*>(rightArg.data.getRef());
    std::string ret = code->code(sol::object(lua.state, sol::in_place, static_cast<std::string>(leftArg)));
    return ret;
}

game_value callLua_Code(game_value rightArg) {
    auto code = static_cast<GameDataLuaCode*>(rightArg.data.getRef());
    std::string ret = code->code();
    return ret;
}

LuaManager::LuaManager() : state{} {

}


LuaManager::~LuaManager() {}


types::registered_sqf_function _compileLuaFromFile;

void LuaManager::preStart() {
    auto codeType = client::host::registerType(r_string("LUACODE"), r_string("luaCode"), r_string("Dis is LUA!"), r_string("luaCode"), createGameDataLuaCode);
    GameDataLuaCode_type = codeType.second;
    _execLua = client::host::registerFunction("execLUA", "Loads, compiles and executes given Lua file", userFunctionWrapper<executeLua>, GameDataType::ANY, GameDataType::ANY, GameDataType::STRING);
    _compileLua = client::host::registerFunction("compileLUA", "Compiles Lua string", userFunctionWrapper<compileLua>, codeType.first, GameDataType::STRING);
    _compileLuaFromFile = client::host::registerFunction("compileLUAFromFile", "Preprocesses and compiles LUA from file. Setting source information in case of errors.", userFunctionWrapper<compileLuaFromFile>, codeType.first, GameDataType::STRING);
    _callLuaString = client::host::registerFunction("callLUA", "Call Named lua function in global Namespace", userFunctionWrapper<callLua_String>, GameDataType::ANY, GameDataType::ANY, GameDataType::STRING);
    _callLuaCodeArgs = client::host::registerFunction("call", "Call compiled lua code", userFunctionWrapper<callLua_Code>, GameDataType::ANY, GameDataType::ANY, codeType.first);
    _callLuaCode = client::host::registerFunction("call", "Call compiled lua code", userFunctionWrapper<callLua_Code>, GameDataType::ANY, codeType.first);
    
    state.open_libraries();
    state["systemChat"] = &system_chat;
    state["getVariable"] = &getVariable;
    state["allPlayers"] = &allPlayers;
    state["player"] = &player;
    state.new_usertype<lua_object, object>("object", "name", &lua_object::getName);
    lua.state.do_file("P:\\test.luac");
    //StartLuaEditRemoteDebugger(32201, state);
}

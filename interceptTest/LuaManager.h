#pragma once

#include "sol.hpp"
#include <client/headers/intercept.hpp>

class LuaManager {
public:
    LuaManager();
    ~LuaManager();
    void preStart();

    sol::state state;
};
extern LuaManager lua;

struct lua_iface {
    intercept::types::r_string(*blubTest)();
};

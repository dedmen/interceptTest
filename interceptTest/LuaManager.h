#pragma once

#include "sol.hpp"

class LuaManager {
public:
    LuaManager();
    ~LuaManager();
    void preStart();

    sol::state state;
};
extern LuaManager lua;

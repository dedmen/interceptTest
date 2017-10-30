#include <stdio.h>
#include <cstdint>
#include <sstream>
#include <experimental/string>

#include "tools.h"
#include <queue>
#include "TFAR.h"
#include "cba.h"
#include "magGroupTest.h"
#include "scriptProfiler.h"

using namespace intercept;
types::registered_sqf_function _interceptEventFunction;
types::registered_sqf_function _interceptEventFunction2;
types::registered_sqf_function _interceptEventFunction3;
types::registered_sqf_function _interceptEventFunction4;
types::registered_sqf_function _interceptEventFunction5;
types::registered_sqf_function _interceptEventFunction6;

// required exported function to return API version
int __cdecl intercept::api_version() {
    return 1;
}
#include <chrono>
using namespace std::chrono;
class thready {
public:
    thready() : th(new std::thread([&]() {
        run(); })) {

    }
    void run() {
        std::this_thread::sleep_for(5s);
        while (shouldRun) {
            client::invoker_lock thread_lock;
            auto player = intercept::sqf::player();
            intercept::sqf::set_dir(player, intercept::sqf::direction(player) + 5);
            std::this_thread::sleep_for(1ms);
        };
    }
    ~thready() {
        shouldRun = false;
        if (th)  th->join();
    }

    bool shouldRun = true;
    std::thread* th = nullptr;


};
thready* globalThready;
TFAR pTFAR;
#include <random>
std::random_device rd;

std::mt19937 e2(rd());

vector3 randomOffset(float maxOffset) {
    std::uniform_real_distribution<float> dist(-maxOffset, maxOffset);
    return{ dist(e2),dist(e2), dist(e2) };
}


game_value redirectWrap(game_value left_arg, game_value right_arg) {
    return u8"heööai?";
}
game_value redirectWrapUnary(game_value right_arg) {
    return {};// TFAR::TFAR_fnc_preparePositionCoordinates(std::move(right_arg));
}
game_value redirectWrapNular() {
    types::auto_array<types::game_value> output;

    auto allUnits = sqf::all_units();
    output.reserve(allUnits.size());

    for (auto& unit : allUnits) {
        output.emplace_back(reinterpret_cast<game_data_object*>(unit.data.getRef())->get_position_matrix()._position);
    }

    return std::move(output);
}

game_value redirectWrapNularScript() {
    types::auto_array<types::game_value> output;

    auto allUnits = sqf::all_units();
    output.reserve(allUnits.size());

    for (auto& unit : allUnits) {
        output.emplace_back(sqf::get_pos(unit));
    }

    return std::move(output);
}

std::vector<types::registered_sqf_function> funcs;

// given a function that generates a random character,
// return a string of the requested length
std::string random_string(size_t length, std::function<char(void)> rand_char) {
    std::string str(length, 0);
    std::generate_n(str.begin(), length, rand_char);
    return str;
}

intercept::types::registered_sqf_function _binaryFuncOne;

intercept::types::game_value binaryFuncOne(intercept::types::game_value left_arg, intercept::types::game_value right_arg) {
    return "Binary Return";
}
#include "LuaManager.h"
LuaManager lua;


void addListItem(r_string name, control ctrl, r_string configEntry) {
    static std::map<size_t, std::tuple<r_string,r_string,r_string,int>> cacheMap;
    auto getFromCache = [&]() {
        auto cacheName = intercept::types::__internal::pairhash(configEntry,name);
        auto found = cacheMap.find(cacheName);
        if (found != cacheMap.end())
            return found->second;
        auto cfgClass = sqf::config_entry() >> configEntry >> name;
        r_string dlcName;
        int modID = 1; //dummy because we can't pushBack to gameVariable

        auto _addons = sqf::config_source_addon_list(cfgClass);
        if (!_addons.empty()) {
            auto _mods = sqf::config_source_mod_list(sqf::config_entry() >> "CfgPatches" >> _addons[0]);
            if (!_mods.empty()) {
                dlcName = _mods[0];
            }
        }
        r_string modIcon;
        if (dlcName.length()) {
            auto modParams = sqf::mod_params(dlcName, sqf::mod_params_options::logo);
            if (!modParams.empty())
            modIcon = modParams[0];
        }


        std::tuple<r_string, r_string, r_string, int> result{
            sqf::get_text(cfgClass >> "displayName"),
            sqf::get_text(cfgClass >> "picture"),
            modIcon,
            modID
        };
        cacheMap[cacheName] = result;
        return result;
    };

    auto item = getFromCache();

    auto _lbAdd = sqf::lb_add(ctrl, std::get<0>(item));

    sqf::lb_set_data(ctrl, _lbAdd, name);
    sqf::lb_set_picture(ctrl, _lbAdd, std::get<1>(item));
    //sqf::lb_set_picture_right(ctrl, _lbAdd, std::get<2>(item));
    sqf::lb_set_value(ctrl, _lbAdd, std::get<3>(item));
    sqf::lb_set_tooltip(ctrl, _lbAdd, sqf::format({ "%1\n%2", std::get<0>(item), name}));
}

game_value addListItems_acearsenal(intercept::types::game_value left_arg, intercept::types::game_value right_arg) {
   
    auto& arr = left_arg[0].to_array();
    r_string listName = left_arg[1];


    for (r_string name : arr) {
        addListItem(name, right_arg, listName);
    }
    return {};
}


#pragma region railScopeHeightHack

float frailHeight = 0;
float fmuzzleHeight = 0;
float fscopeCenter = 0;
float fscopeCenter2 = 0;
float fironCenter = 0;
uint32_t scopeCenterSwapFlag = 0;
uintptr_t railHeightJmpBack;
uintptr_t muzzleHeightJmpBack;
uintptr_t scopeCenterJmpBack;
uintptr_t ironCenterJmpBack;

void __declspec(naked) railHeight() {
    __asm
    {
        //mov         ebp, offset frailHeight;
        //mov         esi, [eax + 0x28];
        //mov         [ebp], esi;
        movss       xmm0, [eax + 0x28];
        movss       frailHeight, xmm0;

        mov         ebp, dword ptr[ebx + 8Ch]
        xor         esi, esi
        test        ebp, ebp
        jmp         railHeightJmpBack//0201E08C
    }
}
void __declspec(naked) scopeCenter() {
    __asm
    {
        push        ecx
        movss       xmm0, [eax + 0x4]

        mov         ecx, scopeCenterSwapFlag
        test        ecx, ecx
        jz isZero
    isNotZero:
        movss       fscopeCenter2, xmm0
        jmp         checkOut
    isZero:
        movss       fscopeCenter, xmm0

    checkOut:
        xor         ecx, 1
        mov         scopeCenterSwapFlag, ecx
        pop         ecx

        movss       xmm0, dword ptr[eax]
        movss       dword ptr[esp + 144h], xmm0
        movss       xmm0, dword ptr[eax + 4]
        movss       dword ptr[esp + 148h], xmm0
        movss       xmm0, dword ptr[eax + 8]
        jmp         scopeCenterJmpBack //0201D7D9
    }
}

//void __declspec(naked) ironCenter() {
//    __asm
//    {
//        movss       xmm5, [eax + 0x4]
//        movss       fironCenter, xmm5
//
//        movss       xmm5, dword ptr[eax]
//        movss       [esp + 60h], xmm5
//        movss       xmm6, dword ptr[eax + 4]
//        movss       dword ptr[esp + 64h], xmm6
//        movss       xmm7, dword ptr[eax + 8]
//        jmp         ironCenterJmpBack// 00BADC2A 0x0 base
//    }
//}

void __declspec(naked) muzzleHeight() {
    __asm
    {
        //mov         eax, offset fmuzzleHeight;
        movss       xmm0, [esp + 0x70];
        movss       fmuzzleHeight, xmm0;
        //eax,ecx 

        movss       xmm0, dword ptr[esp + 6Ch]
        lea         eax, [esp + 20h]
        subss       xmm0, dword ptr[esp + 90h]
        push        eax
        lea         ecx, [esp + 88h]
        jmp         muzzleHeightJmpBack //0201D9C0
    }
}
uintptr_t placeHookTotalOffs(uintptr_t totalOffset, uintptr_t jmpTo) {
    DWORD dwVirtualProtectBackup;


    /*
    32bit
    jmp 0x123122
    0:  e9 1e 31 12 00          jmp    123123 <_main+0x123123>
    64bit
    FF 25 64bit relative
    */
    VirtualProtect(reinterpret_cast<LPVOID>(totalOffset), 5u, 0x40u, &dwVirtualProtectBackup);
    auto jmpInstr = reinterpret_cast<unsigned char *>(totalOffset);
    auto addrOffs = reinterpret_cast<unsigned int *>(totalOffset + 1);
    *jmpInstr = 0xE9;
    *addrOffs = jmpTo - totalOffset - 5;
    VirtualProtect(reinterpret_cast<LPVOID>(totalOffset), 5u, dwVirtualProtectBackup, &dwVirtualProtectBackup);
    return totalOffset + 5;
}

#include <Psapi.h>
#pragma comment (lib, "Psapi.lib")//GetModuleInformation

void addHook() {
    MODULEINFO modInfo = { 0 };
    HMODULE hModule = GetModuleHandle(NULL);
    GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
    auto engineBase = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
    auto engineSize = static_cast<uintptr_t>(modInfo.SizeOfImage);


    placeHookTotalOffs(0x0201E082 - 0x00FC0000 + engineBase, (uintptr_t) railHeight);
    placeHookTotalOffs(0x0201D7B9 - 0x00FC0000 + engineBase, (uintptr_t) scopeCenter);
    //placeHookTotalOffs(0x00BADC10 + engineBase, (uintptr_t) ironCenter);
    //placeHookTotalOffs(0x00BAD2C0 + engineBase, (uintptr_t) ironCenter);
    placeHookTotalOffs(0x0201D9A5 - 0x00FC0000 + engineBase, (uintptr_t) muzzleHeight);



    railHeightJmpBack = 0x0201E08C - 0x00FC0000 + engineBase;
    scopeCenterJmpBack = 0x0201D7D9 - 0x00FC0000 + engineBase;
    //ironCenterJmpBack = 0x00BADC2A + engineBase;
    //ironCenterJmpBack = 0x00BAD2DA + engineBase;
    muzzleHeightJmpBack = 0x0201D9C0 - 0x00FC0000 + engineBase;
}


/*
00FC0000
rail height modelLocal = 0201E082 [eax+0x28]
muzzleHeight = 0201D9A5 [esp+0x70]
scopecenter modellocal = 0201D7B9 [eax+0x4]
*/

#pragma endregion railScopeHeightHack



game_value addRailScopeHeightHook() {
    static bool alreadyAdded = false;
    if (alreadyAdded) return "NO!";
    addHook();
    alreadyAdded = true;
    return "success";
}

game_value getRailScopeStuff() {
    game_value ret{ fmuzzleHeight, frailHeight, fscopeCenter, fscopeCenter2, fironCenter };
    fmuzzleHeight = frailHeight = fscopeCenter = fscopeCenter2 = fironCenter = 1;
    return ret;
}

void __cdecl intercept::pre_start() {
    _binaryFuncOne = intercept::client::host::registerFunction(
        "aliveBinary",
        "This is a test binary command",
        userFunctionWrapper<binaryFuncOne>,
        GameDataType::STRING,
        GameDataType::ARRAY,
        GameDataType::ARRAY
    );
    //sqf::set_variable(sqf::mission_namespace(), "INTERCEPT_TFAR", true);
    //if (!globalThready) globalThready = new thready();
    const auto ch_set = std::vector<char >(
    { '0','1','2','3','4',
        '5','6','7','8','9',
        'A','B','C','D','E','F',
        'G','H','I','J','K',
        'L','M','N','O','P',
        'Q','R','S','T','U',
        'V','W','X','Y','Z',
        'a','b','c','d','e','f',
        'g','h','i','j','k',
        'l','m','n','o','p',
        'q','r','s','t','u',
        'v','w','x','y','z'
    });

    //1) create a non-deterministic random number generator      
    std::default_random_engine rng(std::random_device{}());

    //2) create a random number "shaper" that will give
    //   us uniformly distributed indices into the character set
    std::uniform_int_distribution<> dist(0, ch_set.size() - 1);

    //3) create a function that ties them together, to get:
    //   a non-deterministic uniform distribution from the 
    //   character set of your choice.

    //std::uniform_int_distribution<int> dist2(5, 20);
    //for (size_t i = 0; i < 500; i++) {
    //    auto length = dist2(rng);
    //    auto name = random_string(length, [ch_set, &dist, &rng]() {return ch_set[dist(rng)]; });
    //    funcs.push_back(intercept::client::host::registerFunction(name, "", userFunctionWrapper<redirectWrapUnary>, GameDataType::STRING, GameDataType::ARRAY));
    //}
    //for (size_t i = 0; i < 500; i++) {
    //    auto length = dist2(rng);
    //    auto name = random_string(length, [ch_set, &dist, &rng]() {return ch_set[dist(rng)]; });
    //    funcs.push_back(intercept::client::host::registerFunction(name, "", userFunctionWrapper<redirectWrapNular>, GameDataType::STRING));
    //}
    //for (size_t i = 0; i < 500; i++) {
    //    auto length = dist2(rng);
    //    auto name = random_string(length, [ch_set, &dist, &rng]() {return ch_set[dist(rng)]; });
    //    funcs.push_back(intercept::client::host::registerFunction(name, "", userFunctionWrapper<redirectWrap>, GameDataType::STRING, GameDataType::ARRAY, GameDataType::ARRAY));
    //}

    tools::init();
    lua.preStart();
    _interceptEventFunction = intercept::client::host::registerFunction("Intercept_TFAR_preparePositionCoordinates", "", userFunctionWrapper<redirectWrapUnary>, GameDataType::STRING, GameDataType::ARRAY);
    _interceptEventFunction3 = intercept::client::host::registerFunction("Intercept_loopTest", "", userFunctionWrapper<redirectWrapNular>, GameDataType::ARRAY);
    _interceptEventFunction4 = intercept::client::host::registerFunction("Intercept_loopTestScript", "", userFunctionWrapper<redirectWrapNularScript>, GameDataType::ARRAY);
    _interceptEventFunction2 = intercept::client::host::registerFunction("Intercept_TFAR_preparePositionCoordinates_test", "", userFunctionWrapper<redirectWrap>, GameDataType::STRING, GameDataType::OBJECT, GameDataType::ARRAY);
    _interceptEventFunction6 = intercept::client::host::registerFunction("addListItems_acearsenal", "", userFunctionWrapper<addListItems_acearsenal>, GameDataType::NOTHING, GameDataType::ARRAY, GameDataType::CONTROL);

    auto x = client::host::list_plugin_interfaces("lua_iface"sv);
    std::string modl(x.first);
    auto iface = client::host::request_plugin_interface("lua_iface"sv, 1);

    if (iface) {
        static_cast<lua_iface*>(*iface)->blubTest();
    }
    pTFAR.preStart();
    cba::preStart();
    magGroupTest::preStart();
    profiler.preStart();
    static auto _rHackinterceptEventFunction2 = intercept::client::host::registerFunction("addRailScopeHeightHook", "", userFunctionWrapper<addRailScopeHeightHook>, GameDataType::STRING);
    static auto _rHackinterceptEventFunction6 = intercept::client::host::registerFunction("getRailScopeStuff", "", userFunctionWrapper<getRailScopeStuff>, GameDataType::ARRAY);
}

void __cdecl intercept::pre_init() {
    pTFAR.preInit();
    cba::preInit();
    profiler.preInit();
}


void __cdecl intercept::post_init() {
    //sqf::set_variable(sqf::mission_namespace(), "INTERCEPT_TFAR", true);
    // if (!globalThready) globalThready = new thready();
    //_interceptEventFunction = intercept::client::host::functions.register_sqf_function(std::string("itfarprepcoords"), "", userFunctionWrapper<redirectWrap>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY, types::__internal::GameDataType::ARRAY);
    //_interceptEventFunction2 = intercept::client::host::functions.register_sqf_function_unary(std::string("itfarprepcoords"), "", userFunctionWrapper<redirectWrapUnary>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY);
    //_interceptEventFunction3 = intercept::client::host::functions.register_sqf_function_unary(std::string("itfarSendPInfo"), "", userFunctionWrapper<TFAR_fnc_sendPlayerInfo>, types::__internal::GameDataType::ARRAY, types::__internal::GameDataType::ARRAY);
    //_interceptEventFunction4 = intercept::client::host::functions.register_sqf_function_unary(std::string("itfarprocp"), "", userFunctionWrapper<TFAR_fnc_processPlayerPositions>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY);
    //sqf::set_variable(sqf::mission_namespace(), "INTERCEPT_TFAR", true);
    sqf::system_chat("TFAR Performance active");

    tools::postInit();
    cba::postInit();

              
    static auto _EH2 = client::addEventHandler<client::eventhandlers_object::Fired>(sqf::player(),[](types::object unit, types::r_string weapon, types::r_string muzzle, types::r_string mode, types::r_string ammo, types::r_string magazine, types::object projectile, types::object gunner)
    {
        auto p1 = ((game_data_object*) projectile.data.getRef())->get_position_matrix()._position;
        auto p2 = sqf::position_camera_to_world({ 0,0,0 });

        auto p3 = p2 - p1;


    });
}





class Variable {
public:
    Variable() {};
    Variable(std::string varName) : _namespace(intercept::sqf::mission_namespace()), _name(varName) {};//#TODO namespace/object and other types constructor argument
    auto get() {
        return intercept::sqf::get_variable(_namespace, _name);
    }
    void set(const game_value &gv, bool publicVar = false) {
        intercept::sqf::set_variable(_namespace, _name, gv);
        if (publicVar)
            intercept::sqf::public_variable(_name);
    }
    auto operator*() {
        return get();
    }
    auto operator=(const game_value &gv) {
        return set(gv, false);
    }
    /*
    auto getCache(maxTime){
    if (now - lastCache < maxTime)
        return _cached;
    else
        return get();
    }
    */
private:
    rv_namespace _namespace;
    std::string _name;
};

class Vehicle {
public:
    Vehicle(std::string classname, vector3 position) {
        _obj = intercept::sqf::create_vehicle(classname, position, {});
        intercept::sqf::set_pos(_obj, position);
        haveObj = true;
    }
    ~Vehicle() {
        if (haveObj) {
            haveObj = false;
            // intercept::sqf::delete_vehicle(_obj);
        }
    }
    void turn() {
        std::uniform_real_distribution<float> dist(0, 360);//Get random classname
        intercept::sqf::set_dir(_obj, dist(e2));   //intercept::sqf::direction(_obj) + 5
    }
    auto timeAlive() {
        return std::chrono::system_clock::now() - creation;
    }
    std::chrono::system_clock::time_point creation = std::chrono::system_clock::now();
    object _obj;
    bool haveObj;
};


Vehicle* vec = nullptr;
float getCameraZoom() {
    return sqf::world_to_screen(sqf::position_camera_to_world({ 0,3,4 })).distance({ .5f,.5f })*
        (sqf::get_resolution().ui_scale / 2.f);
}
__itt_string_handle* handle_task1 = __itt_string_handle_create("task1");
__itt_string_handle* handle_task2 = __itt_string_handle_create("task2");
__itt_string_handle* handle_task3 = __itt_string_handle_create("task3");
__itt_string_handle* handle_task4 = __itt_string_handle_create("task4");

// This function is exported and is called by the host each frame.
void __cdecl intercept::on_frame() {
    //auto _allUnits = sqf::all_units();
    //auto _player = sqf::player();
    //auto _group = sqf::get_group(_player);

    //auto start = std::chrono::high_resolution_clock::now();
    //__itt_task_begin(domain, __itt_null, __itt_null, handle_task1);
    //for (int Index = 0; Index < 32; Index++) {
    //    sqf::join(_allUnits, _group);
    //}
    //__itt_task_end(domain);
    //auto end = std::chrono::high_resolution_clock::now();


    /*
     return;
     if (!vec) {
         std::vector<std::string> classes{ "B_Quadbike_01_F","B_LSV_01_armed_F","B_Boat_Transport_01_F","B_Lifeboat" };
         std::uniform_int_distribution<int> dist(0, classes.size() - 1);//Get random classname
         vec = new Vehicle(classes.at(dist(e2)), intercept::sqf::get_pos(sqf::player()) + vector3{ 0,15,0 });
         intercept::sqf::set_variable(intercept::sqf::mission_namespace(), "intercept_test", "teeeest");
     } else {
         vec->turn();
         if (vec->timeAlive() > 4s) {
             delete vec;
             vec = nullptr;
         }
     }
     intercept::sqf::set_variable(intercept::sqf::mission_namespace(), "intercept_testxxxxx", "teeeest");





     intercept::sqf::rv_color color = std::vector<game_value>{ 1.0f, 0.0f, 0.0f, 1.0f };

     std::uniform_real_distribution<float> dist(0, 100);
     for (int i = 0; i < (int) dist(e2); i++)
         intercept::sqf::draw_icon_3d(
             "\\A3\\ui_f\\data\\map\\groupicons\\waypoint.paa",
             color,
             intercept::sqf::get_pos(sqf::player()) + vector3{ 0,0,5 } +randomOffset(2),
             1.0f,
             1.0f,
             1.0f,
             "text",
             2.0f,
             0.036f,
             "RobotoCondensed"
         );

         */
         //auto_array<int> testy{ 0,1,2,3,4,5,6 };
         //std::vector<int> testy2{ 20,21,22,23 };
         //testy.insert(testy.begin()+4, testy2.begin(), testy2.end());
         //testy.erase(std::find(testy.begin(),testy.end(),5));
         //testy.emplace(testy.begin() +5, 77);     
         //testy2.emplace(testy2.end(), 3);
         //std::vector<std::string> entries = intercept::sqf::config_classes("true", intercept::sqf::config_entry(intercept::sqf::config_file()) >> "CfgFunctions" >> "A3");
         //sqf::set_variable(sqf::mission_namespace(), "testVar", intercept::sqf::config_entry(intercept::sqf::config_file()) >> "CfgFunctions" >> "A3");
    //auto p = ((game_data_object*) sqf::player().data.getRef())->get_position_matrix();
    //auto p2 = ((game_data_object*) sqf::player().data.getRef())->get_head_pos();
    //sqf::system_chat(sqf::format({ "%1",p._position }));
    auto uavObject = sqf::get_connected_uav(sqf::player());
    if (uavObject.is_null()) return;
    auto uavPOS = sqf::uav_control(uavObject).position;
    if (uavPOS.compare("GUNNER") != 0) return;
    struct params {
        std::string _letter;
        intercept::types::vector3 _offset1;
    };
    auto DegToVec = [](float deg) {
        return vector3{ sin(deg),cos(deg),0.f };
    };
    float rad = 3.141592f / 180.f;
    float zoom = (1.f / getCameraZoom())*0.5f;
    params arr[] = {
        { "N",DegToVec(0)*zoom },
        { "NE",DegToVec(45 * rad)*zoom },
        { "E",DegToVec(90 * rad)*zoom },
        { "SE",DegToVec(135 * rad)*zoom },
        { "S",DegToVec(180 * rad)*zoom },
        { "SW",DegToVec(225 * rad)*zoom },
        { "W",DegToVec(270 * rad)*zoom },
        { "NW",DegToVec(315 * rad)*zoom },
    };

    for (auto& it : arr) {
        auto _center = intercept::sqf::position_camera_to_world({ 0,0,3 });
        intercept::sqf::draw_icon_3d(
            "",
            { 1.0f, 1.0f, 1.0f, 1.0f },
            _center + (it._offset1*0.7f),
            0, 0, 0,
            it._letter,
            2,
            0.05f,
            "PuristaMedium"
        );
        intercept::sqf::draw_icon_3d(
            "",
            { 1.0f, 1.0f, 1.0f, 1.0f },
            _center + (it._offset1*0.5f),
            0, 0, 0,
            ".",
            2,
            0.05f,
            "PuristaMedium"
        );
        intercept::sqf::draw_icon_3d(
            "",
            { 1.0f, 1.0f, 1.0f, 1.0f },
            _center + (it._offset1*0.25f),
            0, 0, 0,
            ".",
            2,
            0.05f,
            "PuristaMedium"
        );
        intercept::sqf::draw_icon_3d(
            "",
            { 1.0f, 1.0f, 1.0f, 1.0f },
            _center + (it._offset1*0.125f),
            0, 0, 0,
            ".",
            2,
            0.05f,
            "PuristaMedium"
        );
    }

    for (auto& unit : sqf::all_players()) {
        auto worldPos = sqf::get_pos(unit);
        auto pos = sqf::world_to_screen(worldPos);
        if (pos.x == 0.f || pos.y == 0.f) continue;
        auto group = sqf::get_group(unit);
        auto unitName = (std::string)sqf::name(unit);
        auto leader = sqf::leader(group);
        std::string texture = "";
        if (sqf::is_equal_to(leader, unit)) {
            unitName += " (" + (std::string)sqf::group_id(group) + ")";
        } else {
            intercept::sqf::draw_line_3d(worldPos + vector3{ 0.f,0.f,1.f }, sqf::get_pos(leader) + vector3{ 0.f,0.f,1.f }, { 1.f,0.f,1.f,1.f });
        }
        auto team = (std::string)sqf::assigned_team(unit);
        rv_color color = { 1.f,0.f,1.f,1.f };
        if (team.compare("RED") == 0)
            color = { 1.f,0.f,0.f,1.f };
        else if (team.compare("GREEN") == 0)
            color = { 0.f,1.f,0.f,1.f };
        else if (team.compare("BLUE") == 0)
            color = { 0.f,0.f,1.f,1.f };
        else if (team.compare("YELLOW") == 0)
            color = { 1.f,1.f,0.f,1.f };
        intercept::sqf::draw_icon_3d(
            texture,
            color,
            worldPos,
            0, 0, 0,
            unitName,
            2,
            0.03f,
            "PuristaLight"//"PuristaMedium"
        );
    }
    //use
    //https://community.bistudio.com/wiki/worldToScreen
    //To find users on screen and display markers on them.





    // get the player object and store it
    //intercept::types::object player = intercept::sqf::player();
   // intercept::sqf::set_dir(player, intercept::sqf::direction(player) + 5);
    // get the post of the player
    //intercept::types::vector3 pos = intercept::sqf::get_pos(player);

    // build a string...
   // std::stringstream side_chat_msg;
    //side_chat_msg << "Hello Arma World, here is the player pos: " << pos.x << "," << pos.y << "," << pos.z;

    // send it to the binary SQF sideChat command
   //Variable myVar("someGlobalVar");
   //intercept::sqf::side_chat(myVar.get(), side_chat_msg.str());
}

// Normal Windows DLL junk...
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_THREAD_ATTACH:	break;
        case DLL_THREAD_DETACH:	break;
        case DLL_PROCESS_DETACH:
            if (vec) delete vec;
            break;
    }
    return TRUE;
}

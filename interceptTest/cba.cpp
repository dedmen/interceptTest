#define NOMINMAX
#define INTERCEPT_SQF_STRTYPE_RSTRING

#include "cba.h"
#include <client/headers/intercept.hpp>
#include <sstream>
using namespace intercept;


#pragma region hashMap
types::registered_sqf_function _createHashMap;
types::registered_sqf_function _hashMapSet;
types::registered_sqf_function _hashMapFind;
types::registered_sqf_function _hashMapRemove;
types::registered_sqf_function _hashMapContains;
types::registered_sqf_function _hashMapCount;
types::registered_sqf_function _hashMapSelect;
types::registered_sqf_function _hashMapSetVar;
types::registered_sqf_function _hashMapGetVarDef;
types::registered_sqf_function _hashMapGetVarStr;
types::registered_sqf_function _hashMapGetKeyList;
static sqf_script_type GameDataHashMap_type;

class GameDataHashMap : public game_data {

public:
    GameDataHashMap() {}
    void lastRefDeleted() const override { delete this; }
    const sqf_script_type& type() const override { return GameDataHashMap_type; }
    ~GameDataHashMap() override {};

    bool get_as_bool() const override { return true; }
    float get_as_number() const override { return 0.f; }
    const r_string& get_as_string() const override { static r_string nm("hashmap"sv); return nm; }
    game_data* copy() const override { return new GameDataHashMap(*this); } //#TODO make sure this works
    r_string to_string() const override { return r_string("hashMap"sv); }
    //virtual bool equals(const game_data*) const override; //#TODO isEqualTo on hashMaps would be quite nice I guess?
    const char* type_as_string() const override { return "hashMap"; }
    bool is_nil() const override { return false; }
    bool can_serialize() override { return true; }//Setting this to false causes a fail in scheduled and global vars

    serialization_return serialize(param_archive& ar) override {
        game_data::serialize(ar);
        size_t entryCount;
        if (ar._isExporting) entryCount = map.size();
        //ar.serialize("entryCount"sv, entryCount, 1);
        //#TODO add array serialization functions
        //ar._p1->add_array_entry()
        //if (!ar._isExporting) {
        //
        //    for (int i = 0; i < entryCount; ++i) {
        //        s
        //    }
        //
        //
        //
        //}
        return serialization_return::no_error;
    }
    std::unordered_map<game_value, game_value> map;
};

game_data* createGameDataHashMap(param_archive* ar) {
    //#TODO use armaAlloc
    auto x = new GameDataHashMap();
    if (ar)
        x->serialize(*ar);
    return x;
}



game_value createHashMap() {
    return game_value(new GameDataHashMap());
}

//#define CBA_HASH_LOG

game_value hashSet(game_value_parameter hashMap, game_value_parameter args) {
    if (hashMap.is_nil() || args.size() != 2) return {}; //WTF U doin u idiot?! Stop givin me that crap
    auto map = static_cast<GameDataHashMap*>(hashMap.data.get());


    map->map[args[0]] = args[1];
#ifdef CBA_HASH_LOG
    std::stringstream str;
    str << "hashSetVar" << map << " " << static_cast<r_string>(args[0]) << "=" << static_cast<r_string>(args[1]) << "\n";
    OutputDebugStringA(str.str().c_str());
#endif
    return {};
}

game_value hashFind(game_value_parameter hashMap, game_value_parameter toFind) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.get());
    auto found = map->map.find(toFind);
    if (found != map->map.end()) {
    #ifdef CBA_HASH_LOG
        std::stringstream str;
        str << "hashGetVar" << map << " " << static_cast<r_string>(toFind) << "=" << static_cast<r_string>(found->second) << "\n";
        OutputDebugStringA(str.str().c_str());
    #endif
        return found->second;
    }
    return {};
}

game_value hashRemove(game_value_parameter hashMap, game_value_parameter toRemove) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.get());
    auto found = map->map.find(toRemove);
    if (found != map->map.end()) map->map.erase(found);
    return {};
}

game_value hashContains(game_value_parameter toFind, game_value_parameter hashMap) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.get());
    auto found = map->map.find(toFind);
    if (found != map->map.end()) return true;
    return false;
}

game_value hashCount(game_value_parameter hashMap) {
    if (hashMap.is_nil()) return 0;
    auto map = static_cast<GameDataHashMap*>(hashMap.data.get());
    return static_cast<float>(map->map.size());
}


game_value hashSetVar(game_value_parameter hashMap, game_value_parameter args) {
    if (hashMap.is_nil() || args.size() < 2) return {}; //WTF U doin?
    auto map = static_cast<GameDataHashMap*>(hashMap.data.get());


    map->map[args[0]] = args[1];
#ifdef CBA_HASH_LOG
    std::stringstream str;
    str << "hashSetVar" << map << " " << static_cast<r_string>(args[0]) << "=" << static_cast<r_string>(args[1]) << "\n";
    OutputDebugStringA(str.str().c_str());
#endif

    return {};
}

game_value hashGetVarDef(game_value_parameter hashMap, game_value_parameter args) {
    if (hashMap.is_nil() || args.size() < 2) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.get());
    auto found = map->map.find(args[0]);
    if (found != map->map.end()) {
    #ifdef CBA_HASH_LOG
        std::stringstream str;
        str << "hashGetVarDef" << map << " " << static_cast<r_string>(args[0]) << "=" << static_cast<r_string>(found->second) << "\n";
        OutputDebugStringA(str.str().c_str());
    #endif
        return found->second;
    }
    return args[1];
}

game_value hashGetVarStr(game_value_parameter hashMap, game_value_parameter args) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.get());
    auto found = map->map.find(args);
    if (found != map->map.end()) {
    #ifdef CBA_HASH_LOG
        std::stringstream str;
        str << "hashGetVarStr" << map << " " << static_cast<r_string>(args) << "=" << static_cast<r_string>(found->second) << "\n";
        OutputDebugStringA(str.str().c_str());
    #endif
        return found->second;
    }
    return {};
}

game_value hashGetKeyList(game_value_parameter hashMap) {
    if (hashMap.is_nil()) return 0;
    auto_array<game_value> keys;
    auto map = static_cast<GameDataHashMap*>(hashMap.data.get());
    for (auto& k : map->map) {
        keys.emplace_back(k.first);
    }

    return std::move(keys);
}


#pragma endregion hashMap


game_value getNumberWithDef(game_value_parameter right_arg) {
    if (right_arg.size() != 2) return {};
    if (sqf::is_number(right_arg[0]))
        return sqf::get_number(right_arg[0]);
    return right_arg[1];
}

game_value getTextWithDef(game_value_parameter right_arg) {
    if (right_arg.size() != 2) return {};
    if (sqf::is_number(right_arg[0]))
        return sqf::get_text(right_arg[0]);
    return right_arg[1];
}
game_value getArrayWithDef(game_value_parameter right_arg) {
    if (right_arg.size() != 2) return {};
    if (sqf::is_number(right_arg[0]))
        return sqf::get_array(right_arg[0]);
    return right_arg[1];
}
game_value getBoolWithDef(game_value_parameter right_arg) {
    if (right_arg.size() != 2) return {};
    if (sqf::is_number(right_arg[0]))
        return sqf::get_number(right_arg[0]) > 0.f;
    if (sqf::is_text(right_arg[0]))
        return r_string(sqf::get_text(right_arg[0])).compare_case_insensitive("true");

    return right_arg[1];
}

game_value getAnyWithDef(game_value_parameter right_arg) {
    if (right_arg.size() != 2) return {};
    if (sqf::is_number(right_arg[0]))
        return sqf::get_number(right_arg[0]);
    if (sqf::is_text(right_arg[0]))
        return sqf::get_text(right_arg[0]);
    if (sqf::is_array(right_arg[0]))
        return sqf::get_array(right_arg[0]);
    return right_arg[1];
}

game_value selectLast(game_value_parameter right_arg) {
    return right_arg.to_array().back();
}

game_value popEnd(game_value_parameter right_arg) {
    auto elem = right_arg.to_array().back();
    right_arg.to_array().erase(right_arg.to_array().end() - 1);
    return elem;
}

game_value popFront(game_value_parameter right_arg) {
    auto elem = right_arg.to_array().front();
    right_arg.to_array().erase(right_arg.to_array().begin());
    return elem;
}

game_value pushFront(game_value_parameter left_arg, game_value_parameter right_arg) {
    auto& arr = left_arg.to_array();
    arr.emplace(arr.begin(), right_arg);
    return {};
}

game_value pushFrontUnique(game_value_parameter left_arg, game_value_parameter right_arg) {
    auto& arr = left_arg.to_array();
    if (arr.find(right_arg) == arr.end()) {
        arr.emplace(arr.begin(), right_arg);
        return true;
    }
    return false;
}


game_value findCaseInsensitive(game_value_parameter left_arg, game_value_parameter right_arg) {
    bool searchIsString = right_arg.type() == game_data_string::type_def;
    auto& arr = left_arg.to_array();
    for (int it = 0; it < left_arg.size(); it++) {
        auto& element = arr[it];
        if (searchIsString && element.data && element.type() == game_data_string::type_def) {
            if (static_cast<r_string>(element).compare_case_insensitive(static_cast<r_string>(right_arg).c_str())) return it;
        } else {
            if (element == right_arg) return it;
        }
    }
    return -1;
}

game_value inArrayCaseInsensitive(game_value_parameter right_arg, game_value_parameter left_arg) {
    bool searchIsString = right_arg.type() == game_data_string::type_def;
    auto& arr = left_arg.to_array();
    for (int it = 0; it < left_arg.size(); it++) {
        auto& element = arr[it];
        if (searchIsString && element.data && element.type() == game_data_string::type_def) {
            if (static_cast<r_string>(element).compare_case_insensitive(static_cast<r_string>(right_arg).c_str())) return true;
        } else {
            if (element == right_arg) return true;
        }
    }
    return false;
}

game_value stringStartsWith(game_value_parameter left_arg, game_value_parameter right_arg) {
    auto leftStr = static_cast<sqf_string>(left_arg);
    auto rightStr = static_cast<sqf_string>(right_arg);
    if (rightStr.size() > leftStr.size()) return false;
    return (strncmp(leftStr.c_str(), rightStr.c_str(), std::min(leftStr.size(), rightStr.size())) == 0);
}

game_value stringStartsWithCI(game_value_parameter left_arg, game_value_parameter right_arg) {
    auto leftStr = static_cast<sqf_string>(left_arg);
    auto rightStr = static_cast<sqf_string>(right_arg);
    if (rightStr.size() > leftStr.size()) return false;
    return (_strnicmp(leftStr.c_str(), rightStr.c_str(), std::min(leftStr.size(), rightStr.size())) == 0);
}


game_value arrayUnion(game_value_parameter left_arg, game_value_parameter right_arg) {
    auto& leftArr = left_arg.to_array();
    auto& rightArr = right_arg.to_array();
    auto_array<game_value> output(leftArr);
    for (auto& elem : rightArr) {
        if (output.find(elem) != leftArr.end()) leftArr.emplace_back(elem);
    }
    return output;
}

game_value naturalLog(game_value_parameter right_arg) {
    return log(static_cast<float>(right_arg));
}

game_value nthRoot(game_value_parameter left_arg, game_value_parameter right_arg) {
    return std::pow(static_cast<float>(right_arg), 1.0f / static_cast<float>(left_arg));
}

game_value logn(game_value_parameter left_arg, game_value_parameter right_arg) {
    return std::log(static_cast<float>(right_arg)) / std::log(static_cast<float>(left_arg));
}

game_value regexReplace(game_value_parameter left_arg, game_value_parameter right_arg) {
    if (right_arg.size() != 2) return "";
    std::regex regr((std::string)right_arg[0]);
    return std::regex_replace((std::string)left_arg, regr, (std::string)right_arg[1]);
}

game_value nothing0() {
    return {};
}
game_value nothing1(game_value_parameter) {
    return {};
}
game_value nothing2(game_value_parameter, game_value_parameter) {
    return {};
}


class codeWithCexp : public game_data {
public:
    r_string code_string;
    struct {
        uint32_t ref;
        uint32_t count;

    } *instrarr;
    bool is_final;
};


game_value instructionCount(game_value_parameter code) {
    if (code.is_nil()) return 0;
    auto c = (codeWithCexp*) code.data.get();
    if (!c->instrarr) return 0;
    return (float) c->instrarr->count;

    return {};
}


game_value getObjectConfigFromObj(game_value_parameter obj) {
    auto type = sqf::type_of(obj);

    for (std::string_view cls : { "CfgVehicles"sv, "CfgAmmo"sv, "CfgNonAIVehicles"sv }) {
        auto cfgClass = sqf::config_entry() >> cls >> type;
        if (sqf::is_class(cfgClass)) return static_cast<game_value>(cfgClass);

    }
    return static_cast<game_value>(sqf::config_null());
}

game_value getObjectConfigFromStr(game_value_parameter className) {
    sqf_string type = className;
    for (auto& cls : { "CfgVehicles"sv, "CfgAmmo"sv, "CfgNonAIVehicles"sv }) {
        auto cfgClass = sqf::config_entry() >> cls >> type;
        if (sqf::is_class(cfgClass)) return static_cast<game_value>(cfgClass);

    }
    return static_cast<game_value>(sqf::config_null());
}

game_value getItemConfigFromObj(game_value_parameter obj) {
    auto type = sqf::type_of(obj);

    for (auto& cls : { "CfgWeapons"sv, "CfgMagazines"sv, "CfgGlasses"sv }) {
        auto cfgClass = sqf::config_entry() >> cls >> type;
        if (sqf::is_class(cfgClass)) return static_cast<game_value>(cfgClass);

    }
    return static_cast<game_value>(sqf::config_null());
}

game_value getItemConfigFromStr(game_value_parameter className) {
    sqf_string type = className;
    for (auto& cls : { "CfgWeapons"sv, "CfgMagazines"sv, "CfgGlasses"sv }) {
        auto cfgClass = sqf::config_entry() >> cls >> type;
        if (sqf::is_class(cfgClass)) return static_cast<game_value>(cfgClass);

    }
    return static_cast<game_value>(sqf::config_null());
}


//https://github.com/CBATeam/CBA_A3/blob/master/addons/common/fnc_turretPath.sqf
game_value turretPath(game_value_parameter unit) {
    auto vehicle = sqf::vehicle(unit);

    for (auto& turret : sqf::all_turrets(vehicle, true)) {
        if (sqf::turret_unit(vehicle, turret) == unit) return turret;
    }
    return { auto_array<game_value>() }; //empty array
}


game_value aliveGroup(game_value_parameter grp) {
    for (auto& unit : sqf::units(static_cast<group>(grp)))
        if (sqf::alive(unit)) return true;
    return false;
}

game_value unarySpawn(game_value_parameter code) {
    return static_cast<game_value>(sqf::spawn({}, code));
}

game_value hasItem(game_value_parameter obj, game_value_parameter classn) {
    r_string classname = classn;
    auto containsString = [&classname](const sqf_return_string_list& list) {
        for (auto& item : list)
            if (item == classname) return true;
        return false;
    };

    if (containsString(sqf::get_item_cargo(sqf::uniform_container(obj)).types)) return true;
    if (containsString(sqf::get_item_cargo(sqf::vest_container(obj)).types)) return true;
    if (containsString(sqf::assigned_items(obj))) return true;

    if (sqf::goggles(obj) == classname) return true;
    if (sqf::headgear(obj) == classname) return true;
    if (sqf::hmd(obj) == classname) return true;

    if (containsString(sqf::get_item_cargo(sqf::backpack_container(obj)).types)) return true;

    return false;
}

game_value getObjPosRaw(game_value_parameter obj) {
    return sqf::model_to_world_visual_world(obj, { 0,0,0 });
}

game_value CBA_oldUnit;
game_value CBA_oldGroup;
game_value CBA_oldLeader;
r_string CBA_oldWeapon;
game_value CBA_oldLoadout;
sqf::rv_unit_loadout CBA_oldLoadoutNoAmmo;
game_value CBA_oldVehicle;
game_value CBA_oldTurret;
int CBA_oldVisionMode;
r_string CBA_oldCameraView;
bool CBA_oldVisibleMap;

game_value CBA_playerEH_EachFrame() {
    static game_value_static CBA_fnc_currentUnit = sqf::get_variable(sqf::mission_namespace(), "CBA_fnc_currentUnit");
    static game_value_static CBA_fnc_localEvent = sqf::get_variable(sqf::mission_namespace(), "CBA_fnc_localEvent");

    object _player = sqf::get_variable(sqf::mission_namespace(), "bis_fnc_moduleRemoteControl_unit", sqf::player());
    if (_player != CBA_oldUnit) {
        sqf::call(CBA_fnc_localEvent, { "cba_common_unitEvent", { _player , CBA_oldUnit } });
        CBA_oldUnit = _player;
    }

    game_value _data = sqf::get_group(_player);
    if (_data != CBA_oldGroup) {
        sqf::call(CBA_fnc_localEvent, { "cba_common_groupEvent",{ _data , CBA_oldGroup } });
        CBA_oldGroup = _data;
    }

    _data = sqf::leader(_player);
    if (_data != CBA_oldLeader) {
        sqf::call(CBA_fnc_localEvent, { "cba_common_leaderEvent",{ _data , CBA_oldLeader } });
        CBA_oldLeader = _data;
    }

    _data = sqf::current_weapon(_player);
    if (_data != CBA_oldWeapon) {
        CBA_oldWeapon = _data;
        sqf::call(CBA_fnc_localEvent, { "cba_common_weaponEvent",{ _player , _data } });
    }

    _data = sqf::get_unit_loadout(_player);
    if (_data != CBA_oldLoadout) {
        CBA_oldLoadout = _data;
        auto loadout = sqf::rv_unit_loadout(_data);
        // we don't want to trigger this just because your ammo counter decreased.

        //remove ammo
        loadout.primary.primary_muzzle_magazine.ammo = -1;
        loadout.primary.primary_muzzle_magazine.count = -1;
        loadout.primary.secondary_muzzle_magazine.ammo = -1;
        loadout.primary.secondary_muzzle_magazine.count = -1;
        loadout.secondary.primary_muzzle_magazine.ammo = -1;
        loadout.secondary.primary_muzzle_magazine.count = -1;
        loadout.secondary.secondary_muzzle_magazine.ammo = -1;
        loadout.secondary.secondary_muzzle_magazine.count = -1;
        loadout.handgun.primary_muzzle_magazine.ammo = -1;
        loadout.handgun.primary_muzzle_magazine.count = -1;
        loadout.handgun.secondary_muzzle_magazine.ammo = -1;
        loadout.handgun.secondary_muzzle_magazine.count = -1;

        if (!(loadout == CBA_oldLoadoutNoAmmo)) {
            CBA_oldLoadoutNoAmmo = std::move(loadout);
            sqf::call(CBA_fnc_localEvent, { "cba_common_loadoutEvent",{ _player , _data } });
        }
    }

    _data = sqf::vehicle(_player);
    if (_data != CBA_oldVehicle) {
        CBA_oldVehicle = _data;
        sqf::call(CBA_fnc_localEvent, { "cba_common_vehicleEvent",{ _player , _data } });
    }

    _data = turretPath(_player);
    if (_data != CBA_oldTurret) {
        CBA_oldTurret = _data;
        sqf::call(CBA_fnc_localEvent, { "cba_common_turretEvent",{ _player , _data } });
    }

    auto visionMode = sqf::current_vision_mode(_player);
    if (visionMode != CBA_oldVisionMode) {
        CBA_oldVisionMode = visionMode;
        sqf::call(CBA_fnc_localEvent, { "cba_common_visionModeEvent",{ _player , visionMode } });
    }

    auto camView = sqf::camera_view();
    if (camView != CBA_oldCameraView) {
        CBA_oldCameraView = camView;
        sqf::call(CBA_fnc_localEvent, { "cba_common_cameraViewEvent",{ _player , camView } });
    }
    return {};
}

game_value compareBoolNumber(game_value_parameter left, game_value_parameter right) {
    return static_cast<bool>(left) == (static_cast<float>(right) != 0.f);
}
game_value compareNumberBool(game_value_parameter left, game_value_parameter right) {
    return static_cast<bool>(right) == (static_cast<float>(left) != 0.f);
}

class GameInstructionSetLVar : public game_instruction {
public:
    r_string vName;
    game_value val;
    bool exec(types::__internal::game_state* state, void* t) override {
        auto sv = client::host::functions.get_engine_allocator()->setvar_func;
        sv(vName.c_str(), val);
        return false;
    }
    int stack_size(void* t) const override { return 0; };
    r_string get_name() const override { return "GameInstructionSetLVar"sv; };
    ~GameInstructionSetLVar() override {};

};
//#define INTEL_NO_ITTNOTIFY_API
#include <ittnotify.h>

__itt_string_handle* handle_c_task1 = __itt_string_handle_create("catenarybisec");
__itt_string_handle* handle_c_task2 = __itt_string_handle_create("catenarynewton");
__itt_string_handle* handle_c_task3 = __itt_string_handle_create("estimateSegmentCount");
__itt_string_handle* handle_c_task4 = __itt_string_handle_create("catenaryCalc");
__itt_string_handle* handle_c_task5 = __itt_string_handle_create("catenarytoWorld");
__itt_string_handle* handle_c_task6 = __itt_string_handle_create("toGV");
__itt_string_handle* handle_stringtest = __itt_string_handle_create("stringtest");
extern __itt_domain* domain;



void cba::preStart() {

    auto codeType = client::host::registerType("HASHMAP"sv, "hashMap"sv, "Dis is a hashmap. It hashes things."sv, "hashMap"sv, createGameDataHashMap);
    GameDataHashMap_type = codeType.second;

    _createHashMap = client::host::registerFunction("createHashMap", "Creates a Hashmap", userFunctionWrapper<createHashMap>, codeType.first);
    _hashMapSet = client::host::registerFunction("set", "Sets a value in a Hashmap", userFunctionWrapper<hashSet>, GameDataType::NOTHING, codeType.first, GameDataType::ARRAY);
    _hashMapFind = client::host::registerFunction("find", "Finds an element in a Hashmap", userFunctionWrapper<hashFind>, GameDataType::ANY, codeType.first, GameDataType::ANY);
    _hashMapSelect = client::host::registerFunction("select", "Finds an element in a Hashmap", userFunctionWrapper<hashFind>, GameDataType::ANY, codeType.first, GameDataType::ANY);
    _hashMapRemove = client::host::registerFunction("deleteAt", "Deletes an element in a Hashmap", userFunctionWrapper<hashRemove>, GameDataType::NOTHING, codeType.first, GameDataType::ANY);
    _hashMapContains = client::host::registerFunction("in", "Checks if given element is inside Hashmap", userFunctionWrapper<hashContains>, GameDataType::BOOL, GameDataType::ANY, codeType.first);
    _hashMapCount = client::host::registerFunction("count", "Counts number of elements inside Hashmap", userFunctionWrapper<hashCount>, GameDataType::SCALAR, codeType.first);
    _hashMapSetVar = client::host::registerFunction("setVariable", "", userFunctionWrapper<hashSetVar>, GameDataType::NOTHING, codeType.first, GameDataType::ARRAY);
    _hashMapGetVarDef = client::host::registerFunction("getVariable", "", userFunctionWrapper<hashGetVarDef>, GameDataType::ANY, codeType.first, GameDataType::ARRAY);
    _hashMapGetVarStr = client::host::registerFunction("getVariable", "", userFunctionWrapper<hashGetVarStr>, GameDataType::ANY, codeType.first, GameDataType::STRING);
    _hashMapGetKeyList = client::host::registerFunction("allVariables", "", userFunctionWrapper<hashGetKeyList>, GameDataType::ARRAY, codeType.first);

    static auto _getNumberWithDef = intercept::client::host::registerFunction("getNumber", "", userFunctionWrapper<getNumberWithDef>, GameDataType::SCALAR, GameDataType::ARRAY);
    static auto _getTextWithDef = intercept::client::host::registerFunction("getText", "", userFunctionWrapper<getTextWithDef>, GameDataType::STRING, GameDataType::ARRAY);
    static auto _getArrayWithDef = intercept::client::host::registerFunction("getArray", "", userFunctionWrapper<getArrayWithDef>, GameDataType::ARRAY, GameDataType::ARRAY);
    static auto _getBoolWithDef = intercept::client::host::registerFunction("getBool", "", userFunctionWrapper<getBoolWithDef>, GameDataType::BOOL, GameDataType::ARRAY);
    static auto _getAnyWithDef = intercept::client::host::registerFunction("getAny", "", userFunctionWrapper<getAnyWithDef>, GameDataType::ANY, GameDataType::ARRAY);
    static auto _selectLast = intercept::client::host::registerFunction("selectLast", "", userFunctionWrapper<selectLast>, GameDataType::ANY, GameDataType::ARRAY);
    static auto _popEnd = intercept::client::host::registerFunction("popEnd", "", userFunctionWrapper<popEnd>, GameDataType::ANY, GameDataType::ARRAY);
    static auto _popFront = intercept::client::host::registerFunction("popFront", "", userFunctionWrapper<popFront>, GameDataType::ANY, GameDataType::ARRAY);
    static auto _pushFront = intercept::client::host::registerFunction("pushFront", "", userFunctionWrapper<pushFront>, GameDataType::NOTHING, GameDataType::ARRAY, GameDataType::ANY);
    static auto _pushFrontUnique = intercept::client::host::registerFunction("pushFrontUnique", "", userFunctionWrapper<pushFrontUnique>, GameDataType::BOOL, GameDataType::ARRAY, GameDataType::ANY);
    static auto _findCI = intercept::client::host::registerFunction("findCI", "", userFunctionWrapper<findCaseInsensitive>, GameDataType::ANY, GameDataType::ARRAY, GameDataType::ANY);
    static auto _inArrayCI = intercept::client::host::registerFunction("inCI", "", userFunctionWrapper<inArrayCaseInsensitive>, GameDataType::ANY, GameDataType::ANY, GameDataType::ARRAY);
    static auto _startsWith = intercept::client::host::registerFunction("startsWith", "", userFunctionWrapper<stringStartsWith>, GameDataType::BOOL, GameDataType::STRING, GameDataType::STRING);
    static auto _startsWithCI = intercept::client::host::registerFunction("startsWithCI", "", userFunctionWrapper<stringStartsWith>, GameDataType::BOOL, GameDataType::STRING, GameDataType::STRING);
    static auto _arrayUnion = intercept::client::host::registerFunction("arrayUnion", "", userFunctionWrapper<arrayUnion>, GameDataType::ARRAY, GameDataType::ARRAY, GameDataType::ARRAY);
    static auto _naturalLog = intercept::client::host::registerFunction("ln", "", userFunctionWrapper<naturalLog>, GameDataType::SCALAR, GameDataType::SCALAR);
    static auto _nthRoot = intercept::client::host::registerFunction("root", "", userFunctionWrapper<nthRoot>, GameDataType::SCALAR, GameDataType::SCALAR, GameDataType::SCALAR);
    static auto _logn = intercept::client::host::registerFunction("log", "", userFunctionWrapper<logn>, GameDataType::SCALAR, GameDataType::SCALAR, GameDataType::SCALAR);
    static auto _regexReplace = intercept::client::host::registerFunction("regexReplace", "", userFunctionWrapper<regexReplace>, GameDataType::STRING, GameDataType::STRING, GameDataType::ARRAY);
    static auto _nothing0 = intercept::client::host::registerFunction("nothingN", "", userFunctionWrapper<nothing0>, GameDataType::NOTHING);
    static auto _nothing1 = intercept::client::host::registerFunction("nothing", "", userFunctionWrapper<nothing1>, GameDataType::NOTHING, GameDataType::ANY);
    static auto _nothing2 = intercept::client::host::registerFunction("nothing", "", userFunctionWrapper<nothing2>, GameDataType::NOTHING, GameDataType::ANY, GameDataType::ANY);
    static auto _instructionCount = intercept::client::host::registerFunction("instructionCount", "", userFunctionWrapper<instructionCount>, GameDataType::SCALAR, GameDataType::CODE);
    static auto _getObjectConfigFromObj = intercept::client::host::registerFunction("getObjectConfig", "", userFunctionWrapper<getObjectConfigFromObj>, GameDataType::CONFIG, GameDataType::OBJECT);
    static auto _getObjectConfigFromStr = intercept::client::host::registerFunction("getObjectConfig", "", userFunctionWrapper<getObjectConfigFromStr>, GameDataType::CONFIG, GameDataType::STRING);
    static auto _getItemConfigFromObj = intercept::client::host::registerFunction("getItemConfig", "", userFunctionWrapper<getObjectConfigFromObj>, GameDataType::CONFIG, GameDataType::OBJECT);
    static auto _getItemConfigFromStr = intercept::client::host::registerFunction("getItemConfig", "", userFunctionWrapper<getObjectConfigFromStr>, GameDataType::CONFIG, GameDataType::STRING);
    static auto _alive = intercept::client::host::registerFunction("alive", "", userFunctionWrapper<aliveGroup>, GameDataType::BOOL, GameDataType::GROUP);
    static auto _unarySpawn = intercept::client::host::registerFunction("spawn", "", userFunctionWrapper<unarySpawn>, GameDataType::SCRIPT, GameDataType::CODE);
    static auto _turretPath = intercept::client::host::registerFunction("turretPath", "", userFunctionWrapper<turretPath>, GameDataType::ARRAY, GameDataType::OBJECT);
    static auto _hasItem = intercept::client::host::registerFunction("hasItem", "", userFunctionWrapper<hasItem>, GameDataType::BOOL, GameDataType::OBJECT, GameDataType::STRING);
    static auto _mwmwww = intercept::client::host::registerFunction("modelToWorldWorldWorldVisualWorldWorldVisualWorldVisualASL", "", userFunctionWrapper<getObjPosRaw>, GameDataType::ARRAY, GameDataType::OBJECT);
    static auto _playerEH = intercept::client::host::registerFunction("CBA_Intercept_playerEH", "", userFunctionWrapper<CBA_playerEH_EachFrame>, GameDataType::NOTHING);
    static auto _cmpBoolNumber = intercept::client::host::registerFunction("==", "", userFunctionWrapper<compareBoolNumber>, GameDataType::BOOL, GameDataType::BOOL, GameDataType::SCALAR);
    static auto _cmpNumberBool = intercept::client::host::registerFunction("==", "", userFunctionWrapper<compareNumberBool>, GameDataType::BOOL, GameDataType::SCALAR, GameDataType::BOOL);
    static auto _andNumberBool = intercept::client::host::registerFunction("&&", "", [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
        return static_cast<bool>(right) && static_cast<float>(left) != 0.f;
    }, GameDataType::BOOL, GameDataType::SCALAR, GameDataType::BOOL);
    static auto _andBoolNumber = intercept::client::host::registerFunction("&&", "", [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
        return static_cast<bool>(left) && static_cast<float>(right) != 0.f;
    }, GameDataType::BOOL, GameDataType::BOOL, GameDataType::SCALAR);
    static auto _andNumberNumber = intercept::client::host::registerFunction("&&", "", [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
        return static_cast<float>(left) != 0.f && static_cast<float>(right) != 0.f;
    }, GameDataType::BOOL, GameDataType::SCALAR, GameDataType::SCALAR);
    static auto _isEqTo = intercept::client::host::registerFunction(u8"iForEach"sv, "", [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
        auto& arr = left.to_array();
        for (auto& element : arr)
            sqf::call(right, element);
        return {};
    }, GameDataType::NOTHING, GameDataType::ARRAY, GameDataType::CODE);
    static auto _isEqTo2 = intercept::client::host::registerFunction(u8"iForEach2"sv, "", [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
        auto& arr = left.to_array();

        //static game_value_static _xassignment = sqf::compile("private _x = 123; _x");
        //auto baseCode = static_cast<game_data_code*>(_xassignment.data.get());
        auto bodyCode = static_cast<game_data_code*>(right.data.get());

        //auto curElInstruction = static_cast<game_instruction_constant*>(baseCode->instructions->get(1).get());
        ref<GameInstructionSetLVar> curElInstruction = rv_allocator<GameInstructionSetLVar>::create_single();
        curElInstruction->vName = "_x";
        auto oldInstructions = bodyCode->instructions;
        ref<compact_array<ref<game_instruction>>> newInstr = compact_array<ref<game_instruction>>::create(*oldInstructions, oldInstructions->size() + 1);

        //std::_Move_no_deprecate(newInstr->data() + 1, newInstr->data() + oldInstructions->size(), newInstr->data() + 3);

        std::_Copy_no_deprecate(oldInstructions->data() + 1, oldInstructions->data() + oldInstructions->size(), newInstr->data() + 2);
        //newInstr->data()[0] = baseCode->instructions->get(0);


        newInstr->data()[1] = curElInstruction;

        //newInstr->data()[1] = baseCode->instructions->get(1);

        bodyCode->instructions = newInstr;

        for (auto& element : arr) {
            curElInstruction->val.data = element.data;
            sqf::call(right);
        }

        bodyCode->instructions = oldInstructions;

        return {};
    }, GameDataType::NOTHING, GameDataType::ARRAY, GameDataType::CODE);
    static auto _isEqTo3 = intercept::client::host::registerFunction(u8"iForEach3"sv, "", [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
        auto& arr = left.to_array();
        for (auto& element : arr)
            sqf::call(right);
        return {};
    }, GameDataType::NOTHING, GameDataType::ARRAY, GameDataType::CODE);
    static auto _currentUnit = intercept::client::host::registerFunction("currentUnit"sv, "Returns the current Unit (CBA_fnc_currentUnit)"sv, [](uintptr_t) -> game_value {
        auto obj = sqf::get_variable(sqf::mission_namespace(), "");
        if (obj.is_null()) return sqf::player();
        return obj;
    }, GameDataType::OBJECT);


    static auto _sl = intercept::client::host::registerFunction("startLoadingScreen"sv, "Fuck the establishment!"sv, [](uintptr_t, game_value_parameter right) -> game_value {
        sqf::diag_log({ "startloadingscreen", right });
        if (right.size() > 1) {
            //sqf::start_loading_screen(right[0], right[1]);
        } else {
            //sqf::start_loading_screen(right[0]);
        }
        return {};
    }, GameDataType::NOTHING, GameDataType::ARRAY);
    static auto _rl = intercept::client::host::registerFunction("endLoadingScreen"sv, "REALLYBRO!"sv, [](uintptr_t) -> game_value {
        sqf::diag_log("endloadingscreen");
        //sqf::end_loading_screen();
        return  {};
    }, GameDataType::NOTHING);









    static auto _sinh = intercept::client::host::registerFunction("sinh"sv, ""sv, [](uintptr_t, game_value_parameter right) -> game_value {
        return std::sinh(right);
    }, GameDataType::SCALAR, GameDataType::SCALAR);

    static auto _cosh = intercept::client::host::registerFunction("cosh"sv, ""sv, [](uintptr_t, game_value_parameter right) -> game_value {
        return std::cosh(right);
    }, GameDataType::SCALAR, GameDataType::SCALAR);

    static auto _tanh = intercept::client::host::registerFunction("tanh"sv, ""sv, [](uintptr_t, game_value_parameter right) -> game_value {
        return std::tanh(right);
    }, GameDataType::SCALAR, GameDataType::SCALAR);

    static auto _asinh = intercept::client::host::registerFunction("asinh"sv, ""sv, [](uintptr_t, game_value_parameter right) -> game_value {
        return std::asinh(right);
    }, GameDataType::SCALAR, GameDataType::SCALAR);

    static auto _acosh = intercept::client::host::registerFunction("acosh"sv, ""sv, [](uintptr_t, game_value_parameter right) -> game_value {
        return std::acosh(right);
    }, GameDataType::SCALAR, GameDataType::SCALAR);

    static auto _atanh = intercept::client::host::registerFunction("atanh"sv, ""sv, [](uintptr_t, game_value_parameter right) -> game_value {
        return std::atanh(right);
    }, GameDataType::SCALAR, GameDataType::SCALAR);



    static auto _catenaryConnect = intercept::client::host::registerFunction("catenaryConnect"sv, ""sv, [](uintptr_t, game_value_parameter right) -> game_value {
        if (right.size() < 3) {
            /* too few arguments */
            return  { {} };
        }
        #define MAX_ITERATIONS_BISECTION 20
        #define MAX_ITERATIONS_NEWTON 10
        #define MAX_DIFF_BISECTION 0.01
        #define MAX_DIFF_NEWTON 0.001
        #define MAX_SEGMENTS 500
        #define MAX_SEGMENT_DIFF 0.0001
        //pos1,pos2,rope_length_factor,segment_length

        vector3 pos1 = right[0];
        vector3 pos2 = right[1];
        float segment_length = 1.0;
        float rope_length_factor = right[2];
        if (right.size() >= 4)
            segment_length = right[3];

        /* calculate the 2d catenary curve */
        float delta_x = sqrt(pow(pos2.x - pos1.y, 2) + pow(pos2.y - pos1.y, 2));
        float delta_y = pos2.z - pos1.z;
        float rope_length = rope_length_factor * sqrt(delta_x*delta_x + delta_y*delta_y);

        if (delta_x == 0) {
            return  { {} };
        };

        float a = 1.0;
        float aMin = 0.1;
        float aMax = 10 * rope_length;
        float diff = 1.0e+7;
        float alpha = sqrt(pow(rope_length, 2) - pow(delta_y, 2));

        __itt_task_begin(domain, __itt_null, __itt_null, handle_c_task1);

        for (int i = 1; i <= MAX_ITERATIONS_BISECTION && abs(diff) > MAX_DIFF_BISECTION; i = i + 1) {
            a = (aMin + aMax) / 2;
            diff = 2 * a * sinh(delta_x / (2 * a)) - alpha;

            if (diff < 0)
                aMax = a;
            else
                aMin = a;
        }
        __itt_task_end(domain);
        float prev = 1.0e+7;
        __itt_task_begin(domain, __itt_null, __itt_null, handle_c_task2);
        for (int i = 1; i <= MAX_ITERATIONS_NEWTON && abs(prev - a) > MAX_DIFF_NEWTON; i = i + 1) {
            a = a -
                (2 * a * sinh(delta_x / (2 * a)) - alpha) /
                (2 * sinh(delta_x / (2 * a)) - delta_x / a * cosh(delta_x / (2 * a)));

            prev = a;
        };
        __itt_task_end(domain);
        if (!isfinite(a)) {
            /* rope too short */
            return  { {} };
        };

        float x1 = a * atanh(delta_y / rope_length) - delta_x / 2;
        float x2 = a * atanh(delta_y / rope_length) + delta_x / 2;
        float y1 = a * cosh(x1 / a);
        float y2 = a * cosh(x2 / a);

        /* estimate amount of needed segments and increase segment length if necessary */
        __itt_task_begin(domain, __itt_null, __itt_null, handle_c_task3);
        if (rope_length / segment_length > MAX_SEGMENTS) {
            segment_length = rope_length / MAX_SEGMENTS;

            if (segment_length > 1)
                segment_length = ceil(segment_length) + 1;
        };
        __itt_task_end(domain);
        /* generate a vector of x,y points on catenary with distance of the segment length */
        vector2 last_pos = { x1, y1 };
        vector2  end_pos = { x2, y2 };
        std::vector<vector2> catenary_points;
        catenary_points.reserve(std::min(rope_length / segment_length, 500.f));
        __itt_task_begin(domain, __itt_null, __itt_null, handle_c_task4);
        while (sqrt(pow(end_pos.x - last_pos.x, 2) + pow(end_pos.y - last_pos.y, 2)) > segment_length) {
            float u = last_pos.x;
            float y = last_pos.y;

            float uMin = u;
            float uMax = u + segment_length;

            /* find the next "u" */
            while (abs(uMin - uMax) > MAX_SEGMENT_DIFF * segment_length) {
                y = a * cosh(u / a);

                if (sqrt(pow(last_pos.x - u, 2) + pow(last_pos.y - y, 2)) > segment_length)
                    uMax = u;
                else
                    uMin = u;

                u = (uMin + uMax) / 2;
            };

            y = a * cosh(u / a);

            last_pos = { u, y };

            catenary_points.push_back({ u,y });
        };
        __itt_task_end(domain);
        catenary_points.push_back(end_pos);

        /* convert 2d points on catenary to world positions */
        vector2 dir_map{ pos2.x - pos1.x, pos2.y - pos1.y };
        float dir_map_length = dir_map.magnitude();
        dir_map /= dir_map_length;


        std::string result;


        __itt_task_begin(domain, __itt_null, __itt_null, handle_stringtest);
        for (auto& p : catenary_points) {
            std::string x = std::to_string(pos1.x + (p.x - x1) * dir_map.x);
            std::string y = std::to_string(pos1.y + (p.x - x1) * dir_map.y);
            std::string z = std::to_string(pos1.z + p.y - y1);
            result = result + "[" + x + "," + y + +"," + z + "],";
        };

        __itt_task_end(domain);
        


        auto_array<vector3> ret;
        ret.reserve(catenary_points.size());
        __itt_task_begin(domain, __itt_null, __itt_null, handle_c_task5);
        for (auto& p : catenary_points) {
            ret.emplace_back(
                pos1.x + (p.x - x1) * dir_map.x,
                pos1.y + (p.x - x1) * dir_map.y,
                pos1.z + p.y - y1
            );
        };
        __itt_task_end(domain);
        __itt_task_begin(domain, __itt_null, __itt_null, handle_c_task6);
        game_value output(std::move(ret));
        __itt_task_end(domain);
        return std::move(output);
    }, GameDataType::ARRAY, GameDataType::ARRAY);


}


struct pfh {
    game_value func;
    float delay;
    float delta;
    float timeAdded;
    game_value args;
    int handle;
    bool del{ false };

    pfh(game_value _1, float _2, float _3, float _4, game_value _5, float _6) {
        func = _1;
        delay = _2;
        delta = _3;
        timeAdded = _4;
        args = _5;
        handle = _6;
    }


};
std::vector<std::shared_ptr<pfh>> perFrameHandlerArray;
float PFHHandle = 0.f;

struct waitAndExecHandler {
    float time;
    game_value func;
    game_value args;
    waitAndExecHandler(float _1, game_value _2, game_value _3) {
        time = _1;
        func = _2;
        args = _3;
    }
    bool operator< (const waitAndExecHandler& other) {
        return time < other.time;
    }
};
std::vector<std::shared_ptr<waitAndExecHandler>> waitAndExecArray;
bool waitAndExecArrayIsSorted;

//arg,func
std::vector<std::pair<game_value, game_value>> nextFrameBufferA;
std::vector<std::pair<game_value, game_value>> nextFrameBufferB;
int nextFrameNo;

struct waitUntilAndExecHandler {
    game_value args;
    game_value cond;
    game_value func;
    waitUntilAndExecHandler(game_value _1, game_value _2, game_value _3) {
        args = _1;
        cond = _2;
        func = _3;
    }
    waitUntilAndExecHandler(game_value _1, game_value _2, game_value _3, float _4, game_value _5) {
        args = _1;
        cond = _2;
        func = _3;
        timeout = _4;
        timeOutCode = _5;
    }
    float timeout{ 0.f };
    game_value timeOutCode;
    bool done{ false };
};
std::vector<std::shared_ptr<waitUntilAndExecHandler>> waitUntilAndExecArray;

void swapFuncs(game_value orig, game_value newCode) {
    auto c = static_cast<game_data_code*>(orig.data.get());

    auto nc = static_cast<game_data_code*>(newCode.data.get());

    auto _1 = c->code_string;
    auto _2 = c->instructions;
    auto _3 = c->_dummy1;
    auto _4 = c->_dummy;
    auto _5 = c->is_final;


    c->code_string = nc->code_string;
    c->instructions = nc->instructions;
    c->_dummy1 = nc->_dummy1;
    c->_dummy = nc->_dummy;
    c->is_final = nc->is_final;

    nc->code_string = _1;
    nc->instructions = _2;
    nc->_dummy1 = _3;
    nc->_dummy = _4;
    nc->is_final = _5;
}

void cba::preInit() {
    CBA_oldUnit = sqf::obj_null();
    CBA_oldGroup = sqf::grp_null();
    CBA_oldLeader = sqf::obj_null();
    CBA_oldWeapon = ""sv;
    CBA_oldLoadout = game_value{};
    CBA_oldLoadoutNoAmmo = game_value{};
    CBA_oldVehicle = sqf::obj_null();
    CBA_oldTurret = game_value{};
    CBA_oldVisionMode = -1;
    CBA_oldCameraView = ""sv;
    CBA_oldVisibleMap = false;
    sqf::set_variable(sqf::mission_namespace(), "CBA_missionTime", 0);


    static auto onFrame = client::generate_custom_callback([](game_value_parameter arg) -> game_value {
        auto sv = client::host::functions.get_engine_allocator()->setvar_func;
        sv("_tickTime", sqf::diag_ticktime());
        auto tickTime = sqf::diag_ticktime();     //chrono seconds


        sqf::call(sqf::get_variable(sqf::mission_namespace(), "CBA_common_fnc_missionTimePFH"));

        //call FUNC(missionTimePFH);




        // Execute per frame handlers

        perFrameHandlerArray.erase(
            std::remove_if(perFrameHandlerArray.begin(), perFrameHandlerArray.end(), [](const std::shared_ptr<pfh>& it) {
            return it->del;
        }), perFrameHandlerArray.end());

        auto pfhcopy = perFrameHandlerArray;
        for (auto& handler : pfhcopy) {
            if (tickTime > handler->delta) {
                handler->delta += handler->delay;
                sv("_args", handler->args);
                sv("_handle", handler->handle);

                sqf::call(handler->func, { handler->args, handler->handle });
            }
        }

        // Execute wait and execute functions
        // Sort the queue if necessary
        if (!waitAndExecArrayIsSorted) {
            std::sort(waitAndExecArray.begin(), waitAndExecArray.end());
            waitAndExecArrayIsSorted = true;
        };

        float CBA_missionTime = sqf::get_variable(sqf::mission_namespace(), "CBA_missionTime");
        bool _delete = false;
        auto waxcpy = waitAndExecArray;
        for (auto& handler : waxcpy) {
            if (handler->time > CBA_missionTime) break;
            if (handler->args.is_nil())
                sqf::call(handler->func);
            else
                sqf::call(handler->func, handler->args);
            handler->time = 0.f;
            _delete = true;
        }

        if (_delete) {
            waitAndExecArray.erase(
                std::remove_if(waitAndExecArray.begin(), waitAndExecArray.end(), [](const std::shared_ptr<waitAndExecHandler>& hand) {
                return hand->time == 0.f;
            }), waitAndExecArray.end());
        }

        // Execute the exec next frame functions

        for (auto& handler : nextFrameBufferA) {
            sqf::call(handler.second, handler.first);
        }

        // Swap double-buffer:
        std::swap(nextFrameBufferA, nextFrameBufferB);
        nextFrameBufferB.clear();
        nextFrameNo = sqf::diag_frameno() + 1;


        // Execute the waitUntilAndExec functions:


        _delete = false;
        auto wuaxcpy = waitUntilAndExecArray;
        for (auto& handler : wuaxcpy) {

            if (handler->timeout != 0.f && handler->timeout > CBA_missionTime) {
                sqf::call(handler->timeOutCode, handler->args);
                handler->done = true;
                _delete = true;
            } else if (sqf::call(handler->cond, handler->args)) {
                sqf::call(handler->func, handler->args);
                handler->done = true;
                _delete = true;
            }
        }

        if (_delete) {
            waitUntilAndExecArray.erase(
                std::remove_if(waitUntilAndExecArray.begin(), waitUntilAndExecArray.end(), [](const std::shared_ptr<waitUntilAndExecHandler>& hand) {
                return hand->done;
            }), waitUntilAndExecArray.end());
        }

        return {};
    });
    auto orig = sqf::get_variable(sqf::mission_namespace(), "cba_common_fnc_onFrame");
    auto newCode = sqf::compile(onFrame.first);

    swapFuncs(orig, newCode);



    static auto addPFH = client::generate_custom_callback([](game_value_parameter arg) -> game_value {
        /*
        _function - The function you wish to execute. <CODE>
        _delay    - The amount of time in seconds between executions, 0 for every frame. (optional, default: 0) <NUMBER>
        _args     - Parameters passed to the function executing. This will be the same array every execution. (optional) <ANY>
        */
        game_value function = arg[0];
        float delay = 0;
        game_value args;

        if (arg.size() > 1)
            delay = arg[1];
        if (arg.size() > 2)
            args = arg[2];

        perFrameHandlerArray.emplace_back(
            std::make_shared<pfh>(
                function, delay, sqf::diag_ticktime(), sqf::diag_ticktime(), args, PFHHandle)

        );

        PFHHandle += 1.f;

        return PFHHandle - 1.f;
    });
    orig = sqf::get_variable(sqf::mission_namespace(), "CBA_fnc_addPerFrameHandler");
    newCode = sqf::compile(addPFH.first);

    swapFuncs(orig, newCode);


    static auto remPFH = client::generate_custom_callback([](game_value_parameter arg) -> game_value {
        int handle = arg;
        for (auto& handler : perFrameHandlerArray) {
            if (handle > handler->handle) {
                handler->del = true;
            }
        }

        return {};
    });
    orig = sqf::get_variable(sqf::mission_namespace(), "CBA_fnc_removePerFrameHandler");
    newCode = sqf::compile(remPFH.first);

    swapFuncs(orig, newCode);






    auto pfhs = sqf::get_variable(sqf::mission_namespace(), "CBA_common_perFrameHandlerArray");

    PFHHandle = pfhs.size();

    for (auto& it : pfhs.to_array()) {
        perFrameHandlerArray.emplace_back(std::make_shared<pfh>(it[0], it[1], it[2], it[3], it[4], it[5]));
    }


    static auto nextFrame = client::generate_custom_callback([](game_value_parameter arg) -> game_value {
        /*
        _function - The function you wish to execute. <CODE>
        _args     - Parameters passed to the function executing. This will be the same array every execution. (optional) <ANY>
        */


        game_value function;
        game_value args;
        if (arg.type_enum() == GameDataType::CODE) {
            function = arg;
        } else {
            function = arg[0];
            if (arg.size() > 1)
                args = arg[1];
        }





        if (sqf::diag_frameno() != nextFrameNo) {
            nextFrameBufferA.emplace_back(args, function);
        } else {
            nextFrameBufferB.emplace_back(args, function);
        }

        return {};
    });
    orig = sqf::get_variable(sqf::mission_namespace(), "CBA_fnc_execNextFrame");
    newCode = sqf::compile(nextFrame.first);

    swapFuncs(orig, newCode);


    auto fba = sqf::get_variable(sqf::mission_namespace(), "CBA_common_nextFrameBufferA");
    auto fbb = sqf::get_variable(sqf::mission_namespace(), "CBA_common_nextFrameBufferB");

    for (auto& it : fba.to_array()) {
        nextFrameBufferA.emplace_back(it[0], it[1]);
    }
    for (auto& it : fbb.to_array()) {
        nextFrameBufferB.emplace_back(it[0], it[1]);
    }



    static auto waitUntilExec = client::generate_custom_callback([](game_value_parameter arg) -> game_value {
        /*
        _condition   - The function to evaluate as condition. <CODE>
        _statement   - The function to run once the condition is true. <CODE>
        _args        - Parameters passed to the functions (statement and condition) executing. (optional) <ANY>
        _timeout     - Timeout for the condition in seconds. (optional) <NUMBER>
        _timeoutCode - Will execute instead of _statement if the condition times out. (optional) <CODE>
        */
        game_value cond = arg[0];
        game_value function = arg[1];
        game_value args;
        float timeout = 0.f;
        game_value timeoutCode;

        if (arg.size() > 2)
            args = arg[2];
        if (arg.size() > 3)
            timeout = arg[3];
        if (arg.size() > 4)
            timeoutCode = arg[4];

        if (timeout == 0.f) {
            waitUntilAndExecArray.emplace_back(std::make_shared<waitUntilAndExecHandler>(args, cond, function));
        } else {
            waitUntilAndExecArray.emplace_back(

                std::make_shared<waitUntilAndExecHandler>(
                    args, cond, function, timeout + static_cast<float>(sqf::get_variable(sqf::mission_namespace(), "CBA_missionTime")), timeoutCode
                    ));
        }

        return {};
    });
    orig = sqf::get_variable(sqf::mission_namespace(), "CBA_fnc_waitUntilAndExecute");
    newCode = sqf::compile(waitUntilExec.first);

    swapFuncs(orig, newCode);


    auto waux = sqf::get_variable(sqf::mission_namespace(), "CBA_common_waitUntilAndExecArray");

    for (auto& it : waux.to_array()) {
        waitUntilAndExecArray.emplace_back(std::make_shared<waitUntilAndExecHandler>(it[0], it[1], it[2]));
    }




    static auto waitExec = client::generate_custom_callback([](game_value_parameter arg) -> game_value {
        /*
        _function - The function you wish to execute. <CODE>
        _args     - Parameters passed to the function executing. (optional) <ANY>
        _delay    - The amount of time in seconds before the code is executed. (optional, default: 0) <NUMBER>
        */
        game_value function = arg[0];
        game_value args;
        float delay = 0.f;

        if (arg.size() > 1)
            args = arg[1];
        if (arg.size() > 2)
            delay = arg[2];

        waitAndExecArray.emplace_back(
            std::make_shared<waitAndExecHandler>(
                static_cast<float>(sqf::get_variable(sqf::mission_namespace(), "CBA_missionTime")) + delay,
                function, args));

        waitAndExecArrayIsSorted = false;

        return {};
    });
    orig = sqf::get_variable(sqf::mission_namespace(), "CBA_fnc_waitAndExecute");
    newCode = sqf::compile(waitExec.first);

    swapFuncs(orig, newCode);


    auto wax = sqf::get_variable(sqf::mission_namespace(), "CBA_common_waitAndExecArray");

    for (auto& it : wax.to_array()) {
        waitAndExecArray.emplace_back(std::make_shared<waitAndExecHandler>(it[0], it[1], it[2]));
    }
    waitAndExecArrayIsSorted = false;





}

void cba::postInit() {

}

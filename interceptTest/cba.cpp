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

game_value hashSet(game_value hashMap, game_value args) {
    if (hashMap.is_nil() || args.size() != 2) return {}; //WTF U doin u idiot?! Stop givin me that crap
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());


    map->map[args[0]] = args[1];
#ifdef CBA_HASH_LOG
    std::stringstream str;
    str << "hashSetVar" << map << " " << static_cast<r_string>(args[0]) << "=" << static_cast<r_string>(args[1]) << "\n";
    OutputDebugStringA(str.str().c_str());
#endif
    return {};
}

game_value hashFind(game_value hashMap, game_value toFind) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
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

game_value hashRemove(game_value hashMap, game_value toRemove) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
    auto found = map->map.find(toRemove);
    if (found != map->map.end()) map->map.erase(found);
    return {};
}

game_value hashContains(game_value toFind, game_value hashMap) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
    auto found = map->map.find(toFind);
    if (found != map->map.end()) return true;
    return false;
}

game_value hashCount(game_value hashMap) {
    if (hashMap.is_nil()) return 0;
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
    return static_cast<float>(map->map.size());
}


game_value hashSetVar(game_value hashMap, game_value args) {
    if (hashMap.is_nil() || args.size() < 2) return {}; //WTF U doin?
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());


    map->map[args[0]] = args[1];
#ifdef CBA_HASH_LOG
    std::stringstream str;
    str << "hashSetVar" << map << " " << static_cast<r_string>(args[0]) << "=" << static_cast<r_string>(args[1]) << "\n";
    OutputDebugStringA(str.str().c_str());
#endif

    return {};
}

game_value hashGetVarDef(game_value hashMap, game_value args) {
    if (hashMap.is_nil() || args.size() < 2) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
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

game_value hashGetVarStr(game_value hashMap, game_value args) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
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

game_value hashGetKeyList(game_value hashMap) {
    if (hashMap.is_nil()) return 0;
    auto_array<game_value> keys;
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
    for (auto& k : map->map) {
        keys.emplace_back(k.first);
    }

    return std::move(keys);
}


#pragma endregion hashMap


game_value getNumberWithDef(intercept::types::game_value right_arg) {
    if (right_arg.size() != 2) return {};
    if (sqf::is_number(right_arg[0]))
        return sqf::get_number(right_arg[0]);
    return right_arg[1];
}

game_value getTextWithDef(intercept::types::game_value right_arg) {
    if (right_arg.size() != 2) return {};
    if (sqf::is_number(right_arg[0]))
        return sqf::get_text(right_arg[0]);
    return right_arg[1];
}
game_value getArrayWithDef(intercept::types::game_value right_arg) {
    if (right_arg.size() != 2) return {};
    if (sqf::is_number(right_arg[0]))
        return sqf::get_array(right_arg[0]);
    return right_arg[1];
}
game_value getBoolWithDef(intercept::types::game_value right_arg) {
    if (right_arg.size() != 2) return {};
    if (sqf::is_number(right_arg[0]))
        return sqf::get_number(right_arg[0]) > 0.f;
    if (sqf::is_text(right_arg[0]))
        return r_string(sqf::get_text(right_arg[0])).compare_case_insensitive("true");

    return right_arg[1];
}

game_value getAnyWithDef(intercept::types::game_value right_arg) {
    if (right_arg.size() != 2) return {};
    if (sqf::is_number(right_arg[0]))
        return sqf::get_number(right_arg[0]);
    if (sqf::is_text(right_arg[0]))
        return sqf::get_text(right_arg[0]);
    if (sqf::is_array(right_arg[0]))
        return sqf::get_array(right_arg[0]);
    return right_arg[1];
}

game_value selectLast(game_value right_arg) {
    return right_arg.to_array().back();
}

game_value popEnd(game_value right_arg) {
    auto elem = right_arg.to_array().back();
    right_arg.to_array().erase(right_arg.to_array().end()-1);
    return elem;
}

game_value popFront(game_value right_arg) {
    auto elem = right_arg.to_array().front();
    right_arg.to_array().erase(right_arg.to_array().begin());
    return elem;
}

game_value pushFront(game_value left_arg, game_value right_arg) {
    auto& arr = left_arg.to_array();
    arr.emplace(arr.begin(), right_arg);
    return {};
}

game_value pushFrontUnique(game_value left_arg, game_value right_arg) {
    auto& arr = left_arg.to_array();
    if (arr.find(right_arg) == arr.end()) {
        arr.emplace(arr.begin(), right_arg);
        return true;
    }
    return false;
}


game_value findCaseInsensitive(game_value left_arg, game_value right_arg) {
    bool searchIsString = right_arg.type() == game_data_string::type_def;
    auto& arr = left_arg.to_array();
    for (int it = 0; it < left_arg.size() ; it++) {
        auto& element = arr[it];
          if (searchIsString && element.data && element.type() == game_data_string::type_def) {
              if (static_cast<r_string>(element).compare_case_insensitive(static_cast<r_string>(right_arg).c_str())) return it;
          } else {
              if (element == right_arg) return it;
          }
    }
    return -1;
}

game_value inArrayCaseInsensitive(game_value right_arg, game_value left_arg) {
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

game_value stringStartsWith(game_value left_arg, game_value right_arg) {
    auto leftStr = static_cast<sqf_string>(left_arg);
    auto rightStr = static_cast<sqf_string>(right_arg);
    if (rightStr.size() > leftStr.size()) return false;
    return (strncmp(leftStr.c_str(), rightStr.c_str(), std::min(leftStr.size(), rightStr.size())) == 0);
}

game_value stringStartsWithCI(game_value left_arg, game_value right_arg) {
    auto leftStr = static_cast<sqf_string>(left_arg);
    auto rightStr = static_cast<sqf_string>(right_arg);
    if (rightStr.size() > leftStr.size()) return false;
    return (_strnicmp(leftStr.c_str(), rightStr.c_str(), std::min(leftStr.size(), rightStr.size())) == 0);
}


game_value arrayUnion(game_value left_arg, game_value right_arg) {
    auto& leftArr = left_arg.to_array();
    auto& rightArr = right_arg.to_array();
    auto_array<game_value> output(leftArr);
    for (auto& elem : rightArr) {
        if (output.find(elem) != leftArr.end()) leftArr.emplace_back(elem);
    }
    return output;
}

game_value naturalLog(game_value right_arg) {
    return log(static_cast<float>(right_arg));
}

game_value nthRoot(game_value left_arg, game_value right_arg) {
    return std::pow(static_cast<float>(right_arg), 1.0f / static_cast<float>(left_arg));
}

game_value logn(game_value left_arg, game_value right_arg) {
    return std::log(static_cast<float>(right_arg))/ std::log(static_cast<float>(left_arg));
}

game_value regexReplace(game_value left_arg, game_value right_arg) {
    if (right_arg.size() != 2) return "";
    std::regex regr((std::string)right_arg[0]);
    return std::regex_replace((std::string)left_arg, regr, (std::string)right_arg[1]);
}

game_value nothing0() {
    return {};
}
game_value nothing1(game_value) {
    return {};
}
game_value nothing2(game_value, game_value) {
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


game_value instructionCount(game_value code) {
    if (code.is_nil()) return 0;
    auto c = (codeWithCexp*) code.data.getRef();
    if (!c->instrarr) return 0;
    return (float)c->instrarr->count;

    return {};
}


game_value getObjectConfigFromObj(game_value obj) {
    auto type = sqf::type_of(obj);

    for (std::string_view cls : { "CfgVehicles"sv, "CfgAmmo"sv, "CfgNonAIVehicles"sv }) {
        auto cfgClass = sqf::config_entry() >> cls >> type;
        if (sqf::is_class(cfgClass)) return static_cast<game_value>(cfgClass);

    }
    return static_cast<game_value>(sqf::config_null());
}

game_value getObjectConfigFromStr(game_value className) {
    sqf_string type = className;
    for (auto& cls : { "CfgVehicles"sv, "CfgAmmo"sv, "CfgNonAIVehicles"sv }) {
        auto cfgClass = sqf::config_entry() >> cls >> type;
        if (sqf::is_class(cfgClass)) return static_cast<game_value>(cfgClass);

    }
    return static_cast<game_value>(sqf::config_null());
}

game_value getItemConfigFromObj(game_value obj) {
    auto type = sqf::type_of(obj);

    for (auto& cls : { "CfgWeapons"sv, "CfgMagazines"sv, "CfgGlasses"sv }) {
        auto cfgClass = sqf::config_entry() >> cls >> type;
        if (sqf::is_class(cfgClass)) return static_cast<game_value>(cfgClass);

    }
    return static_cast<game_value>(sqf::config_null());
}

game_value getItemConfigFromStr(game_value className) {
    sqf_string type = className;
    for (auto& cls : { "CfgWeapons"sv, "CfgMagazines"sv, "CfgGlasses"sv }) {
        auto cfgClass = sqf::config_entry() >> cls >> type;
        if (sqf::is_class(cfgClass)) return static_cast<game_value>(cfgClass);

    }
    return static_cast<game_value>(sqf::config_null());
}


//https://github.com/CBATeam/CBA_A3/blob/master/addons/common/fnc_turretPath.sqf
game_value turretPath(game_value unit) {
    auto vehicle = sqf::vehicle(unit);

    for (auto& turret : sqf::all_turrets(vehicle, true)) {
        if (sqf::turret_unit(vehicle, turret) == unit) return turret;
    }
    return { auto_array<game_value>() }; //empty array
}


game_value aliveGroup(game_value grp) {
    for (auto& unit : sqf::units(static_cast<group>(grp)))
        if (sqf::alive(unit)) return true;
    return false;
}

game_value unarySpawn(game_value code) {
    return static_cast<game_value>(sqf::spawn({}, code));
}

game_value hasItem(game_value obj, game_value classn) {
    r_string classname = classn;
    auto containsString = [&classname](const sqf_return_string_list& list)
    {
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

game_value getObjPosRaw(game_value obj) {
    return sqf::model_to_world_visual_world(obj, {0,0,0});
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

    object _player = sqf::call(CBA_fnc_currentUnit);
    if (_player != CBA_oldUnit) {
        sqf::call(CBA_fnc_localEvent, { "cba_common_unitEvent", { _player , CBA_oldUnit } });
        CBA_oldUnit = _player;
    }

    game_value _data = sqf::get_group(_player);
    if (_data != CBA_oldGroup) {
        sqf::call(CBA_fnc_localEvent,{ "cba_common_groupEvent",{ _data , CBA_oldGroup } });
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


void cba::preStart() {

    auto codeType = client::host::registerType(r_string("HASHMAP"), r_string("hashMap"), r_string("Dis is a hashmap. It hashes things."), r_string("hashMap"), createGameDataHashMap);
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
}

void cba::postInit() {

}

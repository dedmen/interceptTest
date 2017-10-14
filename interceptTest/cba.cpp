#define NOMINMAX
#include "cba.h"
#include <client/headers/intercept.hpp>

using namespace intercept;


#pragma region hashMap
types::registered_sqf_function _createHashMap;
types::registered_sqf_function _hashMapSet;
types::registered_sqf_function _hashMapFind;
types::registered_sqf_function _hashMapRemove;
types::registered_sqf_function _hashMapContains;
types::registered_sqf_function _hashMapCount;
types::registered_sqf_function _hashMapSelect;
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
    std::map<size_t, game_value> map;
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

game_value hashSet(game_value hashMap, game_value args) {
    if (hashMap.is_nil() || args.size() != 2) return {}; //WTF U doin u idiot?! Stop givin me that crap
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());


    map->map[args[0].hash()] = args[1];
    return {};
}

game_value hashFind(game_value hashMap, game_value toFind) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
    auto found = map->map.find(toFind.hash());
    if (found != map->map.end()) return found->second;
    return {};
}

game_value hashRemove(game_value hashMap, game_value toRemove) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
    auto found = map->map.find(toRemove.hash());
    if (found != map->map.end()) map->map.erase(found);
    return {};
}

game_value hashContains(game_value toFind, game_value hashMap) {
    if (hashMap.is_nil()) return {};
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
    auto found = map->map.find(toFind.hash());
    if (found != map->map.end()) return true;
    return false;
}

game_value hashCount(game_value hashMap) {
    if (hashMap.is_nil()) return 0;
    auto map = static_cast<GameDataHashMap*>(hashMap.data.getRef());
    return static_cast<float>(map->map.size());
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


    static auto _getNumberWithDef = intercept::client::host::registerFunction("getNumber", "", userFunctionWrapper<getNumberWithDef>, GameDataType::SCALAR, GameDataType::ARRAY);
    static auto _getTextWithDef = intercept::client::host::registerFunction("getText", "", userFunctionWrapper<getTextWithDef>, GameDataType::STRING, GameDataType::ARRAY);
    static auto _getArrayWithDef = intercept::client::host::registerFunction("getArray", "", userFunctionWrapper<getArrayWithDef>, GameDataType::ARRAY, GameDataType::ARRAY);
    static auto _getBoolWithDef = intercept::client::host::registerFunction("getBool", "", userFunctionWrapper<getBoolWithDef>, GameDataType::BOOL, GameDataType::ARRAY);
    static auto _getAnyWithDef = intercept::client::host::registerFunction("getAny", "", userFunctionWrapper<getAnyWithDef>, GameDataType::ANY, GameDataType::ARRAY);
    static auto _selectLast = intercept::client::host::registerFunction("selectLast", "", userFunctionWrapper<selectLast>, GameDataType::ANY, GameDataType::ARRAY);
    static auto _popEnd = intercept::client::host::registerFunction("popEnd", "", userFunctionWrapper<popEnd>, GameDataType::ANY, GameDataType::ARRAY);
    static auto _popFront = intercept::client::host::registerFunction("popFront", "", userFunctionWrapper<popFront>, GameDataType::ANY, GameDataType::ARRAY);
    static auto _findCI = intercept::client::host::registerFunction("findCI", "", userFunctionWrapper<findCaseInsensitive>, GameDataType::ANY, GameDataType::ARRAY, GameDataType::ANY);
    static auto _inArrayCI = intercept::client::host::registerFunction("inCI", "", userFunctionWrapper<inArrayCaseInsensitive>, GameDataType::ANY, GameDataType::ANY, GameDataType::ARRAY);
    static auto _startsWith = intercept::client::host::registerFunction("startsWith", "", userFunctionWrapper<stringStartsWith>, GameDataType::BOOL, GameDataType::STRING, GameDataType::STRING);
    static auto _startsWithCI = intercept::client::host::registerFunction("startsWithCI", "", userFunctionWrapper<stringStartsWith>, GameDataType::BOOL, GameDataType::STRING, GameDataType::STRING);
    static auto _arrayUnion = intercept::client::host::registerFunction("arrayUnion", "", userFunctionWrapper<arrayUnion>, GameDataType::ARRAY, GameDataType::ARRAY, GameDataType::ARRAY);

}

void cba::postInit() {

}

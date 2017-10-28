#pragma once
#define INTERCEPT_SQF_STRTYPE_RSTRING
#include <client/headers/intercept.hpp>
//#define INTEL_NO_ITTNOTIFY_API
#include <ittnotify.h>

template <typename type>
class cachedValue {
public:
    type get(std::function<type()> f) {
        if (val) return *val;
        val = f();
        return *val;
    }
    std::optional<type> val;
};


class TFAR_unit {
public:
    TFAR_unit(object _unit, bool _isNearPlayer) : unit(_unit), isNearPlayer(_isNearPlayer) {}
    bool get_TFAR_forceSpectator();
    bool get_isNearPlayer() const { return isNearPlayer; }
    sqf_return_string getUnitName() const { return unitName; }
    void setUnitName(sqf_return_string name) { unitName = name; }
    void setUnit(object _unit) { unit = _unit; }
    object getUnit() const { return unit; }
    vector3 getEyePos();
    vector3 getPos();
    vector3 getPosASL();
private:
    sqf_return_string unitName;
    bool isNearPlayer;
    cachedValue<bool> TFAR_forceSpectator;
    cachedValue<vector3> eyePos;
    cachedValue<vector3> normPos;
    cachedValue<vector3> aslPos;
    object unit;
};

class TFAR {
public:
    TFAR();
    ~TFAR();
    void preStart();
    void preInit() const;

    static vector3 TFAR_fnc_defaultPositionCoordinates(TFAR_unit& unit, bool forceSpectator, bool _isCurrentUnit);

    static bool TFAR_fnc_canUseSWRadio(object& _unit, bool _isolated_and_inside, float _depth);

    static bool TFAR_fnc_canUseLrRadio(object& _unit, bool _isolated_and_inside, float _depth);

    static game_value TFAR_fnc_preparePositionCoordinates(TFAR_unit& unit);

    static game_value TFAR_fnc_getConfigProperty(const std::string& classname, const std::string& property, game_value def = game_value());


    static std::string TFAR_fnc_vehicleID(object& _vehicle);
    static float TFAR_fnc_calcTerrainInterception(TFAR_unit& _unit);

    static float TFAR_fnc_objectInterception(TFAR_unit& _unit);


    static game_value TFAR_fnc_sendPlayerInfo(TFAR_unit& unit);


    static std::set<object> TFAR_fnc_getNearPlayers();


    static game_value TFAR_fnc_processPlayerPositions(game_value arg);
    intercept::types::registered_sqf_function _itfarProcP;
};


class interceptionCache {
public:
    interceptionCache(std::function<float(TFAR_unit&, TFAR_unit&)> _calcFunc) : calcFunc(_calcFunc) {}
    float getValue(TFAR_unit& _left, TFAR_unit& _right);
    std::function<float(TFAR_unit&, TFAR_unit&)> calcFunc;
    std::unordered_map<object, std::tuple<vector3, vector3, float>> cache;
};



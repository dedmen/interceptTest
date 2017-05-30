#include <stdio.h>
#include <cstdint>
#include <sstream>
#include <experimental/string>

// the Intercept library, only one include required.
#include <client/headers/intercept.hpp>
#include <iso646.h>
#define INTEL_NO_ITTNOTIFY_API
#include <ittnotify.h>
#include "tools.h"

using namespace intercept;
types::registered_sqf_function _interceptEventFunction;
types::registered_sqf_function _interceptEventFunction2;
types::registered_sqf_function _interceptEventFunction3;
types::registered_sqf_function _interceptEventFunction4;
static game_value TFAR_fnc_preparePositionCoordinates(game_value left_arg);

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
#include <random>
std::random_device rd;

std::mt19937 e2(rd());

vector3 randomOffset(float maxOffset) {
    std::uniform_real_distribution<float> dist(-maxOffset, maxOffset);
    return{ dist(e2),dist(e2), dist(e2) };
}

vector3 TFAR_fnc_defaultPositionCoordinates(object& _unit, bool _isNearPlayer, bool forceSpectator, bool _isCurrentUnit) {
    if (forceSpectator) return sqf::agl_to_asl(sqf::position_camera_to_world({ 0,0,0 }));



    auto p2 = static_cast<game_data_object*>(_unit.data.getRef())->get_head_pos();
    if (p2.valid)
        return p2._cameraPositionWorld;
    auto _current_eyepos = sqf::eye_pos(_unit);

    //If this is not in here then positions inside fast moving vehicles will be weird. But this is also performance intensive
    if (_isNearPlayer && sqf::velocity(_unit).magnitude() > 3 && !_isCurrentUnit) {
        _current_eyepos = sqf::visible_position(_unit) + (_current_eyepos - sqf::get_pos(_unit));
    };
    return _current_eyepos;
}

bool TFAR_fnc_canUseSWRadio(object& _unit, bool _isolated_and_inside, float _depth) {
    return  (_depth > 0) || _isolated_and_inside || sqf::is_able_to_breathe(_unit);
}

bool TFAR_fnc_canUseLrRadio(object& _unit, bool _isolated_and_inside, float _depth) {
    if (_depth > 0) return true;

    if (_depth > -3 && !sqf::is_equal_to(sqf::vehicle(_unit), _unit)) {
        if ((_isolated_and_inside) || sqf::is_able_to_breathe(_unit)) return true;
    };
    return false;
}



game_value TFAR_fnc_getConfigProperty(const std::string& classname, const std::string& property, game_value def = game_value()) {
    static std::map<std::string, game_value> configCache;

    auto _cacheName = classname + property;
    auto found = configCache.find(_cacheName);
    if (found != configCache.end()) return found->second;

    auto _cfgProperty = (sqf::config_entry() >> "CfgVehicles" >> classname >> property);
            
    if (sqf::is_number(_cfgProperty)){
        game_value _value = sqf::get_number(_cfgProperty);
        configCache.insert_or_assign(_cacheName, _value);
        return _value;
    }

    if (sqf::is_text(_cfgProperty)) {
        game_value _value = sqf::get_text(_cfgProperty);
        configCache.insert_or_assign(_cacheName, _value);
        return _value;
    }

    //if (sqf::is_array(_cfgProperty)) {
    //    game_value _value = sqf::get_array(_cfgProperty);
    //    configCache.insert_or_assign(_cacheName, _value);
    //    return _value;
    //}
    configCache.insert_or_assign(_cacheName, def);
    return def;
}



std::string TFAR_fnc_vehicleID(object& _vehicle) {
    auto _netID = sqf::net_id(_vehicle);
    //static auto TFAR_fnc_getConfigProperty = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getConfigProperty");
    auto typeOf = sqf::type_of(_vehicle);
    
    float _hasIntercom = TFAR_fnc_getConfigProperty(typeOf, "TFAR_hasIntercom", 0.f);
    float _intercomSlot = -1;

    if (_hasIntercom > 0.f) {
        _intercomSlot = sqf::get_variable(_vehicle, std::string("TFAR_IntercomSlot_") + _netID, 0.f);
    };


    return sqf::format({ "%1\x10%2\x10%3\x10%4",_netID,
        //#TODO isTurnedOut
         //false ? "turnout" : 
        TFAR_fnc_getConfigProperty(typeOf, "tf_isolatedAmount", 0.f),
        _intercomSlot,
        sqf::velocity(_vehicle)
    });
}
float TFAR_fnc_calcTerrainInterception(object& _unit, object& TFAR_currentUnit) {

    auto _result = 0.f;
    vector3 _p1 = sqf::eye_pos(TFAR_currentUnit);
    vector3 _p2 = sqf::eye_pos(_unit);
    if (sqf::terrain_intersect_asl(_p1, _p2)) {
        auto _l = 10.0f;
        auto _r = 250.0f;
        auto _m = 100.0f;

        vector3 _middle = {
            ((_p1.x) + (_p2.x)) / 2.f,
            ((_p1.y) + (_p2.y)) / 2.f,
            ((_p1.z) + (_p2.z)) / 2.f
        };
        auto _base = _middle.z;

        while ((_r - _l) > 10) {
            _middle.z = _base + _m;
            if ((!sqf::terrain_intersect_asl(_p1, _middle)) && !sqf::terrain_intersect_asl(_p2, _middle)) {
                _r = _m;
            } else {
                _l = _m;
            };
            _m = (_l + _r) / 2.0f;
        };
        _result = _m;
    };
    return _result;
}

float TFAR_fnc_objectInterception(object& _unit, object& TFAR_currentUnit) {
    auto _ins = sqf::line_intersects_surfaces(
        sqf::eye_pos(TFAR_currentUnit), sqf::eye_pos(_unit),
        _unit,
        TFAR_currentUnit,
        true,
        10, "FIRE", "NONE"
    );

    auto _localParent = sqf::object_parent(TFAR_currentUnit);
    auto _remoteParent = sqf::object_parent(_unit);
    float count = 0.f;
    for (auto& it : _ins) {
        if (_localParent.data != it.intersect_object.data &&
            _remoteParent.data != it.intersect_object.data)
            count += 1.f;
    }
    return count;
}

game_value redirectWrap(game_value left_arg, game_value right_arg) {
    return u8"heööai?";
}
game_value redirectWrapUnary(game_value right_arg) {
    return TFAR_fnc_preparePositionCoordinates(std::move(right_arg));
}
game_value redirectWrapNular() {
    return u8"heööai?";
}
__itt_domain* domain = __itt_domain_create("InterceptTest");
__itt_string_handle* handle_TFAR_fnc_preparePositionCoordinates = __itt_string_handle_create("TFAR_fnc_preparePositionCoordinates");
__itt_string_handle* handle_TFAR_fnc_sendPlayerInfo = __itt_string_handle_create("TFAR_fnc_sendPlayerInfo");
static game_value TFAR_fnc_preparePositionCoordinates(game_value args) {
    __itt_task_begin(domain, __itt_null, __itt_null, handle_TFAR_fnc_preparePositionCoordinates);
    object _unit = args[0];
    bool _nearPlayer = args[1];
    std::string _unitName = args[2];

    auto _isSpectating = sqf::get_variable(_unit, "TFAR_forceSpectator", false);
    object TFAR_currentUnit = sqf::get_variable(sqf::mission_namespace(), "TFAR_currentUnit");
    auto _isRemotePlayer = !sqf::is_equal_to(_unit, TFAR_currentUnit);

    vector3 _pos = TFAR_fnc_defaultPositionCoordinates(_unit, _nearPlayer, _isSpectating, !_isRemotePlayer);
    auto TF_fnc_position = sqf::get_variable(_unit, "TF_fnc_position");
    if (!TF_fnc_position.is_nil())
        _pos = sqf::call(TF_fnc_position, game_value({ _unit ,_nearPlayer }));
    auto _isolated_and_inside = false; //_isInVehicle && {_unit call TFAR_fnc_vehicleIsIsolatedAndInside};
    std::string _vehicle = "no"; //if (_isInVehicle) then {_unit call TFAR_fnc_vehicleId} else {"no"};
    auto vehicle = sqf::object_parent(_unit);
    if (!sqf::is_null(vehicle)) {
        _vehicle = TFAR_fnc_vehicleID(vehicle);// sqf::call(sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_vehicleId"), _unit);
        static auto TFAR_fnc_vehicleIsIsolatedAndInside = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_vehicleIsIsolatedAndInside");
        _isolated_and_inside = sqf::call(TFAR_fnc_vehicleIsIsolatedAndInside, _unit);
    }
    auto _eyeDepth = _pos.z;//Inlined version of TFAR_fnc_eyeDepth to save performance
    auto _can_speak = (_eyeDepth > 0 || _isolated_and_inside); //Inlined version of TFAR_fnc_canSpeak to save performance
    auto _useSw = true;
    auto _useLr = true;
    auto _useDd = false;
    if (_eyeDepth < 0) {
        _useSw = TFAR_fnc_canUseSWRadio(_unit, _isolated_and_inside, _eyeDepth);
        _useLr = TFAR_fnc_canUseLrRadio(_unit, _isolated_and_inside, _eyeDepth);
        _useDd = sqf::call(sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_canUseDDRadio"), { _eyeDepth, _isolated_and_inside });
    };

    if (_nearPlayer && sqf::distance(sqf::get_pos(TFAR_currentUnit), _pos) <= (float) sqf::get_variable(sqf::mission_namespace(), "TF_speakerDistance")) {
        static auto TFAR_fnc_getLrSpeakers = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getLrSpeakers");
        static auto TFAR_fnc_lrRadiosList = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_lrRadiosList");
        static auto TFAR_fnc_getLrFrequency = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getLrFrequency");
        static auto TFAR_fnc_getLrRadioCode = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getLrRadioCode");
        static auto TFAR_fnc_getAdditionalLrChannel = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getAdditionalLrChannel");
        static auto TFAR_fnc_getChannelFrequency = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getChannelFrequency");
        static auto TFAR_fnc_getLrVolume = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getLrVolume");
        static auto TFAR_fnc_getSwSpeakers = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getSwSpeakers");
        static auto TFAR_fnc_radiosList = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_radiosList");
        static auto TFAR_fnc_getSwFrequency = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getSwFrequency");
        static auto TFAR_fnc_getSwRadioCode = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getSwRadioCode");
        static auto TFAR_fnc_getAdditionalSwChannel = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getAdditionalSwChannel");
        static auto TFAR_fnc_getSwVolume = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getSwVolume");

        if (sqf::get_variable(_unit, "TFAR_LRSpeakersEnabled", false) && _useLr) {
            auto lrRadiosList = sqf::call(TFAR_fnc_lrRadiosList, _unit);
            auto lrRadioCount = lrRadiosList.size();
            for (int Index = 0; Index < lrRadioCount; Index++) {
                auto _x = lrRadiosList[Index];

                if (sqf::call(TFAR_fnc_getLrSpeakers, _x)) {
                    std::vector<std::string> _frequencies;
                    std::string freq = sqf::call(TFAR_fnc_getLrFrequency, _x);
                    std::string code = sqf::call(TFAR_fnc_getLrRadioCode, _x);

                    _frequencies.push_back(freq + code);
                    float additonalLrChannel = sqf::call(TFAR_fnc_getAdditionalLrChannel, _x);
                    if (additonalLrChannel > -1.f) {
                        std::string freq = sqf::call(TFAR_fnc_getChannelFrequency, { _x,additonalLrChannel + 1.f });
                        _frequencies.push_back(freq + code);
                    };

                    auto _radio_id = sqf::net_id((object) _x[0]);
                    float lrVolume = sqf::call(TFAR_fnc_getLrVolume, _x);
                    vector3 pos = sqf::get_pos_asl(_unit);
                    std::string radioInfo = _radio_id + "\xA";
                    bool first = true;
                    for (auto& it : _frequencies) {
                        if (!first) radioInfo += "|" + it;
                        else radioInfo += it;
                        first = false;
                    }
                    radioInfo += "\xA" + _unitName + "\xA[]\xA" + std::to_string(lrVolume) + "\xA" + _vehicle + "\xA" + std::to_string(pos.z);

                };
            }

        };

        if (sqf::get_variable(_unit, "TFAR_SRSpeakersEnabled", false) && _useSw) {

            auto swRadiosList = sqf::call(TFAR_fnc_radiosList, _unit);
            auto swRadioCount = swRadiosList.size();
            for (int Index = 0; Index < swRadioCount; Index++) {
                auto _x = swRadiosList[Index];
                if (sqf::call(TFAR_fnc_getSwSpeakers, _x)) {
                    std::vector<std::string> _frequencies;
                    std::string freq = sqf::call(TFAR_fnc_getSwFrequency, _x);
                    std::string code = sqf::call(TFAR_fnc_getSwRadioCode, _x);

                    _frequencies.push_back(freq + code);
                    float additonalSwChannel = sqf::call(TFAR_fnc_getAdditionalSwChannel, _x);
                    if (additonalSwChannel > -1.f) {
                        std::string freq = sqf::call(TFAR_fnc_getChannelFrequency, { _x,additonalSwChannel + 1.f });
                        _frequencies.push_back(freq + code);
                    };

                    auto _radio_id = sqf::net_id((object) _x[0]);
                    float swVolume = sqf::call(TFAR_fnc_getSwVolume, _x);
                    vector3 pos = sqf::get_pos_asl(_unit);
                    std::string radioInfo = _radio_id + "\xA";
                    bool first = true;
                    for (auto& it : _frequencies) {
                        if (!first) radioInfo += "|" + it;
                        else radioInfo += it;
                        first = false;
                    }
                    radioInfo += "\xA" + _unitName + "\xA[]\xA" + std::to_string(swVolume) + "\xA" + _vehicle + "\xA" + std::to_string(pos.z);
                }
            }
        }
    }

    auto _isEnemy = false;
    if (_isRemotePlayer && sqf::get_variable(TFAR_currentUnit, "TFAR_forceSpectator", false)) { //If we are not spectating we are not interested if he is enemy
        static auto BIS_fnc_sideIsEnemy = sqf::get_variable(sqf::mission_namespace(), "BIS_fnc_sideIsEnemy");
        _isEnemy = sqf::call(BIS_fnc_sideIsEnemy, { sqf::player_side(), sqf::get_side(_unit) });
    };
    vector3 _curViewDir = _isSpectating ? (sqf::position_camera_to_world({ 0,0,1 }) - sqf::position_camera_to_world({ 0,0,0 })) : sqf::get_camera_view_direction(_unit);//Inlined version of TFAR_fnc_currentDirection
    static bool objectInterceptionEnabled = sqf::get_variable(sqf::mission_namespace(), "TFAR_objectInterceptionEnabled");

    std::vector<game_value> _data = {
            game_value("POS	%1	%2	%3	%4	%5	%6	%7	%8	%9	%10	%11	%12	%13"),
            game_value(_unitName),
            game_value(_pos),
            game_value(_curViewDir),//Position
            game_value(_can_speak),
            game_value(_useSw),
            game_value(_useLr),
            game_value(_useDd),
            game_value(_vehicle),
            game_value(_nearPlayer ? 0.f : TFAR_fnc_calcTerrainInterception(_unit,TFAR_currentUnit)),
            game_value(sqf::get_variable(_unit, "tf_voiceVolume", 1.0f)),//Externally used API variable. Don't change name
            game_value((_isRemotePlayer && objectInterceptionEnabled) ? TFAR_fnc_objectInterception(_unit,TFAR_currentUnit) : 0.f), //Interceptions
            game_value(_isSpectating),
            game_value(_isEnemy) };
    auto ret = sqf::format(_data);//format is actually faster. 0.128 vs 0.131
    __itt_task_end(domain);
    return ret;
}

game_value TFAR_fnc_sendPlayerInfo(game_value args) {
    __itt_task_begin(domain, __itt_null, __itt_null, handle_TFAR_fnc_sendPlayerInfo);
    static auto TFAR_fnc_fireEventHandlers = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_fireEventHandlers");
    static auto TFAR_fnc_showHint = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_showHint");
    static auto TFAR_fnc_hideHint = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_hideHint");
    auto missionNamespace = sqf::mission_namespace();

    object _player = args[0];
    auto _request = TFAR_fnc_preparePositionCoordinates(args);

    auto result = sqf::call_extension("task_force_radio_pipe", _request);


    if ((result != "OK") && result.length() != 2) {
        auto parsed = sqf::parse_text(result);
        sqf::call(TFAR_fnc_showHint, game_value{ parsed,10.f });
        sqf::set_variable(missionNamespace, "tf_lastError", true);
        return{};
    } else {
        if (sqf::get_variable(missionNamespace, "tf_lastError", game_value(false))) {
            sqf::call(TFAR_fnc_hideHint);
            sqf::set_variable(missionNamespace, "tf_lastError", false);
        };
    };

    bool _isSpeaking = result.front() == '1';
    bool _isReceiving = result.length() == 2 && result.back() == '1';

    if (_isSpeaking) {
        sqf::set_variable(*reinterpret_cast<rv_namespace*>(&_player), "TFAR_speakingSince", sqf::diag_ticktime());
    };
    sqf::set_random_lip(_player, _isSpeaking);



    //Only want to fire EH once
    if (static_cast<bool>(sqf::get_variable(missionNamespace, "TFAR_isSpeaking", false)) != _isSpeaking) {
        sqf::set_variable(*reinterpret_cast<rv_namespace*>(&_player), "TFAR_isSpeaking", game_value(_isSpeaking));
        sqf::set_variable(*reinterpret_cast<rv_namespace*>(&_player), "TF_isSpeaking", game_value(_isSpeaking));
        sqf::call(TFAR_fnc_fireEventHandlers, game_value{ "OnSpeak" , game_value{ (static_cast<game_value>(_player)), game_value(_isSpeaking) } });
    };

    if (static_cast<bool>(sqf::get_variable(missionNamespace, "TFAR_isReceiving", false)) != _isReceiving) {
        sqf::set_variable(*reinterpret_cast<rv_namespace*>(&_player), "TFAR_isReceiving", game_value(_isReceiving));
        sqf::call(TFAR_fnc_fireEventHandlers, game_value{ "OnRadioReceive" ,game_value{ (static_cast<game_value>(_player)), game_value(_isReceiving) } });
    };


    if (!sqf::get_variable(missionNamespace, "TFAR_killedEHAttached", false)) {
        sqf::add_event_handler(_player, "killed", "_player call TFAR_fnc_sendPlayerKilled");
        sqf::set_variable(*reinterpret_cast<rv_namespace*>(&_player), "TFAR_killedEHAttached", true);
    };
    __itt_task_end(domain);
    return{};
}

bool TFAR_currentNearPlayersProcessed = false;
std::queue<object> TFAR_currentNearPlayersProcessing;
bool TFAR_currentFarPlayersProcessed = false;
std::queue<object> TFAR_currentFarPlayersProcessing;
std::set<object> TFAR_currentFarPlayers;
std::set<object> TFAR_currentNearPlayers;
std::chrono::system_clock::time_point TFAR_lastFarPlayerProcessTime;
std::chrono::system_clock::time_point TFAR_lastPlayerScanTime;

__itt_string_handle* handle_nearPlayerProcess = __itt_string_handle_create("nearPlayerProcess");
__itt_string_handle* handle_farPlayerProcess = __itt_string_handle_create("farPlayerProcess");
__itt_string_handle* handle_playerScan = __itt_string_handle_create("playerScan");
__itt_string_handle* handle_UnitName = __itt_string_handle_create("unitName");

class __ittFrameWatch {
public:
    __ittFrameWatch() { __itt_frame_end_v3(domain, NULL); }
    ~__ittFrameWatch() { __itt_frame_end_v3(domain, NULL); }
};

//class TFAR_workerThreadPool {
//
//
//    void threadRun() {
//        while (shouldRun) {
//            std::unique_lock<std::mutex> lock(theadMutex);
//            threadWorkCondition.wait(lock, [this] {return !commandQueue.empty() || !shouldRun; });
//            if (!shouldRun) return;
//            std::string command(std::move(commandQueue.front())); commandQueue.pop();
//            lock.unlock();
//            TFAR_pr(command);
//        }
//    }
//    void addThread() {
//            _threads.emplace_back(std::make_unique<std::thread>(&TFAR_workerThreadPool::threadRun, this));
//        {
//            std::lock_guard<std::mutex> lock(theadMutex);
//            commandQueue.emplace(command);
//        }
//        threadWorkCondition.notify_one();
//    }
//    
//    std::vector<>
//    std::condition_variable threadWorkCondition;
//    std::mutex theadMutex;
//    bool shouldRun;
//    std::vector<std::unique_ptr<std::thread>> _threads;
//};

std::set<object> TFAR_fnc_getNearPlayers() {
    std::set<object> nearPlayers;
    object TFAR_currentUnit = sqf::get_variable(sqf::mission_namespace(), "TFAR_currentUnit");
    TFAR_currentUnit.size();
    if (!sqf::alive(TFAR_currentUnit) && !sqf::get_variable(TFAR_currentUnit, "TFAR_forceSpectator", false)) return nearPlayers;

    auto myGroup = sqf::get_group(TFAR_currentUnit);
    auto myGroupUnits = sqf::units(myGroup);

    auto _players_in_group = myGroupUnits.size();
    auto myPos = sqf::get_pos(TFAR_currentUnit);
    float maxVoiceVolume = sqf::get_variable(sqf::mission_namespace(), "TF_max_voice_volume", 20.f);
    auto nearUnits = sqf::near_entities(myPos, { "Man" }, maxVoiceVolume + 40.f);

    nearUnits.insert(nearUnits.end(), myGroupUnits.begin(), myGroupUnits.end());
    //nearUnits.push_back(TFAR_currentUnit); //#TODO add TFAR_currentUnit if it's not already in there

    //_allUnits pushBackUnique TFAR_currentUnit;//Will be duplicate in normal play but in spectator the group units will be missing
    auto nearVehicles = sqf::near_entities(myPos, { "LandVehicle","Air","Ship" }, maxVoiceVolume + 40.f);
    for (auto& it : nearVehicles) {
        auto crew = sqf::crew(it);
        nearUnits.insert(nearUnits.end(), crew.begin(), crew.end());
    }
    for (auto& it : nearUnits) {
        if (sqf::is_player(it) && (sqf::alive(it) || sqf::get_variable(it, "TFAR_forceSpectator", false)))
            nearPlayers.insert(it);
    }

    return nearPlayers;
}


game_value TFAR_fnc_processPlayerPositions(game_value arg) {
    __ittFrameWatch frameWatch{};
    if (sqf::get_client_state_number() != 10) return "";// { "BI HAS CRAPPY WEIRD BUGS U KNOW! (Keeps PFH from firing after server disconnect)" };
    auto _startTime = std::chrono::system_clock::now();

    //Process queued Near Players
    if (!TFAR_currentNearPlayersProcessed) {
        for (int it = 0; it < 30; it++) {
            __itt_task_begin(domain, __itt_null, __itt_null, handle_nearPlayerProcess);
            if (TFAR_currentNearPlayersProcessing.empty()) { TFAR_currentNearPlayersProcessed = true; break; }
            auto _x = TFAR_currentNearPlayersProcessing.front();
            TFAR_currentNearPlayersProcessing.pop();
            auto _controlled = sqf::get_variable(_x, "TFAR_controlledUnit", game_value());
            auto _unitName = sqf::name(_x);
            __itt_metadata_str_add(domain, __itt_null, handle_UnitName, _unitName.c_str(), _unitName.length());
            if (static_cast<bool>(sqf::get_variable(_x, "TFAR_forceSpectator", false)))
                _unitName = sqf::get_variable(_x, "TFAR_spectatorName", _unitName);
            if (_controlled.is_nil())
                TFAR_fnc_sendPlayerInfo({ _x,true,_unitName });
            else
                TFAR_fnc_sendPlayerInfo({ _controlled,true,_unitName });
            __itt_task_end(domain);
        }
    }
    //Don't process anymore if we already blocked too long (5 millisec)
    if ((std::chrono::system_clock::now() - _startTime) > std::chrono::milliseconds(5)) return "";

    //Process queued Far players
    if (!TFAR_currentFarPlayersProcessed) {
        auto _nearPlayersCount = TFAR_currentFarPlayersProcessing.size();
        if (_nearPlayersCount == 0) TFAR_currentFarPlayersProcessed = true;

        for (int it = 0; it < 50; it++) { //#TODO do batches of 10 until we are running longer than 5 millisecs
            __itt_task_begin(domain, __itt_null, __itt_null, handle_farPlayerProcess);
            if (TFAR_currentFarPlayersProcessing.empty()) { TFAR_lastFarPlayerProcessTime = std::chrono::system_clock::now(); TFAR_currentFarPlayersProcessed = true; break; }
            auto _x = TFAR_currentFarPlayersProcessing.front();
            TFAR_currentFarPlayersProcessing.pop();
            auto _controlled = sqf::get_variable(_x, "TFAR_controlledUnit", game_value());
            auto _unitName = sqf::name(_x);
            if (static_cast<bool>(sqf::get_variable(_x, "TFAR_forceSpectator", false)))
                _unitName = sqf::get_variable(_x, "TFAR_spectatorName", _unitName);
            if (_controlled.is_nil())
                TFAR_fnc_sendPlayerInfo({ _x,true,_unitName });
            else
                TFAR_fnc_sendPlayerInfo({ _controlled,true,_unitName });
            __itt_task_end(domain);
        }
    }

    //Rescan Players
    bool _needNearPlayerScan = ((std::chrono::system_clock::now() - TFAR_lastPlayerScanTime) > std::chrono::seconds(1)) && TFAR_currentNearPlayersProcessed;

    if (_needNearPlayerScan) {
        __itt_task_begin(domain, __itt_null, __itt_null, handle_playerScan);
        TFAR_currentNearPlayers = TFAR_fnc_getNearPlayers();// sqf::all_units();

        for (auto& it : sqf::all_curators()) {
            auto _player = sqf::get_assigned_curator_unit(it);
            if (!_player.is_null())
                TFAR_currentNearPlayers.insert(_player);
        }

        auto _other_units = sqf::all_players();
        for (auto& it : TFAR_currentNearPlayers) {
            auto found = std::find_if(_other_units.begin(), _other_units.end(), [&it](const object& other) {
                return other.data == it.data;
            });
            if (found != _other_units.end()) _other_units.erase(found);
        }

        TFAR_currentFarPlayers.clear();
        for (auto& it : _other_units) {
            if (sqf::is_player(it))
                TFAR_currentFarPlayers.insert(it);
        }

        TFAR_lastPlayerScanTime = std::chrono::system_clock::now();
        __itt_task_end(domain);
    };

    //Queue new updates to plugin if last one processed
    if (TFAR_currentNearPlayersProcessed) {
        static auto TFAR_fnc_sendSpeakerRadios = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_sendSpeakerRadios");
        sqf::call(TFAR_fnc_sendSpeakerRadios);//send Speaker radio infos to plugin Has to be here because it needs a variable from NearPlayer processing
        //#TODO replace by native
        for (auto& it : TFAR_currentNearPlayers) {
            TFAR_currentNearPlayersProcessing.push(it);
        }
        TFAR_currentNearPlayersProcessed = false;
    };

    //Only process FarPlayers once a TFAR_FAR_PLAYER_UPDATE_TIME
    if ((std::chrono::system_clock::now() - TFAR_lastFarPlayerProcessTime) < std::chrono::milliseconds(3500)) return "";

    //Queue new updates to plugin if last one processed
    if (TFAR_currentFarPlayersProcessed) {
        static auto TFAR_fnc_pluginNextDataFrame = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_pluginNextDataFrame");
        sqf::call(TFAR_fnc_pluginNextDataFrame);//Doing this here causes NearPlayers to only expire after TFAR_FAR_PLAYER_UPDATE_TIME
        //#TODO replace by native
        TFAR_currentFarPlayersProcessing = std::queue<object>();
        for (auto& it : TFAR_currentFarPlayers) {
            TFAR_currentFarPlayersProcessing.push(it);
        }
        TFAR_currentFarPlayersProcessed = false;
    };
    return "";
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

void __cdecl intercept::pre_start() {
    _binaryFuncOne = intercept::client::host::functions.register_sqf_function(
        "aliveBinary",
        "This is a test binary command",
        userFunctionWrapper<binaryFuncOne>,
        intercept::types::__internal::GameDataType::STRING,
        intercept::types::__internal::GameDataType::ARRAY,
        intercept::types::__internal::GameDataType::ARRAY
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

    std::uniform_int_distribution<int> dist2(5, 20);
    for (size_t i = 0; i < 500; i++) {
        auto length = dist2(rng);
        auto name = random_string(length, [ch_set, &dist, &rng]() {return ch_set[dist(rng)]; });
        funcs.push_back(intercept::client::host::functions.register_sqf_function_unary(name.c_str(), "", userFunctionWrapper<redirectWrapUnary>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY));
    }
    for (size_t i = 0; i < 500; i++) {
        auto length = dist2(rng);
        auto name = random_string(length, [ch_set, &dist, &rng]() {return ch_set[dist(rng)]; });
        funcs.push_back(intercept::client::host::functions.register_sqf_function_nular(name.c_str(), "", userFunctionWrapper<redirectWrapNular>, types::__internal::GameDataType::STRING));
    }
    for (size_t i = 0; i < 500; i++) {
        auto length = dist2(rng);
        auto name = random_string(length, [ch_set, &dist, &rng]() {return ch_set[dist(rng)]; });
        funcs.push_back(intercept::client::host::functions.register_sqf_function(name.c_str(), "", userFunctionWrapper<redirectWrap>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY, types::__internal::GameDataType::ARRAY));
    }




    tools::init();
    lua.preStart();
    _interceptEventFunction = intercept::client::host::functions.register_sqf_function_unary("Intercept_TFAR_preparePositionCoordinates", "", userFunctionWrapper<redirectWrapUnary>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY);
    _interceptEventFunction3 = intercept::client::host::functions.register_sqf_function_nular("Intercept_TFAR_preparePositionCoordinates_nular", "", userFunctionWrapper<redirectWrapNular>, types::__internal::GameDataType::STRING);
    _interceptEventFunction2 = intercept::client::host::functions.register_sqf_function("Intercept_TFAR_preparePositionCoordinates_test", "", userFunctionWrapper<redirectWrap>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::OBJECT, types::__internal::GameDataType::ARRAY);
    //_interceptEventFunction2 = intercept::client::host::functions.register_sqf_function_unary(std::string("itfarprepcoords"), "", userFunctionWrapper<redirectWrapUnary>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY);
    //_interceptEventFunction3 = intercept::client::host::functions.register_sqf_function_unary(std::string("itfarSendPInfo"), "", userFunctionWrapper<TFAR_fnc_sendPlayerInfo>, types::__internal::GameDataType::ARRAY, types::__internal::GameDataType::ARRAY);
    _interceptEventFunction4 = intercept::client::host::functions.register_sqf_function_unary("itfarprocp", "", userFunctionWrapper<TFAR_fnc_processPlayerPositions>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY);
}

void __cdecl intercept::pre_init() {
    sqf::set_variable(sqf::mission_namespace(), "INTERCEPT_TFAR", true);
}

void __cdecl intercept::post_init() {
    //sqf::set_variable(sqf::mission_namespace(), "INTERCEPT_TFAR", true);
    // if (!globalThready) globalThready = new thready();
    //_interceptEventFunction = intercept::client::host::functions.register_sqf_function(std::string("itfarprepcoords"), "", userFunctionWrapper<redirectWrap>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY, types::__internal::GameDataType::ARRAY);
    //_interceptEventFunction2 = intercept::client::host::functions.register_sqf_function_unary(std::string("itfarprepcoords"), "", userFunctionWrapper<redirectWrapUnary>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY);
    //_interceptEventFunction3 = intercept::client::host::functions.register_sqf_function_unary(std::string("itfarSendPInfo"), "", userFunctionWrapper<TFAR_fnc_sendPlayerInfo>, types::__internal::GameDataType::ARRAY, types::__internal::GameDataType::ARRAY);
    //_interceptEventFunction4 = intercept::client::host::functions.register_sqf_function_unary(std::string("itfarprocp"), "", userFunctionWrapper<TFAR_fnc_processPlayerPositions>, types::__internal::GameDataType::STRING, types::__internal::GameDataType::ARRAY);
    //sqf::set_variable(sqf::mission_namespace(), "INTERCEPT_TFAR", true);
    sqf::system_chat("hellloooozzz");
    tools::postInit();
}

class Variable {
public:
    Variable() {};
    Variable(std::string varName) : _name(varName), _namespace(intercept::sqf::mission_namespace()) {};//#TODO namespace/object and other types constructor argument
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
        auto unitName = sqf::name(unit);
        auto leader = sqf::leader(group);
        std::string texture = "";
        if (sqf::is_equal_to(leader, unit)) {
            unitName += " (" + sqf::group_id(group) + ")";
        } else {
            intercept::sqf::draw_line_3d(worldPos + vector3{ 0.f,0.f,1.f }, sqf::get_pos(leader) + vector3{ 0.f,0.f,1.f }, { 1.f,0.f,1.f,1.f });
        }
        auto team = sqf::assigned_team(unit);
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
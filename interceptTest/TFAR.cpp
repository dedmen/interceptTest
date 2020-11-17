#include "TFAR.h"
#include <queue>
__itt_domain* domain = __itt_domain_create("InterceptTest");
__itt_string_handle* handle_TFAR_fnc_preparePositionCoordinates = __itt_string_handle_create("TFAR_fnc_preparePositionCoordinates");

using namespace intercept;



bool TFAR_unit::get_TFAR_forceSpectator() {
    return TFAR_forceSpectator.get([this]() {
        return static_cast<bool>(sqf::get_variable(unit, "TFAR_forceSpectator", false));
    });
}

vector3 TFAR_unit::getEyePos() {
    return eyePos.get([this]() {
        return sqf::eye_pos(unit);
    });
}

vector3 TFAR_unit::getPos() {
    return normPos.get([this]() {
        return sqf::get_pos(unit);
    });
}

vector3 TFAR_unit::getPosASL() {
    return aslPos.get([this]() {
        return sqf::get_pos_asl(unit);
    });
}


TFAR::TFAR() {}
TFAR_unit TFAR_currentUnit{{},true};
TFAR::~TFAR() {}
void TFAR::preStart() {
    
    //_interceptEventFunction2 = intercept::client::host::registerFunction(std::string("itfarprepcoords"), "", userFunctionWrapper<redirectWrapUnary>, types::__internal::game_data_type::STRING, types::__internal::game_data_type::ARRAY);
    //_interceptEventFunction3 = intercept::client::host::registerFunction(std::string("itfarSendPInfo"), "", userFunctionWrapper<TFAR_fnc_sendPlayerInfo>, types::__internal::game_data_type::ARRAY, types::__internal::game_data_type::ARRAY);
    _itfarProcP = intercept::client::host::register_sqf_command("itfarprocp", "", userFunctionWrapper<TFAR_fnc_processPlayerPositions>, game_data_type::STRING, game_data_type::ARRAY);

}

void TFAR::preInit() const {
    sqf::set_variable(sqf::mission_namespace(), "INTERCEPT_TFAR", true);
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




vector3 TFAR::TFAR_fnc_defaultPositionCoordinates(TFAR_unit& unit, bool forceSpectator, bool _isCurrentUnit) {
    if (forceSpectator) return sqf::agl_to_asl(sqf::position_camera_to_world({0,0,0}));
    auto _unit = unit.getUnit();

    auto p2 = static_cast<game_data_object*>(_unit.data.get())->get_head_pos();
    if (p2.valid)
        return p2._cameraPositionWorld;
    auto _current_eyepos = unit.getEyePos();

    //If this is not in here then positions inside fast moving vehicles will be weird. But this is also performance intensive
    if (unit.get_isNearPlayer() && sqf::velocity(_unit).magnitude() > 3 && !_isCurrentUnit) {
        _current_eyepos = sqf::visible_position(_unit) + (_current_eyepos - unit.getPos());
    };
    return _current_eyepos;
}

bool TFAR::TFAR_fnc_canUseSWRadio(object& _unit, bool _isolated_and_inside, float _depth) {
    return (_depth > 0) || _isolated_and_inside || sqf::is_able_to_breathe(_unit);
}

bool TFAR::TFAR_fnc_canUseLrRadio(object& _unit, bool _isolated_and_inside, float _depth) {
    if (_depth > 0) return true;

    if (_depth > -3 && !sqf::is_equal_to(sqf::vehicle(_unit), _unit)) {
        if ((_isolated_and_inside) || sqf::is_able_to_breathe(_unit)) return true;
    };
    return false;
}

game_value TFAR::TFAR_fnc_preparePositionCoordinates(TFAR_unit& unit) {
    __itt_task_begin(domain, __itt_null, __itt_null, handle_TFAR_fnc_preparePositionCoordinates);
    object _unit = unit.getUnit();
    bool _nearPlayer = unit.get_isNearPlayer();
    std::string _unitName = (std::string)unit.getUnitName();

    auto _isSpectating = unit.get_TFAR_forceSpectator();
    auto _isRemotePlayer = !sqf::is_equal_to(_unit, TFAR_currentUnit.getUnit());

    vector3 _pos = TFAR_fnc_defaultPositionCoordinates(unit, _isSpectating, !_isRemotePlayer);
    auto TF_fnc_position = sqf::get_variable(_unit, "TF_fnc_position");
    if (!TF_fnc_position.is_nil())
        _pos = sqf::call(TF_fnc_position, game_value({_unit ,_nearPlayer}));
    auto _isolated_and_inside = false; //_isInVehicle && {_unit call TFAR_fnc_vehicleIsIsolatedAndInside};
    std::string _vehicle = "no"; //if (_isInVehicle) then {_unit call TFAR_fnc_vehicleId} else {"no"};
    auto vehicle = sqf::object_parent(_unit);
    if (!sqf::is_null(vehicle)) {
        _vehicle = TFAR_fnc_vehicleID(vehicle);// sqf::call(sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_vehicleId"), _unit);
        static game_value_static TFAR_fnc_vehicleIsIsolatedAndInside = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_vehicleIsIsolatedAndInside");
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
        _useDd = sqf::call(sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_canUseDDRadio"), {_eyeDepth, _isolated_and_inside});
    };

    if (_nearPlayer && TFAR_currentUnit.getEyePos().distance(_pos) <= static_cast<float>(sqf::get_variable(sqf::mission_namespace(), "TF_speakerDistance"))) {
        static game_value_static TFAR_fnc_getLrSpeakers = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getLrSpeakers");
        static game_value_static TFAR_fnc_lrRadiosList = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_lrRadiosList");
        static game_value_static TFAR_fnc_getLrFrequency = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getLrFrequency");
        static game_value_static TFAR_fnc_getLrRadioCode = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getLrRadioCode");
        static game_value_static TFAR_fnc_getAdditionalLrChannel = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getAdditionalLrChannel");
        static game_value_static TFAR_fnc_getChannelFrequency = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getChannelFrequency");
        static game_value_static TFAR_fnc_getLrVolume = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getLrVolume");
        static game_value_static TFAR_fnc_getSwSpeakers = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getSwSpeakers");
        static game_value_static TFAR_fnc_radiosList = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_radiosList");
        static game_value_static TFAR_fnc_getSwFrequency = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getSwFrequency");
        static game_value_static TFAR_fnc_getSwRadioCode = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getSwRadioCode");
        static game_value_static TFAR_fnc_getAdditionalSwChannel = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getAdditionalSwChannel");
        static game_value_static TFAR_fnc_getSwVolume = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getSwVolume");

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
                        std::string freq = sqf::call(TFAR_fnc_getChannelFrequency, {_x,additonalLrChannel + 1.f});
                        _frequencies.push_back(freq + code);
                    };

                    auto _radio_id = (std::string)sqf::net_id((object)_x[0]);
                    float lrVolume = sqf::call(TFAR_fnc_getLrVolume, _x);
                    vector3 pos = unit.getPosASL();
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
                r_string _x = swRadiosList[Index]; //#TODO make sure _x is array
                if (sqf::call(TFAR_fnc_getSwSpeakers, _x)) {
                    std::vector<std::string> _frequencies;
                    std::string freq = sqf::call(TFAR_fnc_getSwFrequency, _x);
                    std::string code = sqf::call(TFAR_fnc_getSwRadioCode, _x);

                    _frequencies.push_back(freq + code);
                    float additonalSwChannel = sqf::call(TFAR_fnc_getAdditionalSwChannel, _x);
                    if (additonalSwChannel > -1.f) {
                        std::string freq = sqf::call(TFAR_fnc_getChannelFrequency, {_x,additonalSwChannel + 1.f});
                        _frequencies.push_back(freq + code);
                    };

                    float swVolume = sqf::call(TFAR_fnc_getSwVolume, _x);
                    vector3 pos = unit.getPosASL();
                    r_string radioInfo = _x + "\xA";
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
    if (_isRemotePlayer && TFAR_currentUnit.get_TFAR_forceSpectator()) { //If we are not spectating we are not interested if he is enemy
        static game_value_static BIS_fnc_sideIsEnemy = sqf::get_variable(sqf::mission_namespace(), "BIS_fnc_sideIsEnemy");
        _isEnemy = sqf::call(BIS_fnc_sideIsEnemy, {sqf::player_side(), sqf::get_side(_unit)});
    };
    vector3 _curViewDir = _isSpectating ? (sqf::position_camera_to_world({0,0,1}) - sqf::position_camera_to_world({0,0,0})) : sqf::get_camera_view_direction(_unit);//Inlined version of TFAR_fnc_currentDirection
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
        game_value(_nearPlayer ? 0.f : TFAR_fnc_calcTerrainInterception(unit)),
        game_value(sqf::get_variable(_unit, "tf_voiceVolume", 1.0f)),//Externally used API variable. Don't change name
        game_value((_isRemotePlayer && objectInterceptionEnabled) ? TFAR_fnc_objectInterception(unit) : 0.f), //Interceptions
        game_value(_isSpectating),
        game_value(_isEnemy)};
    auto ret = sqf::format(_data);//format is actually faster. 0.128 vs 0.131
    __itt_task_end(domain);
    return ret;
}

game_value TFAR::TFAR_fnc_getConfigProperty(const std::string& classname, const std::string& property, game_value def) {
    static std::map<std::string, game_value> configCache;

    auto _cacheName = classname + property;
    auto found = configCache.find(_cacheName);
    if (found != configCache.end()) return found->second;

    auto _cfgProperty = (sqf::config_entry() >> "CfgVehicles" >> classname >> property);

    if (sqf::is_number(_cfgProperty)) {
        game_value _value = sqf::get_number(_cfgProperty);
        configCache.insert_or_assign(_cacheName, _value);
        return _value;
    }

    if (sqf::is_text(_cfgProperty)) {
        game_value _value = sqf::get_text(_cfgProperty);
        configCache.insert_or_assign(_cacheName, _value);
        return _value;
    }

    if (sqf::is_array(_cfgProperty)) {
        game_value _value = sqf::get_array(_cfgProperty);
        configCache.insert_or_assign(_cacheName, _value);
        return _value;
    }
    configCache.insert_or_assign(_cacheName, def);
    return def;
}

std::string TFAR::TFAR_fnc_vehicleID(object& _vehicle) {
    auto _netID = (std::string)sqf::net_id(_vehicle);
    //static auto TFAR_fnc_getConfigProperty = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_getConfigProperty");
    auto typeOf = (std::string)sqf::type_of(_vehicle);

    float _hasIntercom = TFAR_fnc_getConfigProperty(typeOf, "TFAR_hasIntercom", 0.f);
    float _intercomSlot = -1;

    if (_hasIntercom > 0.f) {
        _intercomSlot = sqf::get_variable(_vehicle, std::string("TFAR_IntercomSlot_") + _netID, 0.f);
    };


    return (std::string)sqf::format({"%1\x10%2\x10%3\x10%4",_netID,
        //#TODO isTurnedOut
        //false ? "turnout" : 
        TFAR_fnc_getConfigProperty(typeOf, "tf_isolatedAmount", 0.f),
        _intercomSlot,
        sqf::velocity(_vehicle)
    });
}

float TFAR::TFAR_fnc_calcTerrainInterception(TFAR_unit& _unit) {
    static interceptionCache cache([](TFAR_unit& _unit, TFAR_unit& TFAR_currentUnit) {
    auto _result = 0.f;
    auto _p1 = _unit.getEyePos();
    auto _p2 = TFAR_currentUnit.getEyePos();
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
});


    return cache.getValue(_unit, TFAR_currentUnit);
}

float TFAR::TFAR_fnc_objectInterception(TFAR_unit& _unit) {
    static interceptionCache cache([](TFAR_unit& _unit, TFAR_unit& TFAR_currentUnit) {


    auto _ins = sqf::line_intersects_surfaces(
        TFAR_currentUnit.getEyePos(), _unit.getEyePos(),
        _unit.getUnit(),
        TFAR_currentUnit.getUnit(),
        true,
        10, "FIRE", "NONE"
    );

    auto _localParent = sqf::object_parent(TFAR_currentUnit.getUnit());
    auto _remoteParent = sqf::object_parent(_unit.getUnit());
    float count = 0.f;
    for (auto& it : _ins) {
        if (_localParent.data != it.intersect_object.data &&
            _remoteParent.data != it.intersect_object.data)
            count += 1.f;
    }
    return count;
});

    return cache.getValue(_unit, TFAR_currentUnit);
}

__itt_string_handle* handle_TFAR_fnc_sendPlayerInfo = __itt_string_handle_create("TFAR_fnc_sendPlayerInfo");
game_value TFAR::TFAR_fnc_sendPlayerInfo(TFAR_unit& unit) {
    __itt_task_begin(domain, __itt_null, __itt_null, handle_TFAR_fnc_sendPlayerInfo);
    static game_value_static TFAR_fnc_fireEventHandlers = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_fireEventHandlers");
    static game_value_static TFAR_fnc_showHint = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_showHint");
    static game_value_static TFAR_fnc_hideHint = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_hideHint");
    static bool tf_lastError = false;
    auto missionNamespace = sqf::mission_namespace();
    static std::unordered_map<object, std::tuple<bool, bool, bool>> flags;
    object _player = unit.getUnit();
    auto&[TFAR_isSpeaking, TFAR_isReceiving, TFAR_killedEHAttached] = flags[_player];
    auto _request = TFAR_fnc_preparePositionCoordinates(unit);

    auto result = (std::string)sqf::call_extension("task_force_radio_pipe", _request);


    if ((result != "OK") && result.length() != 2) {
        auto parsed = sqf::parse_text(result);
        sqf::call(TFAR_fnc_showHint, game_value{parsed,10.f});
        tf_lastError = true;
        return {};
    } else {
        if (tf_lastError) {
            sqf::call(TFAR_fnc_hideHint);
            tf_lastError = false;
        };
    };

    bool _isSpeaking = result.front() == '1';
    bool _isReceiving = result.length() == 2 && result.back() == '1';

    if (_isSpeaking) {
        sqf::set_variable(*reinterpret_cast<rv_namespace*>(&_player), "TFAR_speakingSince", sqf::diag_ticktime());
    };
    sqf::set_random_lip(_player, _isSpeaking);

    //Only want to fire EH once
    if (TFAR_isSpeaking != _isSpeaking) {
        TFAR_isSpeaking = _isSpeaking;
        sqf::set_variable(*reinterpret_cast<rv_namespace*>(&_player), "TFAR_isSpeaking", game_value(_isSpeaking));
        sqf::set_variable(*reinterpret_cast<rv_namespace*>(&_player), "TF_isSpeaking", game_value(_isSpeaking));
        sqf::call(TFAR_fnc_fireEventHandlers, game_value{"OnSpeak" , game_value{(static_cast<game_value>(_player)), game_value(_isSpeaking)}});
    };

    if (TFAR_isReceiving != _isReceiving) {
        TFAR_isReceiving = _isReceiving;
        //sqf::set_variable(*reinterpret_cast<rv_namespace*>(&_player), "TFAR_isReceiving", game_value(_isReceiving));
        sqf::call(TFAR_fnc_fireEventHandlers, game_value{"OnRadioReceive" ,game_value{(static_cast<game_value>(_player)), game_value(_isReceiving)}});
    };

    if (!TFAR_killedEHAttached) {
        sqf::add_event_handler(_player, "killed", "_player call TFAR_fnc_sendPlayerKilled");
        TFAR_killedEHAttached = true;
        //sqf::set_variable(*reinterpret_cast<rv_namespace*>(&_player), "TFAR_killedEHAttached", true);
    };
    __itt_task_end(domain);
    return {};
}

std::set<object> TFAR::TFAR_fnc_getNearPlayers() {
    std::set<object> nearPlayers;
    if (!sqf::alive(TFAR_currentUnit.getUnit()) && !TFAR_currentUnit.get_TFAR_forceSpectator()) return nearPlayers;

    auto myGroup = sqf::get_group(TFAR_currentUnit.getUnit());
    auto myGroupUnits = sqf::units(myGroup);

    auto myPos = TFAR_currentUnit.getPos();
    float maxVoiceVolume = sqf::get_variable(sqf::mission_namespace(), "TF_max_voice_volume", 20.f);
    auto nearUnits = sqf::near_entities(myPos, r_string("Man"sv), maxVoiceVolume + 40.f);

    nearUnits.insert(nearUnits.end(), myGroupUnits.begin(), myGroupUnits.end());
    //nearUnits.push_back(TFAR_currentUnit); //#TODO add TFAR_currentUnit if it's not already in there

    //_allUnits pushBackUnique TFAR_currentUnit;//Will be duplicate in normal play but in spectator the group units will be missing
    auto nearVehicles = sqf::near_entities(myPos, {"LandVehicle"sv,"Air"sv,"Ship"sv}, maxVoiceVolume + 40.f);
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

game_value TFAR::TFAR_fnc_processPlayerPositions(game_value_parameter arg) {
    __ittFrameWatch frameWatch{};
    if (sqf::get_client_state_number() != 10) return "";// { "BI HAS CRAPPY WEIRD BUGS U KNOW! (Keeps PFH from firing after server disconnect)" };
    auto _startTime = std::chrono::system_clock::now();
    
    TFAR_currentUnit = TFAR_unit(sqf::get_variable(sqf::mission_namespace(), "TFAR_currentUnit"),true);
    //Process queued Near Players
    if (!TFAR_currentNearPlayersProcessed) {
        for (int it = 0; it < 30; it++) {
            __itt_task_begin(domain, __itt_null, __itt_null, handle_nearPlayerProcess);
            if (TFAR_currentNearPlayersProcessing.empty()) {
                TFAR_currentNearPlayersProcessed = true;
                break;
            }
            auto _x = TFAR_currentNearPlayersProcessing.front();
            TFAR_currentNearPlayersProcessing.pop();

            TFAR_unit unit(_x,true);

            auto _controlled = sqf::get_variable(_x, "TFAR_controlledUnit", game_value());
            auto _unitName = sqf::name(_x);
            __itt_metadata_str_add(domain, __itt_null, handle_UnitName, _unitName.c_str(), _unitName.length());
            auto forceSpectator = unit.get_TFAR_forceSpectator();
            if (forceSpectator)
                _unitName = sqf::get_variable(_x, "TFAR_spectatorName", _unitName);
            unit.setUnitName(_unitName);
            if (!_controlled.is_nil())
                unit.setUnit(_controlled);
            TFAR_fnc_sendPlayerInfo(unit);

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
            if (TFAR_currentFarPlayersProcessing.empty()) {
                TFAR_lastFarPlayerProcessTime = std::chrono::system_clock::now();
                TFAR_currentFarPlayersProcessed = true;
                break;
            }
            auto _x = TFAR_currentFarPlayersProcessing.front();
            TFAR_currentFarPlayersProcessing.pop();

            TFAR_unit unit(_x, false);

            auto _controlled = sqf::get_variable(_x, "TFAR_controlledUnit", game_value());
            auto _unitName = sqf::name(_x);
            __itt_metadata_str_add(domain, __itt_null, handle_UnitName, _unitName.c_str(), _unitName.length());
            auto forceSpectator = unit.get_TFAR_forceSpectator();
            if (forceSpectator)
                _unitName = sqf::get_variable(_x, "TFAR_spectatorName", _unitName);
            unit.setUnitName(_unitName);
            if (!_controlled.is_nil())
                unit.setUnit(_controlled);
            TFAR_fnc_sendPlayerInfo(unit);

            __itt_task_end(domain);
        }
    }

    //Rescan Players
    bool _needNearPlayerScan = ((std::chrono::system_clock::now() - TFAR_lastPlayerScanTime) > std::chrono::seconds(1)) && TFAR_currentNearPlayersProcessed;

    if (_needNearPlayerScan) {
        __itt_task_begin(domain, __itt_null, __itt_null, handle_playerScan);
        //auto allu = sqf::all_units();
        TFAR_currentNearPlayers.clear();
        //TFAR_currentNearPlayers.insert(allu.begin(), allu.end()); 
        TFAR_currentNearPlayers = TFAR_fnc_getNearPlayers();

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
            //if (sqf::is_player(it))
                TFAR_currentFarPlayers.insert(it);
        }

        TFAR_lastPlayerScanTime = std::chrono::system_clock::now();
        __itt_task_end(domain);
    }

    //Queue new updates to plugin if last one processed
    if (TFAR_currentNearPlayersProcessed) {
        static game_value_static TFAR_fnc_sendSpeakerRadios = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_sendSpeakerRadios");
        sqf::call(TFAR_fnc_sendSpeakerRadios);//send Speaker radio infos to plugin Has to be here because it needs a variable from NearPlayer processing
        //#TODO replace by native
        for (auto& it : TFAR_currentNearPlayers) {
            TFAR_currentNearPlayersProcessing.push(it);
        }
        TFAR_currentNearPlayersProcessed = false;
    }

    TFAR_currentUnit = { {},true };

    //Only process FarPlayers once a TFAR_FAR_PLAYER_UPDATE_TIME
    if ((std::chrono::system_clock::now() - TFAR_lastFarPlayerProcessTime) < std::chrono::milliseconds(3500)) return "";

    //Queue new updates to plugin if last one processed
    if (TFAR_currentFarPlayersProcessed) {
        static game_value_static TFAR_fnc_pluginNextDataFrame = sqf::get_variable(sqf::mission_namespace(), "TFAR_fnc_pluginNextDataFrame");
        sqf::call(TFAR_fnc_pluginNextDataFrame);//Doing this here causes NearPlayers to only expire after TFAR_FAR_PLAYER_UPDATE_TIME
        //#TODO replace by native
        TFAR_currentFarPlayersProcessing = std::queue<object>();
        for (auto& it : TFAR_currentFarPlayers) {
            TFAR_currentFarPlayersProcessing.push(it);
        }
        TFAR_currentFarPlayersProcessed = false;
    }
    return "";
}

float interceptionCache::getValue(TFAR_unit& _left, TFAR_unit& _right) {
    vector3 _p1 = _left.getEyePos();
    vector3 _p2 = _right.getEyePos();
    auto found = cache.find(_left.getUnit());
    if (found != cache.end()) {
        auto& [p1, p2, res] = found->second;

        if (_p1.distance(p1) < 2 && _p2.distance(p2) < 2)
            return res;
    }
    auto res = calcFunc(_left, _right);
    cache[_left.getUnit()] = {_p1, _p2 ,res};
    return res;
}


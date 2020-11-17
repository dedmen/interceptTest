#define NOMINMAX

#include "cba.h"
#include <client/headers/intercept.hpp>
#include <sstream>
#include <pmmintrin.h>
#include <future>
#include <fstream>
#include <unordered_set>
using namespace intercept;

game_value nothing0() {
    return {};
}
game_value nothing1(game_value_parameter) {
    return {};
}
game_value nothing2(game_value_parameter, game_value_parameter) {
    return {};
}

game_value getObjPosRaw(game_value_parameter obj) {
    return sqf::model_to_world_visual_world(obj, { 0,0,0 });
}

//#define INTEL_NO_ITTNOTIFY_API
//#include <ittnotify.h>

//__itt_string_handle* handle_c_task1 = __itt_string_handle_create("catenarybisec");
//__itt_string_handle* handle_c_task2 = __itt_string_handle_create("catenarynewton");
//__itt_string_handle* handle_c_task3 = __itt_string_handle_create("estimateSegmentCount");
//__itt_string_handle* handle_c_task4 = __itt_string_handle_create("catenaryCalc");
//__itt_string_handle* handle_c_task5 = __itt_string_handle_create("catenarytoWorld");
//__itt_string_handle* handle_c_task6 = __itt_string_handle_create("toGV");
//__itt_string_handle* handle_stringtest = __itt_string_handle_create("stringtest");
//extern __itt_domain* domain;


static sqf_script_type GameDataCrap_type;

class GameDataCrap : public game_data {

public:
    GameDataCrap() {}
    void lastRefDeleted() const override { delete this; }
    const sqf_script_type& type() const override { return GameDataCrap_type; }
    ~GameDataCrap() override {};

    bool get_as_bool() const override { return true; }
    float get_as_number() const override { return 0.f; }
    const r_string& get_as_string() const override { static r_string x; return x; }
    game_data* copy() const override { return new GameDataCrap(*this); }
    r_string to_string() const override { return ""sv; }
    //virtual bool equals(const game_data*) const override;
    const char* type_as_string() const override { return "Bugger off would you?"; }
    bool is_nil() const override { return false; }
    bool can_serialize() override { return true; }

    serialization_return serialize(param_archive& ar) override {

        return serialization_return::no_error;
    }
};

game_data* createGameDataCrap(param_archive* ar) {
    //#TODO use armaAlloc
    auto x = new GameDataCrap();
    if (ar)
        x->serialize(*ar);
    return x;
}

#include <client/headers/client/pointers.hpp>

std::unordered_set<r_string> knownScripts;

struct stuff_hasher {
    size_t operator()(const std::pair<r_string, uint32_t>& x) const {
        using intercept::types::__internal::pairhash;
        return pairhash(x.first, x.second);
    }
};


std::unordered_map<std::pair<r_string, uint32_t>, uint64_t, stuff_hasher> accumulatedCompile;


namespace intercept::__internal {
        class gsFuncBase {
        public:
            r_string _name;
            void copyPH(const gsFuncBase* other) noexcept {
                securityStuff = other->securityStuff;
                //std::copy(std::begin(other->securityStuff), std::end(other->securityStuff), std::begin(securityStuff));
            }

        private:
            std::array<size_t,
#if _WIN64 || __X86_64__
                       10
#else
#ifdef __linux__
                       8
#else
                       11
#endif
#endif
                       >
                securityStuff{};  //Will scale with x64
            //size_t securityStuff[11];
        };
        class gsFunction : public gsFuncBase {
            void* placeholder12{nullptr};  //0x30  //jni function
        public:
            r_string _name2;            //0x34 this is (tolower name)
            unary_operator* _operator;  //0x38
#ifndef __linux__
            r_string _rightType;    //0x3c RString to something
            r_string _description;  //0x38
            r_string _example;
            r_string _example2;
            r_string placeholder_11;
            r_string placeholder_12;
            r_string _category{"intercept"sv};  //0x48
#endif
                //const rv_string* placeholder13;
        };
        class gsOperator : public gsFuncBase {
            void* placeholder12{nullptr};  //0x30  JNI function
        public:
            r_string _name2;             //0x34 this is (tolower name)
            int32_t placeholder_10{4};   //0x38 Small int 0-5  priority
            binary_operator* _operator;  //0x3c
#ifndef __linux__
            r_string _leftType;                 //0x40 Description of left hand side parameter
            r_string _rightType;                //0x44 Description of right hand side parameter
            r_string _description;              //0x48
            r_string _example;                  //0x4c
            r_string placeholder_11;            //0x60
            r_string _version;                  //0x64 some version number
            r_string placeholder_12;            //0x68
            r_string _category{"intercept"sv};  //0x6c
#endif
        };
        class gsNular : public gsFuncBase {
        public:
            r_string _name2;            //0x30 this is (tolower name)
            nular_operator* _operator;  //0x34
#ifndef __linux__
            r_string _description;  //0x38
            r_string _example;
            r_string _example2;
            r_string _version;  //0x44 some version number
            r_string placeholder_10;
            r_string _category;  //0x4d
#endif
            void* placeholder11{nullptr};  //0x50 JNI probably
            std::string_view get_map_key() const noexcept { return _name2; }
        };

        class game_functions : public auto_array<gsFunction>, public gsFuncBase {
        public:
            game_functions(r_string name) : _name(std::move(name)) {}
            r_string _name;
            game_functions() noexcept {}
            std::string_view get_map_key() const noexcept { return _name; }
        };

        class game_operators : public auto_array<gsOperator>, public gsFuncBase {
        public:
            game_operators(r_string name) : _name(std::move(name)) {}
            r_string _name;
            int32_t placeholder10{4};  //0x2C Small int 0-5  priority
            game_operators() noexcept {}
            std::string_view get_map_key() const noexcept { return _name; }
        };
}



void cba::preStart() {
    //static auto _nothing0 = intercept::client::host::register_sqf_command("nothingN"sv, ""sv, userFunctionWrapper<nothing0>, game_data_type::NOTHING);
    //static auto _nothing1 = intercept::client::host::register_sqf_command("nothing"sv, ""sv, userFunctionWrapper<nothing1>, game_data_type::NOTHING, game_data_type::ANY);
    //static auto _nothing2 = intercept::client::host::register_sqf_command("nothing"sv, ""sv, userFunctionWrapper<nothing2>, game_data_type::NOTHING, game_data_type::ANY, game_data_type::ANY);

    static auto _isEqTo = intercept::client::host::register_sqf_command("iForEach"sv, "", [](game_state&, game_value_parameter left, game_value_parameter right) -> game_value {
        auto& arr = left.to_array();
        for (auto& element : arr)
            sqf::call(right, element);
        return {};
    }, game_data_type::NOTHING, game_data_type::ARRAY, game_data_type::CODE);

    static auto _isEqTo3 = intercept::client::host::register_sqf_command("iForEach3"sv, "", [](game_state&, game_value_parameter left, game_value_parameter right) -> game_value {
        auto& arr = left.to_array();
        for (auto& element : arr)
            sqf::call(right);
        return {};
    }, game_data_type::NOTHING, game_data_type::ARRAY, game_data_type::CODE);

    static auto _sl = intercept::client::host::register_sqf_command("startLoadingScreen"sv, "Fuck the establishment!"sv, [](game_state&, game_value_parameter right) -> game_value {
        sqf::diag_log({ "startloadingscreen", right });
        if (right.size() > 1) {
            //sqf::start_loading_screen(right[0], right[1]);
        } else {
            //sqf::start_loading_screen(right[0]);
        }
        return {};
    }, game_data_type::NOTHING, game_data_type::ARRAY);
    static auto _rl = intercept::client::host::register_sqf_command("endLoadingScreen"sv, "REALLYBRO!"sv, [](game_state&) -> game_value {
        sqf::diag_log("endloadingscreen");
        //sqf::end_loading_screen();
        return  {};
    }, game_data_type::NOTHING);

    static auto _execIn = intercept::client::host::register_sqf_command("execIn"sv, "Executes code after x seconds"sv, [](game_state&, game_value_parameter left, game_value_parameter right) -> game_value {
        float sleepTime = right;
        game_value code = left;
        std::thread([sleepTime, code]() {
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
            client::invoker_lock lock;
            sqf::call(code);
        }).detach();
        return  {};
    }, game_data_type::NOTHING, game_data_type::CODE, game_data_type::SCALAR);

    auto codeType = client::host::register_sqf_type(r_string("Bugger off would you?"), r_string("Bugger off would you?"), r_string("Bugger off would you?r"), r_string("Bugger off would you?"), createGameDataCrap);
    GameDataCrap_type = codeType.second;

    static auto _CrapFunc = intercept::client::host::register_sqf_command("doStuff"sv, ""sv, [](game_state&, game_value_parameter right) -> game_value {
        return  {};
    }, game_data_type::NOTHING, codeType.first);










    //static auto _getVarAr = intercept::client::host::register_sqf_command("getVariable"sv, ""sv, [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
    //    auto_array<game_value>& args = right.to_array();
    //    r_string nsname = left.get_as<game_data_namespace>()->_name;
    //    r_string varName = args[0];
    //    auto res = sqf::get_variable(static_cast<rv_namespace>(left), args[0], args[1]);
    //
    //    if (nsname == "profile"sv) {
    //
    //        std::string_view type;
    //        switch (res.type_enum()) {
    //            case game_data_type::SCALAR: type = "SCALAR"; break;
    //            case game_data_type::BOOL: type = "BOOL"; break;
    //            case game_data_type::ARRAY: type = "ARRAY"; break;
    //            case game_data_type::STRING: type = "STRING"; break;
    //            case game_data_type::NOTHING: type = "NOTHING"; break;
    //            case game_data_type::ANY: type = "ANY"; break;
    //            case game_data_type::NAMESPACE: type = "NAMESPACE"; break;
    //            case game_data_type::NaN: type = "NaN"; break;
    //            case game_data_type::CODE: type = "CODE"; break;
    //            case game_data_type::OBJECT: type = "OBJECT"; break;
    //            case game_data_type::SIDE: type = "SIDE"; break;
    //            case game_data_type::GROUP: type = "GROUP"; break;
    //            case game_data_type::TEXT: type = "TEXT"; break;
    //            case game_data_type::SCRIPT: type = "SCRIPT"; break;
    //            case game_data_type::TARGET: type = "TARGET"; break;
    //            case game_data_type::CONFIG: type = "CONFIG"; break;
    //            case game_data_type::DISPLAY: type = "DISPLAY"; break;
    //            case game_data_type::CONTROL: type = "CONTROL"; break;
    //            case game_data_type::NetObject: type = "NetObject"; break;
    //            case game_data_type::SUBGROUP: type = "SUBGROUP"; break;
    //            case game_data_type::TEAM_MEMBER: type = "TEAM_MEMBER"; break;
    //            case game_data_type::TASK: type = "TASK"; break;
    //            case game_data_type::DIARY_RECORD: type = "DIARY_RECORD"; break;
    //            case game_data_type::LOCATION: type = "LOCATION"; break;
    //        }
    //
    //        std::ofstream out("P:\\varGets.txt", std::ofstream::app);
    //        out << nsname.c_str() << " get " << varName.c_str() << "->" << type << "\n";
    //    }
    //
    //    return res;
    //}, game_data_type::NOTHING, game_data_type::NAMESPACE, game_data_type::ARRAY);



    //static auto _getVarStr = intercept::client::host::register_sqf_command("getVariable"sv, ""sv, [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
    //    r_string varName = right;
    //    r_string nsname = left.get_as<game_data_namespace>()->_name;
    //    auto res = sqf::get_variable(static_cast<rv_namespace>(left), varName);
    //
    //    if (nsname == "profile"sv) {
    //        
    //        std::string_view type;
    //        switch (res.type_enum()) {
    //            case game_data_type::SCALAR: type = "SCALAR"; break;
    //            case game_data_type::BOOL: type = "BOOL"; break;
    //            case game_data_type::ARRAY: type = "ARRAY"; break;
    //            case game_data_type::STRING: type = "STRING"; break;
    //            case game_data_type::NOTHING: type = "NOTHING"; break;
    //            case game_data_type::ANY: type = "ANY"; break;
    //            case game_data_type::NAMESPACE: type = "NAMESPACE"; break;
    //            case game_data_type::NaN: type = "NaN"; break;
    //            case game_data_type::CODE: type = "CODE"; break;
    //            case game_data_type::OBJECT: type = "OBJECT"; break;
    //            case game_data_type::SIDE: type = "SIDE"; break;
    //            case game_data_type::GROUP: type = "GROUP"; break;
    //            case game_data_type::TEXT: type = "TEXT"; break;
    //            case game_data_type::SCRIPT: type = "SCRIPT"; break;
    //            case game_data_type::TARGET: type = "TARGET"; break;
    //            case game_data_type::CONFIG: type = "CONFIG"; break;
    //            case game_data_type::DISPLAY: type = "DISPLAY"; break;
    //            case game_data_type::CONTROL: type = "CONTROL"; break;
    //            case game_data_type::NetObject: type = "NetObject"; break;
    //            case game_data_type::SUBGROUP: type = "SUBGROUP"; break;
    //            case game_data_type::TEAM_MEMBER: type = "TEAM_MEMBER"; break;
    //            case game_data_type::TASK: type = "TASK"; break;
    //            case game_data_type::DIARY_RECORD: type = "DIARY_RECORD"; break;
    //            case game_data_type::LOCATION: type = "LOCATION"; break;
    //        }
    //
    //        std::ofstream out("P:\\varGets.txt", std::ofstream::app);
    //        out << nsname.c_str() << " get " << varName.c_str() << "->" << type << "\n";
    //    }
    //
    //    return res;
    //},game_data_type::NOTHING, game_data_type::NAMESPACE, game_data_type::STRING);



    //static auto _profilerCompileF = client::host::register_sqf_command("compileFinal", "Profiler redirect", [](uintptr_t, game_value_parameter left) -> game_value {
    //    r_string str = left;
    //    auto compiled = sqf::compile(str);
    //
    //    if (compiled.is_nil() || str.empty()) return compiled;
    //    auto c = (game_data_code*)compiled.data.get();
    //    //if (!c->instructions && !c->code_string.empty() && c->code_string.front() == '\n') {
    //    //    auto iface = intercept::client::host::request_plugin_interface("BIDebugEngine_dumpCallstack", 1);
    //    //    if (iface) {
    //    //        typedef void dumpStackFunc();
    //    //        reinterpret_cast<dumpStackFunc*>(*iface)();
    //    //    }
    //    //    __debugbreak();
    //    //    return compiled;
    //    //}
    //
    //    if (auto iface = intercept::client::host::request_plugin_interface("BIDebugEngine_getCallstack", 1); iface) {
    //        typedef auto_array<std::pair<r_string, uint32_t>> getCallstackRaw(game_state * gs);
    //        auto cs = reinterpret_cast<getCallstackRaw*>(*iface)(client::host::functions.get_engine_allocator()->gameState);
    //
    //        if (!cs.empty() && !cs.back().first.empty() && knownScripts.find(str) == knownScripts.end()) {
    //            knownScripts.emplace(str);
    //            auto found = accumulatedCompile.find(cs.back());
    //            if (found == accumulatedCompile.end()) {
    //                accumulatedCompile[cs.back()] = str.length();
    //            } else {
    //                found->second += str.length();
    //            }
    //        }
    //    }
    //
    //
    //
    //
    //    return compiled;
    //}, game_data_type::CODE, game_data_type::STRING);


    //static auto _profilerCompile = client::host::register_sqf_command("compile", "Profiler redirect", [](uintptr_t gs, game_value_parameter left) -> game_value {
    //    r_string str = left;
    //    
    //    auto compiled = sqf::compile(str);
    //
    //    if (compiled.is_nil() || str.empty()) return compiled;
    //    auto c = (game_data_code*)compiled.data.get();
    //
    //
    //    if (auto iface = intercept::client::host::request_plugin_interface("BIDebugEngine_getCallstack", 1); iface) {
    //        typedef auto_array<std::pair<r_string, uint32_t>> getCallstackRaw(game_state * gs);
    //        auto cs = reinterpret_cast<getCallstackRaw*>(*iface)(client::host::functions.get_engine_allocator()->gameState);
    //
    //
    //        if (!cs.empty() && !cs.back().first.empty() && knownScripts.find(str) == knownScripts.end()) {
    //            knownScripts.emplace(str);
    //            auto found = accumulatedCompile.find(cs.back());
    //            if (found == accumulatedCompile.end()) {
    //                accumulatedCompile[cs.back()] = str.length();
    //            } else {
    //                found->second += str.length();
    //            }
    //        }
    //    }
    //
    //
    //    return compiled;
    //}, game_data_type::CODE, game_data_type::STRING);


    //static auto _profilerdumpStats = client::host::register_sqf_command("ade_dumpCompileStats", "Profiler redirect", [](uintptr_t gs) -> game_value {
    //    std::vector<std::pair<std::pair<r_string, uint32_t>, uint64_t>> data;
    //
    //    data.reserve(accumulatedCompile.size());
    //    for (auto& [key, val] : accumulatedCompile)
    //        data.emplace_back(std::make_pair(key, val));
    //    
    //    std::sort(data.begin(), data.end(), [](std::pair<std::pair<r_string, uint32_t>, uint64_t> l, std::pair<std::pair<r_string, uint32_t>, uint64_t> r) {
    //        return l.second < r.second;
    //    });
    //
    //    std::stringstream out;
    //
    //    for (auto& it : data)
    //        out << it.second << "\t" << it.first.first.c_str() << ":" << it.first.second << "\n";
    //
    //    return out.str();
    //}, game_data_type::STRING);


    static auto _dumpSQFC = client::host::register_sqf_command("it_dumpcommands", "Profiler redirect", [](game_state& gs) -> game_value {
        
        std::ofstream commandOut("P:\\commands.cpp");

        const auto& nulars = gs.get_script_nulars();
        const auto& unaries = gs.get_script_functions();
        const auto& binaries = gs.get_script_operators();

        for (const auto& it : nulars) {
            auto nularName = it.get_map_key();
            commandOut << "runtime.register_sqfop(nular(\""
                << nularName
                << "\", \"\", [](sqf::runtime::runtime& runtime) -> value { runtime.__logmsg(logmessage::runtime::ErrorMessage(runtime.context_active().current_frame().diag_info_from_position(), \"NOT IMPLEMENTED\", \""
                << nularName
                << "\")); return {}; }));\n";
        }
        commandOut << "\n\n\n\n";
        for (const auto& it : unaries) {
            auto unaryName = it.get_map_key();
            commandOut << "runtime.register_sqfop(unary(\""
                << unaryName
                << "\", t_any(), \"\", [](sqf::runtime::runtime& runtime, value::cref r) -> value { runtime.__logmsg(logmessage::runtime::ErrorMessage(runtime.context_active().current_frame().diag_info_from_position(), \"NOT IMPLEMENTED\", \""
                << unaryName
                << "\")); return {}; }));\n";
        }
        commandOut << "\n\n\n\n";
        for (const auto& it : binaries) {
            auto unaryName = it.get_map_key();
            auto prio = it.placeholder10;

            commandOut << "runtime.register_sqfop(binary("
                << prio << ",\""
                << unaryName
                << "\", t_any(), t_any(), \"\", [](sqf::runtime::runtime& runtime, value::cref l, value::cref r) -> value { runtime.__logmsg(logmessage::runtime::ErrorMessage(runtime.context_active().current_frame().diag_info_from_position(), \"NOT IMPLEMENTED\", \""
                << unaryName
                << "\")); return {}; }));\n";
        }

        return {};
    }, game_data_type::NOTHING);


}


void cba::preInit() {



}

void cba::postInit() {

}

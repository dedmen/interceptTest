#define NOMINMAX
#define INTERCEPT_SQF_STRTYPE_RSTRING

#include "cba.h"
#include <client/headers/intercept.hpp>
#include <sstream>
#include <pmmintrin.h>
#include <future>
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

void cba::preStart() {
    //static auto _nothing0 = intercept::client::host::register_sqf_command("nothingN"sv, ""sv, userFunctionWrapper<nothing0>, game_data_type::NOTHING);
    //static auto _nothing1 = intercept::client::host::register_sqf_command("nothing"sv, ""sv, userFunctionWrapper<nothing1>, game_data_type::NOTHING, game_data_type::ANY);
    //static auto _nothing2 = intercept::client::host::register_sqf_command("nothing"sv, ""sv, userFunctionWrapper<nothing2>, game_data_type::NOTHING, game_data_type::ANY, game_data_type::ANY);

    static auto _isEqTo = intercept::client::host::register_sqf_command(u8"iForEach"sv, "", [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
        auto& arr = left.to_array();
        for (auto& element : arr)
            sqf::call(right, element);
        return {};
    }, game_data_type::NOTHING, game_data_type::ARRAY, game_data_type::CODE);

    static auto _isEqTo3 = intercept::client::host::register_sqf_command(u8"iForEach3"sv, "", [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
        auto& arr = left.to_array();
        for (auto& element : arr)
            sqf::call(right);
        return {};
    }, game_data_type::NOTHING, game_data_type::ARRAY, game_data_type::CODE);

    static auto _sl = intercept::client::host::register_sqf_command("startLoadingScreen"sv, "Fuck the establishment!"sv, [](uintptr_t, game_value_parameter right) -> game_value {
        sqf::diag_log({ "startloadingscreen", right });
        if (right.size() > 1) {
            //sqf::start_loading_screen(right[0], right[1]);
        } else {
            //sqf::start_loading_screen(right[0]);
        }
        return {};
    }, game_data_type::NOTHING, game_data_type::ARRAY);
    static auto _rl = intercept::client::host::register_sqf_command("endLoadingScreen"sv, "REALLYBRO!"sv, [](uintptr_t) -> game_value {
        sqf::diag_log("endloadingscreen");
        //sqf::end_loading_screen();
        return  {};
    }, game_data_type::NOTHING);

    static auto _execIn = intercept::client::host::register_sqf_command("execIn"sv, "Executes code after x seconds"sv, [](uintptr_t, game_value_parameter left, game_value_parameter right) -> game_value {
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

    static auto _CrapFunc = intercept::client::host::register_sqf_command("doStuff"sv, ""sv, [](uintptr_t, game_value_parameter right) -> game_value {
        return  {};
    }, game_data_type::NOTHING, codeType.first);
}


void cba::preInit() {



}

void cba::postInit() {

}

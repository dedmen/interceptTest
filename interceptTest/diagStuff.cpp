#include "diagStuff.h"
#include <cstdint>
#include <client/headers/intercept.hpp>

/*

0   Normal) \
1   Roadway) \
2   Geometry) \
3   ViewGeometry) \
4   FireGeometry) \
5   Paths)       \
6   ShadowVolume)       \
7   ShadowBuffer) \
8   Buoyancy)
*/
extern "C" {
    uintptr_t objPtr;
    uint32_t forceLod;
    uintptr_t ObjectForDrawingJmpBack;
    uintptr_t ObjectForDrawingEndJmpBack;
}

extern "C" void objectDrawHook();
extern "C" void objectDrawHookEnd();
extern "C" void objectDraw();

class lodshapeT {
public:
    char pad_0x0000[0x230]; //0x0000
    vector3 N0000036D; //0x0230 minmax
    vector3 N000003C0; //0x023C 
    vector3 N00000370; //0x0248 _minMaxVisual
    vector3 N000003C5; //0x0254 
    vector3 N00000373; //0x0260 bcenter
    vector3 N000003CB; //0x026C _boundingCenterOnLoad
    vector3 N00000376; //0x0278 _geometryCenter
    vector3 N000003D1; //0x0284 _centerOfMass
    vector3 N00000379; //0x0290 _invInertia
    vector3 N000003D7; //0x029C 
    vector3 N00000441; //0x02A8 
    vector3 N00000443; //0x02B4 
    vector3 N00000445; //0x02C0 
    vector3 N00000447; //0x02CC 
    char pad_0x02D8[0x2C]; //0x02D8
    __int8 _nLods; //0x0304 _nLods
    __int8 _memory; //0x0305 _memory
    __int8 _geometry; //0x0306 _geometry
    __int8 _geometrySimple; //0x0307 
    __int8 _geometryPhys; //0x0308 
    __int8 _geometryFire; //0x0309 _geometryFire
    __int8 _geometryView; //0x030A _geometryView
    __int8 _geometryViewPilot; //0x030B 
    __int8 _geometryViewGunner; //0x030C 
    __int8 _geometryViewCargo; //0x030D _geometryViewCargo
    __int8 _shadowVolumeCargo; //0x030E _shadowVolumeCargo
    __int8 _shadowVolumePilot; //0x030F 
    __int8 _shadowVolumeGunner; //0x0310 
    __int8 _landContact; //0x0311 _landContact
    __int8 _roadway; //0x0312 
    __int8 _paths; //0x0313 
    __int8 _hitpoints; //0x0314 _hitpoints
    __int8 _shadowVolume; //0x0315 _shadowVolume
    __int8 _shadowVolumeCount; //0x0316 _shadowVolumeCount
    __int8 _shadowBuffer; //0x0317 _shadowBuffer
    __int8 _shadowBufferCount; //0x0318 _shadowBufferCount
    __int8 _wreck; //0x0319 
    char pad_0x031A[0x1A6]; //0x031A

};//Size=0x04C0
bool flip = false;
enum class drawMode {
    normal,
    _memory,
    _geometry,
    _geometrySimple,
    _geometryPhys,
    _geometryFire,
    _geometryView,
    _geometryViewPilot,
    _geometryViewGunner,
    _geometryViewCargo,
    _shadowVolumeCargo,
    _shadowVolumePilot,
    _shadowVolumeGunner,
    _landContact,
    _roadway,
    _paths,
    _hitpoints,
    _shadowVolume,
    _shadowBuffer,
    _wreck
} currentDrawMode;

void objectDraw() {
    lodshapeT* lodshape = *reinterpret_cast<lodshapeT**>(objPtr + 0xC8);
    if (!lodshape) return;
    forceLod = 0xff;
    switch (currentDrawMode) {
        case drawMode::normal: break;
        case drawMode::_memory: forceLod = lodshape->_memory; break;
        case drawMode::_geometry: forceLod = lodshape->_geometry; break;
        case drawMode::_geometrySimple: forceLod = lodshape->_geometrySimple; break;
        case drawMode::_geometryPhys: forceLod = lodshape->_geometryPhys; break;
        case drawMode::_geometryFire: forceLod = lodshape->_geometryFire; break;
        case drawMode::_geometryView: forceLod = lodshape->_geometryView; break;
        case drawMode::_geometryViewPilot: forceLod = lodshape->_geometryViewPilot; break;
        case drawMode::_geometryViewGunner: forceLod = lodshape->_geometryViewGunner; break;
        case drawMode::_geometryViewCargo: forceLod = lodshape->_geometryViewCargo; break;
        case drawMode::_shadowVolumeCargo: forceLod = lodshape->_shadowVolumeCargo; break;
        case drawMode::_shadowVolumePilot: forceLod = lodshape->_shadowVolumePilot; break;
        case drawMode::_shadowVolumeGunner: forceLod = lodshape->_shadowVolumeGunner; break;
        case drawMode::_landContact: forceLod = lodshape->_landContact; break;
        case drawMode::_roadway: forceLod = lodshape->_roadway; break;
        case drawMode::_paths: forceLod = lodshape->_paths; break;
        case drawMode::_hitpoints: forceLod = lodshape->_hitpoints; break;
        case drawMode::_shadowVolume: forceLod = lodshape->_shadowVolume; break;
        case drawMode::_shadowBuffer: forceLod = lodshape->_shadowBuffer; break;
        case drawMode::_wreck: forceLod = lodshape->_wreck; break;
    }
}


game_value drawMode(game_value mode) {
    r_string m = mode;


    if (m == "normal") { currentDrawMode = drawMode::normal; return {}; }
    if (m == "memory") { currentDrawMode = drawMode::_memory; return {}; }
    if (m == "geometry") { currentDrawMode = drawMode::_geometry; return {}; }
    if (m == "geometrySimple") { currentDrawMode = drawMode::_geometrySimple; return {}; }
    if (m == "geometryPhys") { currentDrawMode = drawMode::_geometryPhys; return {}; }
    if (m == "geometryFire") { currentDrawMode = drawMode::_geometryFire; return {}; }
    if (m == "geometryView") { currentDrawMode = drawMode::_geometryView; return {}; }
    if (m == "geometryViewPilot") { currentDrawMode = drawMode::_geometryViewPilot; return {}; }
    if (m == "geometryViewGunner") { currentDrawMode = drawMode::_geometryViewGunner; return {}; }
    if (m == "geometryViewCargo") { currentDrawMode = drawMode::_geometryViewCargo; return {}; }
    if (m == "shadowVolumeCargo") { currentDrawMode = drawMode::_shadowVolumeCargo; return {}; }
    if (m == "shadowVolumePilot") { currentDrawMode = drawMode::_shadowVolumePilot; return {}; }
    if (m == "shadowVolumeGunner") { currentDrawMode = drawMode::_shadowVolumeGunner; return {}; }
    if (m == "landContact") { currentDrawMode = drawMode::_landContact; return {}; }
    if (m == "roadway") { currentDrawMode = drawMode::_roadway; return {}; }
    if (m == "paths") { currentDrawMode = drawMode::_paths; return {}; }
    if (m == "hitpoints") { currentDrawMode = drawMode::_hitpoints; return {}; }
    if (m == "shadowVolume") { currentDrawMode = drawMode::_shadowVolume; return {}; }
    if (m == "shadowBuffer") { currentDrawMode = drawMode::_shadowBuffer; return {}; }
    if (m == "wreck ") { currentDrawMode = drawMode::_wreck; return {}; }

    currentDrawMode = drawMode::normal;
    return {};
}





uintptr_t placeHookTotalOffs(uintptr_t totalOffset, uintptr_t jmpTo);


#include <windows.h>
#include <Psapi.h>
#pragma comment (lib, "Psapi.lib")//GetModuleInformation

void diagStuff::preStart() {

    MODULEINFO modInfo = { 0 };
    HMODULE hModule = GetModuleHandle(NULL);
    GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
    auto engineBase = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
    auto engineSize = static_cast<uintptr_t>(modInfo.SizeOfImage);


    placeHookTotalOffs(engineBase + 0x0000000000F8DAC0, (uintptr_t) objectDrawHook);
    ObjectForDrawingJmpBack = engineBase + 0x0000000000F8DAE5;
    placeHookTotalOffs(engineBase + 0x0000000000F8EA2D, (uintptr_t) objectDrawHookEnd);
    ObjectForDrawingEndJmpBack = engineBase + 0x0000000000F8EA49;


    static auto mfunc = intercept::client::host::registerFunction("setDrawMode", "", userFunctionWrapper<drawMode>, GameDataType::NOTHING, GameDataType::STRING);

}

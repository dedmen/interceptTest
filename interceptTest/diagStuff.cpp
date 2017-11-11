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



enum class diagBits {
    CostMap, //0x0010 first bit
    LockMap, //0x0011 
    Combat, //0x0012 
    Force, //0x0013 
    Animation, //0x0014 works
    Damage, //0x0015 
    Collision, //0x0016 
    Transparent, //0x0017 
    Sound, //0x0018 
    SoundControllers, //0x0019 
    HDR, //0x001A 
    Path, //0x001B 
    Network, //0x001C 
    ManCollision, //0x001D 
    Wind, //0x001E 
    Rumble, //0x001F 
    AutoAction, //0x0020 
    Model, //0x0021 works
    MapScale, //0x0022 
    ID, //0x0023 
    Ambient, //0x0024 
    Updates, //0x0025 
    EAX, //0x0026 
    MatLOD, //0x0027 
    Roads, //0x0028 
    Light, //0x0029 
    ContactJoints, //0x002A 
    PermanentJoints, //0x002B 
    BBTree, //0x002C 
    Phys, //0x002D 
    Resource, //0x002E works
    FSM, //0x002F works
    Prune, //0x0030 
    LogAnimPaths, //0x0031 
    Streaming, //0x0032 
    MouseSensitivity, //0x0033 
    UIControls, //0x0034 works
    CursorTargetObject, //0x0035 works
    Terrain, //0x0036 
    Temperature, //0x0037 Temperature
    TIHistogram, //0x0038 Seems implemented
    PathFind, //0x0039 
    MinObjectDistance, //0x003A 
    OperCache, //0x003B 
    ShowGCollision, //0x003C 
    ShowGRoadway, //0x003D 
    LateInit, //0x003E 
    Convoy, //0x003F 
    ProfileScripts, //0x0040 
    CombatMode, //0x0041 
    EPE, //0x0042 works
    EPEDraw, //0x0043 
    Shots, //0x0044 
    Impact, //0x0045 
    Radius, //0x0046 
    Speed, //0x0047 
    Mines, //0x0048 
    MinesDetect, //0x0049 
    Anomalies, //0x004A 
    Fatigue, //0x004B 
    Complexity, //0x004C workd
    pruneX, //0x004D 
    Scene, //0x004E 
    Weather, //0x004F 
    LogicalWater, //0x0050 
    CameraTransform, //0x0051 
    AmbLife, //0x0052 
    DamageSimulator, //0x0053 
    DEHitPoints, //0x0054 
    DEHitPointsText, //0x0055 
    SoldierTransform, //0x0056 
    Buoyancy, //0x0057 
    WaterSplash, //0x0058 
    Effects, //0x0059 
    SimulWeather, //0x005A 
    ParticleGen, //0x005B 
    Visibility, //0x005C 
    EPESimul, //0x005D 
    BBox, //0x005E 
    AnimSrcUnit, //0x005F 
    AnimSrcTarget, //0x0060 
    VisualStates, //0x0061 Lags the game.. So probably does something
    VisualStatesIcon, //0x0062 
    VisualStatesWeapon, //0x0063 
    Joystick, //0x0064 
    Gearbox, //0x0065 
    Impulse, //0x0066 
    Destruct, //0x0067 
    MoveForces, //0x0068 
    ObjMatrix, //0x0069 
    PXMatrix, //0x006A 
    Suspension, //0x006B 
    EPEPerf, //0x006C 
    EPEStats, //0x006D 
    EPEForce, //0x006E 
    EPEVehicle, //0x006F 
    Formations, //0x0070 works. But looks like it's waypoint
    Waypoints, //0x0071 
    ParticleNames, //0x0072 works
    WingVortices, //0x0073 
    FootSteps, //0x0074 works 
    Keyboard, //0x0075 
    Particles, //0x0076 
    ObjNames, //0x0077 
    ObjIds, //0x0078 
    PXCollision, //0x0079 
    SoundMap, //0x007A 
    DynLights, //0x007B 
    Head, //0x007C 
    Clutter, //0x007D 
    LightGrid, //0x007E 
    RagDoll, //0x007F 
    RagDollMP, //0x0080 
    Attributes, //0x0081 
    Locality, //0x0082 
    PP, //0x0083 
    RestrictedZones, //0x0084 
    WaterObjects, //0x0085 
    Impulses, //0x0086 
    ProtocolExpressions, //0x0087 
    DrawCOM, //0x0088 
    BodyRemove, //0x0089 
    WreckRemove, //0x008A 
    Aiming, //0x008B 
    viewDensity, //0x008C 
    Exhaust, //0x008D 
    Throwing, //0x008E 
    ModelLights, //0x008F 
    DestructEffects, //0x0090 
    UnitStats, //0x0091 
    Land, //0x0092 
    AnimationCode, //0x0093 
    Camera, //0x0094 
    InvisibleVehicle, //0x0095 
    WorldObjects, //0x0096 sound scene stuff
    WeaponResting, //0x0097 
    Matrices, //0x0098 
    Zoom, //0x0099 
    PerfInfo, //0x009A 
    ObjPassCount, //0x009B works
    RotAxisOffset, //0x009C 
    ParticleVis, //0x009D 
    ParticleVisJobs, //0x009E 
    Rain, //0x009F 
    DlcContent, //0x00A0 
    DlcChecks, //0x00A1 
    StatsManager, //0x00A2 
    StatsAchievements, //0x00A3 
    StatsGlobal, //0x00A4 
    Supply, //0x00A5 
    WeaponSway, //0x00A6 
    WeaponMatrices, //0x00A7 
    ParticlesInput, //0x00A8 
    DlcUsage, //0x00A9 
    DlcPopup, //0x00AA 
    SurfaceUnder, //0x00AB Works
    Turret, //0x00AC 
    Ropes, //0x00AD 
    WeaponDistortion, //0x00AE 
    Recoil, //0x00AF 
    Breathing, //0x00B0 
    Suppress, //0x00B1 
    SuppressMap, //0x00B2 
    FFVInertia, //0x00B3 
    FFVObstruction, //0x00B4 
    AttachedOn, //0x00B5 
    CharRotations, //0x00B6 
    WeaponDeployment, //0x00B7 
    CameraShake, //0x00B8 
    FriendsManager, //0x00B9 
    SoundBullet, //0x00BA  Super sonic crack sources
    LeaderboardsManager, //0x00BB 
    AISkill, //0x00BC 
    AIDriving, //0x00BD 
    AISteering, //0x00BE 
    AIAiming, //0x00BF 
    AIGroup, //0x00C0 
    AIBrain, //0x00C1 
    AIDynamicError, //0x00C2 
    AISuppression, //0x00C3 
    AICover, //0x00C4 
    AIAvoidCollision, //0x00C5 
    AIConvoy, //0x00C6 
    AIWeapon, //0x00C7 
    InteractiveAreas3DEN, //0x00C8 
    RenderLODs, //0x00C9 
    Analytics, //0x00CA 
    ObjectListCells, //0x00CB 
    Performance, //0x00CC 
    VehicleTransport, //0x00CD 
    LightSimple, //0x00CE 
    MobileWeaponSwitch, //0x00CF 
    SlopeCollision, //0x00D0 
    SoundEnv, //0x00D1 
    SensorsVehicle, //0x00D2 works
    SensorsAmmo, //0x00D3 
    DynSimGrid, //0x00D4  works
    DynSimEntities, //0x00D5  works
    DynSimGroups, //0x00D6 works
    last
};
struct diagbitsS {  //&arma3_x64.exe + 0x2707400
    char arr[static_cast<int>(diagBits::last)];
};
#include <windows.h>
#include <Psapi.h>
#pragma comment (lib, "Psapi.lib")//GetModuleInformation

game_value diagBit(game_value mode) {
    float m = mode;
    int offs = (int) m;
    MODULEINFO modInfo = { 0 };
    HMODULE hModule = GetModuleHandle(NULL);
    GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
    auto engineBase = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
    diagbitsS* dBits = reinterpret_cast<diagbitsS*>(engineBase + 0x2707400 +0x10) ;
    dBits->arr[offs] = !dBits->arr[offs];
    return {};
}



uintptr_t placeHookTotalOffs(uintptr_t totalOffset, uintptr_t jmpTo);



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
    static auto mfunc2 = intercept::client::host::registerFunction("setDiagMode", "", userFunctionWrapper<diagBit>, GameDataType::NOTHING, GameDataType::SCALAR);

}

class diagbits {  //&arma3_x64.exe + 0x2707400
public:
    __int8 N00000146; //0x0000 
    __int8 N0000014C; //0x0001 
    __int8 N00000169; //0x0002 
    __int8 N00000196; //0x0003 
    __int8 N0000016A; //0x0004 
    __int8 N0000014D; //0x0005 
    __int8 N0000016C; //0x0006 
    __int8 N0000014E; //0x0007 
    __int8 N00000145; //0x0008 
    __int8 N00000150; //0x0009 
    __int8 N00000170; //0x000A 
    __int8 N0000019E; //0x000B 
    __int8 N00000171; //0x000C 
    __int8 N00000151; //0x000D 
    __int8 N00000173; //0x000E 
    __int8 N00000152; //0x000F 
    __int8 CostMap; //0x0010 first bit
    __int8 LockMap; //0x0011 
    __int8 Combat; //0x0012 
    __int8 Force; //0x0013 
    __int8 Animation; //0x0014 works
    __int8 Damage; //0x0015 
    __int8 Collision; //0x0016 
    __int8 Transparent; //0x0017 
    __int8 Sound; //0x0018 
    __int8 SoundControllers; //0x0019 
    __int8 HDR; //0x001A 
    __int8 Path; //0x001B 
    __int8 Network; //0x001C 
    __int8 ManCollision; //0x001D 
    __int8 Wind; //0x001E 
    __int8 Rumble; //0x001F 
    __int8 AutoAction; //0x0020 
    __int8 Model; //0x0021 works
    __int8 MapScale; //0x0022 
    __int8 ID; //0x0023 
    __int8 Ambient; //0x0024 
    __int8 Updates; //0x0025 
    __int8 EAX; //0x0026 
    __int8 MatLOD; //0x0027 
    __int8 Roads; //0x0028 
    __int8 Light; //0x0029 
    __int8 ContactJoints; //0x002A 
    __int8 PermanentJoints; //0x002B 
    __int8 BBTree; //0x002C 
    __int8 Phys; //0x002D 
    __int8 Resource; //0x002E works
    __int8 FSM; //0x002F works
    __int8 Prune; //0x0030 
    __int8 LogAnimPaths; //0x0031 
    __int8 Streaming; //0x0032 
    __int8 MouseSensitivity; //0x0033 
    __int8 UIControls; //0x0034 works
    __int8 CursorTargetObject; //0x0035 works
    __int8 Terrain; //0x0036 
    __int8 Temperature; //0x0037 Temperature
    __int8 TIHistogram; //0x0038 Seems implemented
    __int8 PathFind; //0x0039 
    __int8 MinObjectDistance; //0x003A 
    __int8 OperCache; //0x003B 
    __int8 ShowGCollision; //0x003C 
    __int8 ShowGRoadway; //0x003D 
    __int8 LateInit; //0x003E 
    __int8 Convoy; //0x003F 
    __int8 ProfileScripts; //0x0040 
    __int8 CombatMode; //0x0041 
    __int8 EPE; //0x0042 works
    __int8 EPEDraw; //0x0043 
    __int8 Shots; //0x0044 
    __int8 Impact; //0x0045 
    __int8 Radius; //0x0046 
    __int8 Speed; //0x0047 
    __int8 Mines; //0x0048 
    __int8 MinesDetect; //0x0049 
    __int8 Anomalies; //0x004A 
    __int8 Fatigue; //0x004B 
    __int8 Complexity; //0x004C workd
    __int8 pruneX; //0x004D 
    __int8 Scene; //0x004E 
    __int8 Weather; //0x004F 
    __int8 LogicalWater; //0x0050 
    __int8 CameraTransform; //0x0051 
    __int8 AmbLife; //0x0052 
    __int8 DamageSimulator; //0x0053 
    __int8 DEHitPoints; //0x0054 
    __int8 DEHitPointsText; //0x0055 
    __int8 SoldierTransform; //0x0056 
    __int8 Buoyancy; //0x0057 
    __int8 WaterSplash; //0x0058 
    __int8 Effects; //0x0059 
    __int8 SimulWeather; //0x005A 
    __int8 ParticleGen; //0x005B 
    __int8 Visibility; //0x005C 
    __int8 EPESimul; //0x005D 
    __int8 BBox; //0x005E 
    __int8 AnimSrcUnit; //0x005F 
    __int8 AnimSrcTarget; //0x0060 
    __int8 VisualStates; //0x0061 Lags the game.. So probably does something
    __int8 VisualStatesIcon; //0x0062 
    __int8 VisualStatesWeapon; //0x0063 
    __int8 Joystick; //0x0064 
    __int8 Gearbox; //0x0065 
    __int8 Impulse; //0x0066 
    __int8 Destruct; //0x0067 
    __int8 MoveForces; //0x0068 
    __int8 ObjMatrix; //0x0069 
    __int8 PXMatrix; //0x006A 
    __int8 Suspension; //0x006B 
    __int8 EPEPerf; //0x006C 
    __int8 EPEStats; //0x006D 
    __int8 EPEForce; //0x006E 
    __int8 EPEVehicle; //0x006F 
    __int8 Formations; //0x0070 works. But looks like it's waypoint
    __int8 Waypoints; //0x0071 
    __int8 ParticleNames; //0x0072 works
    __int8 WingVortices; //0x0073 
    __int8 FootSteps; //0x0074 works 
    __int8 Keyboard; //0x0075 
    __int8 Particles; //0x0076 
    __int8 ObjNames; //0x0077 
    __int8 ObjIds; //0x0078 
    __int8 PXCollision; //0x0079 
    __int8 SoundMap; //0x007A 
    __int8 DynLights; //0x007B 
    __int8 Head; //0x007C 
    __int8 Clutter; //0x007D 
    __int8 LightGrid; //0x007E 
    __int8 RagDoll; //0x007F 
    __int8 RagDollMP; //0x0080 
    __int8 Attributes; //0x0081 
    __int8 Locality; //0x0082 
    __int8 PP; //0x0083 
    __int8 RestrictedZones; //0x0084 
    __int8 WaterObjects; //0x0085 
    __int8 Impulses; //0x0086 
    __int8 ProtocolExpressions; //0x0087 
    __int8 DrawCOM; //0x0088 
    __int8 BodyRemove; //0x0089 
    __int8 WreckRemove; //0x008A 
    __int8 Aiming; //0x008B 
    __int8 viewDensity; //0x008C 
    __int8 Exhaust; //0x008D 
    __int8 Throwing; //0x008E 
    __int8 ModelLights; //0x008F 
    __int8 DestructEffects; //0x0090 
    __int8 UnitStats; //0x0091 
    __int8 Land; //0x0092 
    __int8 AnimationCode; //0x0093 
    __int8 Camera; //0x0094 
    __int8 InvisibleVehicle; //0x0095 
    __int8 WorldObjects; //0x0096 sound scene stuff
    __int8 WeaponResting; //0x0097 
    __int8 Matrices; //0x0098 
    __int8 Zoom; //0x0099 
    __int8 PerfInfo; //0x009A 
    __int8 ObjPassCount; //0x009B works
    __int8 RotAxisOffset; //0x009C 
    __int8 ParticleVis; //0x009D 
    __int8 ParticleVisJobs; //0x009E 
    __int8 Rain; //0x009F 
    __int8 DlcContent; //0x00A0 
    __int8 DlcChecks; //0x00A1 
    __int8 StatsManager; //0x00A2 
    __int8 StatsAchievements; //0x00A3 
    __int8 StatsGlobal; //0x00A4 
    __int8 Supply; //0x00A5 
    __int8 WeaponSway; //0x00A6 
    __int8 WeaponMatrices; //0x00A7 
    __int8 ParticlesInput; //0x00A8 
    __int8 DlcUsage; //0x00A9 
    __int8 DlcPopup; //0x00AA 
    __int8 SurfaceUnder; //0x00AB Works
    __int8 Turret; //0x00AC 
    __int8 Ropes; //0x00AD 
    __int8 WeaponDistortion; //0x00AE 
    __int8 Recoil; //0x00AF 
    __int8 Breathing; //0x00B0 
    __int8 Suppress; //0x00B1 
    __int8 SuppressMap; //0x00B2 
    __int8 FFVInertia; //0x00B3 
    __int8 FFVObstruction; //0x00B4 
    __int8 AttachedOn; //0x00B5 
    __int8 CharRotations; //0x00B6 
    __int8 WeaponDeployment; //0x00B7 
    __int8 CameraShake; //0x00B8 
    __int8 FriendsManager; //0x00B9 
    __int8 SoundBullet; //0x00BA  Super sonic crack sources
    __int8 LeaderboardsManager; //0x00BB 
    __int8 AISkill; //0x00BC 
    __int8 AIDriving; //0x00BD 
    __int8 AISteering; //0x00BE 
    __int8 AIAiming; //0x00BF 
    __int8 AIGroup; //0x00C0 
    __int8 AIBrain; //0x00C1 
    __int8 AIDynamicError; //0x00C2 
    __int8 AISuppression; //0x00C3 
    __int8 AICover; //0x00C4 
    __int8 AIAvoidCollision; //0x00C5 
    __int8 AIConvoy; //0x00C6 
    __int8 AIWeapon; //0x00C7 
    __int8 InteractiveAreas3DEN; //0x00C8 
    __int8 RenderLODs; //0x00C9 
    __int8 Analytics; //0x00CA 
    __int8 ObjectListCells; //0x00CB 
    __int8 Performance; //0x00CC 
    __int8 VehicleTransport; //0x00CD 
    __int8 LightSimple; //0x00CE 
    __int8 MobileWeaponSwitch; //0x00CF 
    __int8 SlopeCollision; //0x00D0 
    __int8 SoundEnv; //0x00D1 
    __int8 SensorsVehicle; //0x00D2 works
    __int8 SensorsAmmo; //0x00D3 
    __int8 DynSimGrid; //0x00D4  works
    __int8 DynSimEntities; //0x00D5  works
    __int8 DynSimGroups; //0x00D6 works


};//Size=0x04B0

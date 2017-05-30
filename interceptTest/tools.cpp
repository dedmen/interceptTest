#include "tools.h"
#include <client/headers/intercept.hpp>
#include <fstream>
using namespace intercept;


types::registered_sqf_function _funcExport;

std::vector<std::string> alreadyHave{
    "BIS_fnc_packStaticWeapon",
    "BIS_fnc_unpackStaticWeapon",
    "BIS_fnc_ambientFlyby",
    "BIS_fnc_ambientAnim",
    "BIS_fnc_ambientAnimCombat",
    "BIS_fnc_ambientAnimGetParams",
    "BIS_fnc_animalBehaviour",
    "BIS_fnc_animalRandomization",
    "BIS_fnc_sortBy",
    "BIS_fnc_arrayShuffle",
    "BIS_fnc_updatePlayerArray",
    "BIS_fnc_consolidateArray",
    "BIS_fnc_findInPairs",
    "BIS_fnc_getFromPairs",
    "BIS_fnc_addToPairs",
    "BIS_fnc_removeFromPairs",
    "BIS_fnc_setToPairs",
    "BIS_fnc_sortAlphabetically",
    "BIS_fnc_findAllNestedElements",
    "BIS_fnc_bitwiseAND",
    "BIS_fnc_bitwiseOR",
    "BIS_fnc_bitwiseXOR",
    "BIS_fnc_bitwiseNOT",
    "BIS_fnc_bitflagsSet",
    "BIS_fnc_bitflagsUnset",
    "BIS_fnc_bitflagsFlip",
    "BIS_fnc_bitflagsCheck",
    "BIS_fnc_bitflagsToArray",
    "BIS_fnc_briefingAnimate",
    "BIS_fnc_briefingInit",
    "BIS_fnc_enemyTargets",
    "BIS_fnc_enemyDetected",
    "BIS_fnc_fireSupport",
    "BIS_fnc_fireSupportCluster",
    "BIS_fnc_fireSupportVirtual",
    "BIS_fnc_stalk",
    "BIS_fnc_configPath",
    "BIS_fnc_subClasses",
    "BIS_fnc_uniqueClasses",
    "BIS_fnc_loadClass",
    "BIS_fnc_loadEntry",
    "BIS_fnc_unitAddon",
    "BIS_fnc_weaponAddon",
    "BIS_fnc_returnChildren",
    "BIS_fnc_configExtremes",
    "BIS_fnc_weaponsEntityType",
    "BIS_fnc_magazinesEntityType",
    "BIS_fnc_getCfg",
    "BIS_fnc_getCfgIsClass",
    "BIS_fnc_getCfgSubClasses",
    "BIS_fnc_getCfgDataArray",
    "BIS_fnc_getCfgDataObject",
    "BIS_fnc_getCfgDataBool",
    "BIS_fnc_getCfgData",
    "BIS_fnc_getCfgDataPool",
    "BIS_fnc_debugProfile",
    "BIS_fnc_functionMeta",
    "BIS_fnc_camera",
    "BIS_fnc_cameraOld",
    "BIS_fnc_isDemo",
    "BIS_fnc_animViewer",
    "BIS_fnc_logFormat",
    "BIS_fnc_logFormatServer",
    "BIS_fnc_preload",
    "BIS_fnc_errorParamsType",
    "BIS_fnc_isDebugConsoleAllowed",
    "BIS_fnc_debugConsoleExec",
    "BIS_fnc_diagAAR",
    "BIS_fnc_diagAARrecord",
    "BIS_fnc_diagHit",
    "BIS_fnc_diagKey",
    "BIS_fnc_diagRadio",
    "BIS_fnc_diagMissionWeapons",
    "BIS_fnc_diagMissionPositions",
    "BIS_fnc_diagPreview",
    "BIS_fnc_diagPreviewVehicleCrew",
    "BIS_fnc_diagPreviewCycle",
    "BIS_fnc_diagKeyLayout",
    "BIS_fnc_diagKeyTest",
    "BIS_fnc_diagWiki",
    "BIS_fnc_diagVehicleIcons",
    "BIS_fnc_diagConfig",
    "BIS_fnc_diagLoop",
    "BIS_fnc_locWeaponInfo",
    "BIS_fnc_diagKnownTargets",
    "BIS_fnc_diagKnownAsTarget",
    "BIS_fnc_diagBulletCam",
    "BIS_fnc_diagMacros",
    "BIS_fnc_diagMacrosVerify",
    "BIS_fnc_diagMacrosMapSize",
    "BIS_fnc_diagMacrosNameSound",
    "BIS_fnc_diagMacrosSimpleObject",
    "BIS_fnc_diagMacrosAuthor",
    "BIS_fnc_diagMacrosEditorPreview",
    "BIS_fnc_diagFindMissingAuthors",
    "BIS_fnc_diagObjectPerformance",
    "BIS_fnc_exportGroupFormations",
    "BIS_fnc_exportCfgVehicles",
    "BIS_fnc_exportCfgVehiclesAssetDB",
    "BIS_fnc_exportCfgWeapons",
    "BIS_fnc_exportCfgMagazines",
    "BIS_fnc_exportCfgPatches",
    "BIS_fnc_exportCfgHints",
    "BIS_fnc_exportEditorPreviews",
    "BIS_fnc_exportConfigHierarchy",
    "BIS_fnc_importImageLinks",
    "BIS_fnc_exportFunctionsToWiki",
    "BIS_fnc_diagJIRAlink",
    "BIS_fnc_effectFired",
    "BIS_fnc_effectKilled",
    "BIS_fnc_effectPlankton",
    "BIS_fnc_effectFiredLongSmoke",
    "BIS_fnc_effectFiredRifle",
    "BIS_fnc_effectFiredArtillery",
    "BIS_fnc_effectFiredRocket",
    "BIS_fnc_effectFiredSmokeLauncher",
    "BIS_fnc_effectFiredSmokeLauncher_boat",
    "BIS_fnc_effectFiredFlares",
    "BIS_fnc_effectFiredHeliRocket",
    "BIS_fnc_effectKilledAirDestruction",
    "BIS_fnc_effectKilledAirDestructionStage2",
    "BIS_fnc_effectKilledSecondaries",
    "BIS_fnc_setPPeffectTemplate",
    "BIS_fnc_initWorldScene",
    "BIS_fnc_feedbackMain",
    "BIS_fnc_bloodEffect",
    "BIS_fnc_damageChanged",
    "BIS_fnc_damagePulsing",
    "BIS_fnc_dirtEffect",
    "BIS_fnc_fatigueEffect",
    "BIS_fnc_feedbackInit",
    "BIS_fnc_flamesEffect",
    "BIS_fnc_healing",
    "BIS_fnc_incapacitatedEffect",
    "BIS_fnc_indicateBleeding",
    "BIS_fnc_radialRed",
    "BIS_fnc_radialRedOut",
    "BIS_fnc_relScaledDist",
    "BIS_fnc_terrainGradAngle",
    "BIS_fnc_groupIndicator",
    "BIS_fnc_rscLayer",
    "BIS_fnc_guiEffectTiles",
    "BIS_fnc_cinemaBorder",
    "BIS_fnc_textTiles",
    "BIS_fnc_titlecard",
    "BIS_fnc_liveFeed",
    "BIS_fnc_liveFeedEffects",
    "BIS_fnc_liveFeedSetSource",
    "BIS_fnc_liveFeedSetTarget",
    "BIS_fnc_liveFeedTerminate",
    "BIS_fnc_establishingShot",
    "BIS_fnc_showNotification",
    "BIS_fnc_ctrlFitToTextHeight",
    "BIS_fnc_setIDCStreamFriendly",
    "BIS_fnc_typeText",
    "BIS_fnc_typeText2",
    "BIS_fnc_ctrlSetScale",
    "BIS_fnc_credits_movie",
    "BIS_fnc_credits_movieConfig",
    "BIS_fnc_credits_movieSupport",
    "BIS_fnc_showUnitInfo",
    "BIS_fnc_toUpperDisplayTexts",
    "BIS_fnc_createLogRecord",
    "BIS_fnc_showMissionStatus",
    "BIS_fnc_setMissionStatusSlot",
    "BIS_fnc_openFieldManual",
    "BIS_fnc_controlConfigs",
    "BIS_fnc_showWelcomeScreen",
    "BIS_fnc_versionInfo",
    "BIS_fnc_exportGUIBaseClasses",
    "BIS_fnc_advHint",
    "BIS_fnc_advHintArg",
    "BIS_fnc_advHintCall",
    "BIS_fnc_advHintCredits",
    "BIS_fnc_advHintFormat",
    "BIS_fnc_holdActionAdd",
    "BIS_fnc_holdActionRemove",
    "BIS_fnc_addWeapon",
    "BIS_fnc_itemType",
    "BIS_fnc_basicBackpack",
    "BIS_fnc_loadInventory",
    "BIS_fnc_unitHeadgear",
    "BIS_fnc_isThrowable",
    "BIS_fnc_limitAmmunition",
    "BIS_fnc_limitItems",
    "BIS_fnc_limitWeaponItems",
    "BIS_fnc_markWaypoints",
    "BIS_fnc_mapSize",
    "BIS_fnc_markerToTrigger",
    "BIS_fnc_triggerToMarker",
    "BIS_fnc_exportMapToBiTXT",
    "BIS_fnc_basicTask",
    "BIS_fnc_missionTasks",
    "BIS_fnc_missionConversations",
    "BIS_fnc_isCampaign",
    "BIS_fnc_missionHandlers",
    "BIS_fnc_trackMissionTime",
    "BIS_fnc_forceEnd",
    "BIS_fnc_fadeEffect",
    "BIS_fnc_findOverwatch",
    "BIS_fnc_onDiaryChanged",
    "BIS_fnc_colorRGBAtoHTML",
    "BIS_fnc_rankParams",
    "BIS_fnc_colorRGBAtoTexture",
    "BIS_fnc_healthEffects",
    "BIS_fnc_textureMarker",
    "BIS_fnc_textureVehicleIcon",
    "BIS_fnc_localize",
    "BIS_fnc_colorConfigToRGBA",
    "BIS_fnc_keyCode",
    "BIS_fnc_addScriptedEventHandler",
    "BIS_fnc_removeScriptedEventHandler",
    "BIS_fnc_removeAllScriptedEventHandlers",
    "BIS_fnc_callScriptedEventHandler",
    "BIS_fnc_startLoadingScreen",
    "BIS_fnc_endLoadingScreen",
    "BIS_fnc_progressLoadingScreen",
    "BIS_fnc_groupVehicles",
    "BIS_fnc_nearestRoad",
    "BIS_fnc_addCommMenuItem",
    "BIS_fnc_removeCommMenuItem",
    "BIS_fnc_refreshCommMenu",
    "BIS_fnc_interpolateWeather",
    "BIS_fnc_weaponComponents",
    "BIS_fnc_jukebox",
    "BIS_fnc_loop",
    "BIS_fnc_runLater",
    "BIS_fnc_fixDate",
    "BIS_fnc_isLeapYear",
    "BIS_fnc_monthDays",
    "BIS_fnc_disableLoading",
    "BIS_fnc_gridToPos",
    "BIS_fnc_encodeFlags",
    "BIS_fnc_decodeFlags",
    "BIS_fnc_deleteCounter",
    "BIS_fnc_sunriseSunsetTime",
    "BIS_fnc_getArea",
    "BIS_fnc_animateFlag",
    "BIS_fnc_initModules",
    "BIS_fnc_moduleInit",
    "BIS_fnc_moduleHandle",
    "BIS_fnc_moduleModules",
    "BIS_fnc_moduleTriggers",
    "BIS_fnc_moduleUnits",
    "BIS_fnc_moduleExecute",
    "BIS_fnc_initMultiplayer",
    "BIS_fnc_call",
    "BIS_fnc_spawn",
    "BIS_fnc_execFSM",
    "BIS_fnc_execVM",
    "BIS_fnc_execRemote",
    "BIS_fnc_addScore",
    "BIS_fnc_setRespawnDelay",
    "BIS_fnc_onPlayerConnected",
    "BIS_fnc_initPlayable",
    "BIS_fnc_missionTimeLeft",
    "BIS_fnc_deleteVehicleCrew",
    "BIS_fnc_admin",
    "BIS_fnc_netId",
    "BIS_fnc_groupFromNetId",
    "BIS_fnc_objectFromNetId",
    "BIS_fnc_countdown",
    "BIS_fnc_filterString",
    "BIS_fnc_inString",
    "BIS_fnc_ordinalNumber",
    "BIS_fnc_romanNumeral",
    "BIS_fnc_phoneticalWord",
    "BIS_fnc_teamColor",
    "BIS_fnc_alignTabs",
    "BIS_fnc_splitString",
    "BIS_fnc_trimString",
    "BIS_fnc_addSupportLink",
    "BIS_fnc_changeSupportRadioChannel",
    "BIS_fnc_limitSupport",
    "BIS_fnc_removeSupportLink",
    "BIS_fnc_getTurrets",
    "BIS_fnc_objectVar",
    "BIS_fnc_allSynchronizedObjects",
    "BIS_fnc_setRank",
    "BIS_fnc_spawnObjects",
    "BIS_fnc_objectHeight",
    "BIS_fnc_objectType",
    "BIS_fnc_crewCount",
    "BIS_fnc_setIdentity",
    "BIS_fnc_setObjectRotation",
    "BIS_fnc_cargoTurretIndex",
    "BIS_fnc_switchLamp",
    "BIS_fnc_attachToRelative",
    "BIS_fnc_setHitPointDamage",
    "BIS_fnc_createSimpleObject",
    "BIS_fnc_adjustSimpleObject",
    "BIS_fnc_simpleObjectData",
    "BIS_fnc_replaceWithSimpleObject",
    "BIS_fnc_addClassOO",
    "BIS_fnc_createObjectOO",
    "BIS_fnc_validateParametersOO",
    "BIS_fnc_storeParamsValues",
    "BIS_fnc_initParams",
    "BIS_fnc_getParamValue",
    "BIS_fnc_paramDaytime",
    "BIS_fnc_paramWeather",
    "BIS_fnc_paramCountdown",
    "BIS_fnc_paramRespawnTickets",
    "BIS_fnc_paramGuerFriendly",
    "BIS_fnc_paramReviveUnconsciousStateMode",
    "BIS_fnc_paramReviveMode",
    "BIS_fnc_paramReviveDuration",
    "BIS_fnc_paramReviveRequiredTrait",
    "BIS_fnc_paramReviveRequiredItems",
    "BIS_fnc_paramReviveMedicSpeedMultiplier",
    "BIS_fnc_paramReviveBleedOutDuration",
    "BIS_fnc_paramReviveForceRespawnDuration",
    "BIS_fnc_ORBATOpen",
    "BIS_fnc_ORBATAnimate",
    "BIS_fnc_ORBATTooltip",
    "BIS_fnc_ORBATSetGroupFade",
    "BIS_fnc_ORBATSetGroupParams",
    "BIS_fnc_ORBATGetGroupParams",
    "BIS_fnc_ORBATAddGroupOverlay",
    "BIS_fnc_ORBATRemoveGroupOverlay",
    "BIS_fnc_ORBATConfigPreview",
    "BIS_fnc_StrategicMapOpen",
    "BIS_fnc_StrategicMapAnimate",
    "BIS_fnc_StrategicMapMouseButtonClick",
    "BIS_fnc_setUnitInsignia",
    "BIS_fnc_getUnitInsignia",
    "BIS_fnc_selectRespawnTemplate",
    "BIS_fnc_initRespawn",
    "BIS_fnc_showRespawnMenu",
    "BIS_fnc_setRespawnInventory",
    "BIS_fnc_addRespawnInventory",
    "BIS_fnc_removeRespawnInventory",
    "BIS_fnc_getRespawnInventories",
    "BIS_fnc_addRespawnPosition",
    "BIS_fnc_removeRespawnPosition",
    "BIS_fnc_getRespawnPositions",
    "BIS_fnc_getRespawnMarkers",
    "BIS_fnc_missionRespawnType",
    "BIS_fnc_moveToRespawnPosition",
    "BIS_fnc_respawnNone",
    "BIS_fnc_respawnSpectator",
    "BIS_fnc_respawnInstant",
    "BIS_fnc_respawnBase",
    "BIS_fnc_respawnGroup",
    "BIS_fnc_respawnSide",
    "BIS_fnc_respawnEndMission",
    "BIS_fnc_respawnSeagull",
    "BIS_fnc_respawnTimePenalty",
    "BIS_fnc_respawnWave",
    "BIS_fnc_respawnCounter",
    "BIS_fnc_respawnConfirm",
    "BIS_fnc_respawnTickets",
    "BIS_fnc_respawnMenuPosition",
    "BIS_fnc_respawnMenuInventory",
    "BIS_fnc_showRespawnMenuDisableItem",
    "BIS_fnc_showRespawnMenuDisableItemDraw",
    "BIS_fnc_showRespawnMenuDisableItemCheck",
    "BIS_fnc_showRespawnMenuHeader",
    "BIS_fnc_showRespawnMenuInventory",
    "BIS_fnc_showRespawnMenuInventoryDetails",
    "BIS_fnc_showRespawnMenuInventoryItems",
    "BIS_fnc_showRespawnMenuInventoryList",
    "BIS_fnc_showRespawnMenuInventoryLimit",
    "BIS_fnc_showRespawnMenuInventoryLimitRefresh",
    "BIS_fnc_showRespawnMenuInventoryLimitRespawn",
    "BIS_fnc_showRespawnMenuInventoryLoadout",
    "BIS_fnc_showRespawnMenuInventoryMetadata",
    "BIS_fnc_showRespawnMenuPosition",
    "BIS_fnc_showRespawnMenuPositionList",
    "BIS_fnc_showRespawnMenuPositionMap",
    "BIS_fnc_showRespawnMenuPositionMapDraw",
    "BIS_fnc_showRespawnMenuPositionMapHandle",
    "BIS_fnc_showRespawnMenuPositionMetadata",
    "BIS_fnc_showRespawnMenuPositionName",
    "BIS_fnc_showRespawnMenuPositionRefresh",
    "BIS_fnc_enemySides",
    "BIS_fnc_friendlySides",
    "BIS_fnc_areFriendly",
    "BIS_fnc_sideID",
    "BIS_fnc_sideColor",
    "BIS_fnc_sideName",
    "BIS_fnc_sideType",
    "BIS_fnc_objectSide",
    "BIS_fnc_playerSideFaction",
    "BIS_fnc_sideIsFriendly",
    "BIS_fnc_sideIsEnemy",
    "BIS_fnc_sideNameUnlocalized",
    "BIS_fnc_InstructorFigure",
    "BIS_fnc_bleedTickets",
    "BIS_fnc_setTask",
    "BIS_fnc_setTaskLocal",
    "BIS_fnc_taskVar",
    "BIS_fnc_deleteTask",
    "BIS_fnc_sharedObjectives",
    "BIS_fnc_taskTypeIcon",
    "BIS_fnc_taskSetType",
    "BIS_fnc_taskType",
    "BIS_fnc_taskSetAlwaysVisible",
    "BIS_fnc_taskAlwaysVisible",
    "BIS_fnc_taskChildren",
    "BIS_fnc_taskCompleted",
    "BIS_fnc_taskCreate",
    "BIS_fnc_taskCurrent",
    "BIS_fnc_taskDescription",
    "BIS_fnc_taskDestination",
    "BIS_fnc_taskExists",
    "BIS_fnc_taskHint",
    "BIS_fnc_taskParent",
    "BIS_fnc_taskReal",
    "BIS_fnc_tasksUnit",
    "BIS_fnc_taskSetCurrent",
    "BIS_fnc_taskSetDescription",
    "BIS_fnc_taskSetDestination",
    "BIS_fnc_taskSetState",
    "BIS_fnc_taskState",
    "BIS_fnc_vehicleRoles",
    "BIS_fnc_missileLaunchPositionFix",
    "BIS_fnc_loadFunctions",
    "BIS_fnc_areEqualNotNil",
    "BIS_fnc_vectorDirAndUpRelative",
    "BIS_fnc_weaponDirectionRelative",
    "BIS_fnc_scriptedWaypointType",
    "BIS_fnc_wpLand",
    "BIS_fnc_wpPatrol",
    "BIS_fnc_wpRelax",
    "BIS_fnc_wpArtillery",
    "BIS_fnc_wpSuppress",
    "BIS_fnc_moduleFiringDrill",
    "BIS_fnc_moduleFDCPIn",
    "BIS_fnc_moduleFDCPOut",
    "BIS_fnc_moduleFDCPClear",
    "BIS_fnc_moduleFDStatsClear",
    "BIS_fnc_moduleFDSkeetDestruction",
    "BIS_fnc_moduleFDFadeMarker",
    "BIS_fnc_moduleTimeTrial",
    "BIS_fnc_moduleTTCPIn",
    "BIS_fnc_moduleTTCPOut",
    "BIS_fnc_moduleTTCPClear",
    "BIS_fnc_moduleTTCPTrigger",
    "BIS_fnc_moduleTTStatsClear",
    "BIS_fnc_moduleTTCPTriggerBehind",
    "BIS_fnc_moduleFDBalloonAirDestruction",
    "BIS_fnc_moduleFDBalloonWaterDestruction",
    "BIS_fnc_ambientBoats",
    "BIS_fnc_ambientHelicopters",
    "BIS_fnc_ambientPlanes",
    "BIS_fnc_ambientBlacklist",
    "BIS_fnc_ambientBlacklistAdd",
    "BIS_fnc_ambientPostprocess",
    "BIS_fnc_kbTell",
    "BIS_fnc_kbTellLocal",
    "BIS_fnc_kbMenu",
    "BIS_fnc_kbSentence",
    "BIS_fnc_kbTopicConfig",
    "BIS_fnc_kbPriority",
    "BIS_fnc_kbCanSpeak",
    "BIS_fnc_kbSkip",
    "BIS_fnc_kbCreateDummy",
    "BIS_fnc_kbIsSpeaking",
    "BIS_fnc_genericSentence",
    "BIS_fnc_genericSentenceInit",
    "BIS_fnc_dbSymbolClass",
    "BIS_fnc_dbSymbolValue",
    "BIS_fnc_dbisClass",
    "BIS_fnc_dbisValue",
    "BIS_fnc_dbClassId",
    "BIS_fnc_dbClassSet",
    "BIS_fnc_dbClassCheck",
    "BIS_fnc_dbClassIndex",
    "BIS_fnc_dbClassRemove",
    "BIS_fnc_dbClassReturn",
    "BIS_fnc_dbClassList",
    "BIS_fnc_dbValueId",
    "BIS_fnc_dbValueSet",
    "BIS_fnc_dbValueCheck",
    "BIS_fnc_dbValueIndex",
    "BIS_fnc_dbValueRemove",
    "BIS_fnc_dbValueReturn",
    "BIS_fnc_dbValueList",
    "BIS_fnc_dbImportConfig",
    "BIS_fnc_dbImportXML",
    "BIS_fnc_dbConfigPath",
    "BIS_fnc_dbPrint",
    "BIS_fnc_error",
    "BIS_fnc_errorMsg",
    "BIS_fnc_halt",
    "BIS_fnc_param",
    "BIS_fnc_paramIn",
    "BIS_fnc_log",
    "BIS_fnc_functionsDebug",
    "BIS_fnc_functionPath",
    "BIS_fnc_codePerformance",
    "BIS_fnc_GC",
    "BIS_fnc_GCinit",
    "BIS_fnc_shutdown",
    "BIS_fnc_getIDD",
    "BIS_fnc_getIDC",
    "BIS_fnc_GUIeditor",
    "BIS_fnc_GUIbackground",
    "BIS_fnc_GUImessage",
    "BIS_fnc_GUIhint",
    "BIS_fnc_GUIgrid",
    "BIS_fnc_GUIgridToProfile",
    "BIS_fnc_GUInewsfeed",
    "BIS_fnc_PIP",
    "BIS_fnc_camFollow",
    "BIS_fnc_displayColorSet",
    "BIS_fnc_displayColorGet",
    "BIS_fnc_displayControls",
    "BIS_fnc_displayResize",
    "BIS_fnc_displayClouds",
    "BIS_fnc_displayLoading",
    "BIS_fnc_displayMission",
    "BIS_fnc_overviewTimeTrial",
    "BIS_fnc_overviewMission",
    "BIS_fnc_overviewAuthor",
    "BIS_fnc_overviewTerrain",
    "BIS_fnc_overviewDifficulty",
    "BIS_fnc_diaryHints",
    "BIS_fnc_diaryMaps",
    "BIS_fnc_HUDLimits",
    "BIS_fnc_credits",
    "BIS_fnc_titleText",
    "BIS_fnc_ctrlTextHeight",
    "BIS_fnc_shakeGauges",
    "BIS_fnc_helicopterDamage",
    "BIS_fnc_helicopterCanFly",
    "BIS_fnc_helicopterSeat",
    "BIS_fnc_helicopterSeatMove",
    "BIS_fnc_helicopterType",
    "BIS_fnc_helicopterGetHitpoints",
    "BIS_fnc_KMLimport",
    "BIS_fnc_noFlyZonesCreate",
    "BIS_fnc_noFlyZonesExport",
    "BIS_fnc_noFlyZone",
    "BIS_fnc_keypointsExport",
    "BIS_fnc_keypointsExportFromKML",
    "BIS_fnc_worldArea",
    "BIS_fnc_randomPos",
    "BIS_fnc_randomPosTrigger",
    "BIS_fnc_markerCreate",
    "BIS_fnc_markerParams",
    "BIS_fnc_markerPath",
    "BIS_fnc_posDegtoUTM",
    "BIS_fnc_posDegToWorld",
    "BIS_fnc_posUTMToDeg",
    "BIS_fnc_nearestHelipad",
    "BIS_fnc_endMission",
    "BIS_fnc_onLoad",
    "BIS_fnc_onEnd",
    "BIS_fnc_displayName",
    "BIS_fnc_counter",
    "BIS_fnc_isLocalized",
    "BIS_fnc_roundDir",
    "BIS_fnc_saveGame",
    "BIS_fnc_animType",
    "BIS_fnc_position",
    "BIS_fnc_assignPlayerRole",
    "BIS_fnc_convertUnits",
    "BIS_fnc_moveIn",
    "BIS_fnc_setHeight",
    "BIS_fnc_playerName",
    "BIS_fnc_singleMissionName",
    "BIS_fnc_singleMissionConfig",
    "BIS_fnc_singleMissionKeys",
    "BIS_fnc_MP",
    "BIS_fnc_MPexec",
    "BIS_fnc_numberDigits",
    "BIS_fnc_numberText",
    "BIS_fnc_radioSetChannel",
    "BIS_fnc_radioSetPlaylist",
    "BIS_fnc_radioSetTrack",
    "BIS_fnc_secondsToString",
    "BIS_fnc_customGPS",
    "BIS_fnc_customGPSVideo",
    "BIS_fnc_3Dcredits",
    "BIS_fnc_dirIndicator",
    "BIS_fnc_playVideo",
    "BIS_fnc_sandstorm",
    "BIS_fnc_crows",
    "BIS_fnc_destroyCity",
    "BIS_fnc_flies",
    "BIS_fnc_spawnEnemy",
    "BIS_fnc_findExtreme",
    "BIS_fnc_configviewer",
    "BIS_fnc_AAN",
    "BIS_fnc_infoText",
    "BIS_fnc_dynamicText",
    "BIS_fnc_halo",
    "BIS_fnc_music",
    "BIS_fnc_scriptedMove",
    "BIS_fnc_boundingBoxCorner",
    "BIS_fnc_boundingBoxMarker",
    "BIS_fnc_boundingBoxDimensions",
    "BIS_fnc_boundingCircle",
    "BIS_fnc_UnitCapture",
    "BIS_fnc_UnitCaptureFiring",
    "BIS_fnc_UnitCaptureSimple",
    "BIS_fnc_UnitPlay",
    "BIS_fnc_UnitPlaySimple",
    "BIS_fnc_UnitPlayFiring",
    "BIS_fnc_FTLmanager",
    "BIS_fnc_RespawnManager",
    "BIS_fnc_spotter",
    "BIS_fnc_linearConversion",
    "BIS_fnc_selectRandomWeighted",
    "BIS_fnc_findNestedElement",
    "BIS_fnc_returnNestedElement",
    "BIS_fnc_setNestedElement",
    "BIS_fnc_removeNestedElement",
    "BIS_fnc_maxDiffArray",
    "BIS_fnc_removeIndex",
    "BIS_fnc_selectRandom",
    "BIS_fnc_randomIndex",
    "BIS_fnc_arrayPush",
    "BIS_fnc_arrayPushStack",
    "BIS_fnc_arrayPop",
    "BIS_fnc_arrayShift",
    "BIS_fnc_arrayUnShift",
    "BIS_fnc_arrayCompare",
    "BIS_fnc_arrayFindDeep",
    "BIS_fnc_conditionalSelect",
    "BIS_fnc_subSelect",
    "BIS_fnc_arrayInsert",
    "BIS_fnc_classWeapon",
    "BIS_fnc_classMagazine",
    "BIS_fnc_returnConfigEntry",
    "BIS_fnc_returnParents",
    "BIS_fnc_help",
    "BIS_fnc_recompile",
    "BIS_fnc_diagAnim",
    "BIS_fnc_fps",
    "BIS_fnc_dirTo",
    "BIS_fnc_relativeDirTo",
    "BIS_fnc_relPos",
    "BIS_fnc_distance2D",
    "BIS_fnc_distance2Dsqr",
    "BIS_fnc_getLineDist",
    "BIS_fnc_inAngleSector",
    "BIS_fnc_inv",
    "BIS_fnc_invAdd",
    "BIS_fnc_invRemove",
    "BIS_fnc_invCodeToArray",
    "BIS_fnc_invString",
    "BIS_fnc_invSlots",
    "BIS_fnc_invSlotsEmpty",
    "BIS_fnc_invSlotType",
    "BIS_fnc_findSafePos",
    "BIS_fnc_getFactions",
    "BIS_fnc_inTrigger",
    "BIS_fnc_isPosBlacklisted",
    "BIS_fnc_listPlayers",
    "BIS_fnc_PosToGrid",
    "BIS_fnc_nearestPosition",
    "BIS_fnc_createmenu",
    "BIS_fnc_colorRGBtoHTML",
    "BIS_fnc_addEvidence",
    "BIS_fnc_taskHandler",
    "BIS_fnc_threat",
    "BIS_fnc_commsMenuCreate",
    "BIS_fnc_commsMenuToggleVisibility",
    "BIS_fnc_commsMenuToggleAvailability",
    "BIS_fnc_version",
    "BIS_fnc_randomNum",
    "BIS_fnc_randomInt",
    "BIS_fnc_arithmeticMean",
    "BIS_fnc_geometricMean",
    "BIS_fnc_cutDecimals",
    "BIS_fnc_greatestNum",
    "BIS_fnc_lowestNum",
    "BIS_fnc_nearestNum",
    "BIS_fnc_sortNum",
    "BIS_fnc_roundNum",
    "BIS_fnc_parseNumber",
    "BIS_fnc_isInFrontOf",
    "BIS_fnc_setPitchBank",
    "BIS_fnc_getPitchBank",
    "BIS_fnc_sceneGetParticipants",
    "BIS_fnc_sceneGetPositionByAngle",
    "BIS_fnc_sceneSetPosFormation",
    "BIS_fnc_sceneSetAnimationsForGroup",
    "BIS_fnc_sceneSetBehaviour",
    "BIS_fnc_sceneCreateSceneTrigger",
    "BIS_fnc_miscanim",
    "BIS_fnc_sceneRotate",
    "BIS_fnc_sceneSetObjects",
    "BIS_fnc_sceneGetObjects",
    "BIS_fnc_sceneMiscStuff",
    "BIS_fnc_zzRotate",
    "BIS_fnc_sceneCheckWeapons",
    "BIS_fnc_sceneCreateSoundEntities",
    "BIS_fnc_sceneAreaClearance",
    "BIS_fnc_sceneIntruderDetector",
    "BIS_fnc_timeToString",
    "BIS_fnc_transportService",
    "BIS_fnc_supplydropService",
    "BIS_fnc_supplydrop",
    "BIS_fnc_respect",
    "BIS_fnc_showTime",
    "BIS_fnc_locations",
    "BIS_fnc_areEqual",
    "BIS_fnc_swapVars",
    "BIS_fnc_variableSpaceAdd",
    "BIS_fnc_variableSpaceRemove",
    "BIS_fnc_crossProduct",
    "BIS_fnc_dotProduct",
    "BIS_fnc_magnitude",
    "BIS_fnc_magnitudeSqr",
    "BIS_fnc_unitVector",
    "BIS_fnc_vectorMultiply",
    "BIS_fnc_vectorDiff",
    "BIS_fnc_vectorAdd",
    "BIS_fnc_vectorFromXToY",
    "BIS_fnc_rotateVector2D",
    "BIS_fnc_absSpeed",
    "BIS_fnc_spawnGroup",
    "BIS_fnc_returnGroupComposition",
    "BIS_fnc_spawnVehicle",
    "BIS_fnc_spawnCrew",
    "BIS_fnc_selectCrew",
    "BIS_fnc_taskPatrol",
    "BIS_fnc_taskDefend",
    "BIS_fnc_taskAttack",
    "BIS_fnc_returnVehicleTurrets",
    "BIS_fnc_ObjectsMapper",
    "BIS_fnc_ObjectsGrabber",
    "BIS_fnc_blackOut",
    "BIS_fnc_blackIn",
    "BIS_fnc_buildingPositions",
    "BIS_fnc_isBuildingEnterable",
    "BIS_fnc_addStackedEventHandler",
    "BIS_fnc_executeStackedEventHandler",
    "BIS_fnc_removeStackedEventHandler",
    "BIS_fnc_initExpo",
    "BIS_fnc_enableSaving",
    "BIS_fnc_disableSaving",
    "BIS_fnc_prepareAO",
    "BIS_fnc_quotations",
    "BIS_fnc_showMarkers",
    "BIS_fnc_isInZoom",
    "BIS_fnc_isInsideArea",
    "BIS_fnc_relPosObject",
    "BIS_fnc_earthquake",
    "BIS_fnc_forceCuratorInterface",
    "BIS_fnc_isForcedCuratorInterface",
    "BIS_fnc_curatorObjectRegistered",
    "BIS_fnc_curatorObjectRegisteredTable",
    "BIS_fnc_registerCuratorObject",
    "BIS_fnc_showCuratorFeedbackMessage",
    "BIS_fnc_isCurator",
    "BIS_fnc_removeDestroyedCuratorEditableObjects",
    "BIS_fnc_curatorPinged",
    "BIS_fnc_curatorWaypointPlaced",
    "BIS_fnc_curatorObjectEdited",
    "BIS_fnc_curatorAttachObject",
    "BIS_fnc_curatorRespawn",
    "BIS_fnc_manageCuratorAddons",
    "BIS_fnc_listCuratorPlayers",
    "BIS_fnc_addCuratorAreaFromTrigger",
    "BIS_fnc_setCuratorVisionModes",
    "BIS_fnc_curatorVisionModes",
    "BIS_fnc_toggleCuratorVisionMode",
    "BIS_fnc_mirrorCuratorSettings",
    "BIS_fnc_drawCuratorLocations",
    "BIS_fnc_drawCuratorRespawnMarkers",
    "BIS_fnc_drawCuratorDeaths",
    "BIS_fnc_addCuratorIcon",
    "BIS_fnc_removeCuratorIcon",
    "BIS_fnc_isCuratorEditable",
    "BIS_fnc_curatorAutomatic",
    "BIS_fnc_curatorAutomaticPositions",
    "BIS_fnc_setCuratorAttributes",
    "BIS_fnc_curatorAttributes",
    "BIS_fnc_initCuratorAttribute",
    "BIS_fnc_setCuratorCamera",
    "BIS_fnc_shakeCuratorCamera",
    "BIS_fnc_curatorHint",
    "BIS_fnc_curatorSayMessage",
    "BIS_fnc_exportCuratorCostTable",
    "BIS_fnc_addCuratorChallenge",
    "BIS_fnc_finishCuratorChallenge",
    "BIS_fnc_manageCuratorChallenges",
    "BIS_fnc_formatCuratorChallengeObjects",
    "BIS_fnc_completedCuratorChallengesCount",
    "BIS_fnc_curatorChallengeFireWeapon",
    "BIS_fnc_curatorChallengeGetInVehicle",
    "BIS_fnc_curatorChallengeDestroyVehicle",
    "BIS_fnc_curatorChallengeFindIntel",
    "BIS_fnc_curatorChallengeSpawnLightning",
    "BIS_fnc_curatorChallengeIlluminate",
    "BIS_fnc_setOvercast",
    "BIS_fnc_setFog",
    "BIS_fnc_setDate",
    "BIS_fnc_locationDescription",
    "BIS_fnc_drawAO",
    "BIS_fnc_drawMinefields",
    "BIS_fnc_drawRespawnPositions",
    "BIS_fnc_activateAddons",
    "BIS_fnc_playEndMusic",
    "BIS_fnc_neutralizeUnit",
    "BIS_fnc_isLoading",
    "BIS_fnc_isUnitVirtual",
    "BIS_fnc_endMissionServer",
    "BIS_fnc_exportCfgGroups",
    "BIS_fnc_selectDiarySubject",
    "BIS_fnc_sayMessage",
    "BIS_fnc_playSound",
    "BIS_fnc_playMusic",
    "BIS_fnc_setObjectTexture",
    "BIS_fnc_estimatedTimeLeft",
    "BIS_fnc_initIntelObject",
    "BIS_fnc_initVirtualUnit",
    "BIS_fnc_initRespawnBackpack",
    "BIS_fnc_respawnRounds",
    "BIS_fnc_respawnMenuSpectator",
    "BIS_fnc_setServerVariable",
    "BIS_fnc_getServerVariable",
    "BIS_fnc_moduleEffectsEmitterCreator",
    "BIS_fnc_moduleCreateProjectile",
    "BIS_fnc_moduleGrenade",
    "BIS_fnc_moduleEffectsPlankton",
    "BIS_fnc_moduleEffectsBubbles",
    "BIS_fnc_moduleEffectsShells",
    "BIS_fnc_moduleEffectsSmoke",
    "BIS_fnc_moduleEffectsFire",
    "BIS_fnc_moduleChat",
    "BIS_fnc_moduleDate",
    "BIS_fnc_moduleSaveGame",
    "BIS_fnc_moduleVolume",
    "BIS_fnc_moduleGenericRadio",
    "BIS_fnc_moduleCombatGetIn",
    "BIS_fnc_moduleGroupID",
    "BIS_fnc_moduleTaskCreate",
    "BIS_fnc_moduleTaskSetDescription",
    "BIS_fnc_moduleTaskSetDestination",
    "BIS_fnc_moduleTaskSetState",
    "BIS_fnc_moduleCreateDiaryRecord",
    "BIS_fnc_moduleHQ",
    "BIS_fnc_moduleZoneRestriction",
    "BIS_fnc_moduleFriendlyFire",
    "BIS_fnc_moduleTrident",
    "BIS_fnc_tridentClient",
    "BIS_fnc_tridentHandleDamage",
    "BIS_fnc_tridentSetRelationship",
    "BIS_fnc_tridentGetRelationship",
    "BIS_fnc_tridentExecute",
    "BIS_fnc_moduleUnlockObject",
    "BIS_fnc_moduleUnlockArea",
    "BIS_fnc_moduleSector",
    "BIS_fnc_moduleRespawnVehicle",
    "BIS_fnc_moduleAI",
    "BIS_fnc_moduleAmmo",
    "BIS_fnc_moduleDamage",
    "BIS_fnc_moduleDoorOpen",
    "BIS_fnc_moduleFuel",
    "BIS_fnc_moduleHealth",
    "BIS_fnc_moduleMode",
    "BIS_fnc_modulePositioning",
    "BIS_fnc_moduleRank",
    "BIS_fnc_moduleRating",
    "BIS_fnc_moduleShowHide",
    "BIS_fnc_moduleSkill",
    "BIS_fnc_moduleSimulationManager",
    "BIS_fnc_moduleSimulationManager_grab",
    "BIS_fnc_animalSiteSpawn",
    "BIS_fnc_moduleSiteInit",
    "BIS_fnc_moduleStrategicMapOpen",
    "BIS_fnc_moduleStrategicMapInit",
    "BIS_fnc_moduleStrategicMapMission",
    "BIS_fnc_moduleStrategicMapORBAT",
    "BIS_fnc_moduleStrategicMapImage",
    "BIS_fnc_moduleSupportsInitRequester",
    "BIS_fnc_moduleSupportsInitProvider",
    "BIS_fnc_moduleSupportsInitProviderVirtual",
    "BIS_fnc_modulePoster",
    "BIS_fnc_Door",
    "BIS_fnc_DoorOpen",
    "BIS_fnc_DoorClose",
    "BIS_fnc_DoorNoHandleOpen",
    "BIS_fnc_DoorNoHandleClose",
    "BIS_fnc_HatchOpen",
    "BIS_fnc_HatchClose",
    "BIS_fnc_LockedDoorOpen",
    "BIS_fnc_LockedDoorNoHandleOpen",
    "BIS_fnc_ObjectInventoryAnimatedOpen",
    "BIS_fnc_ObjectInventoryAnimatedClose",
    "BIS_fnc_SingleWingSlideDoorOpen",
    "BIS_fnc_SingleWingSlideDoorClose",
    "BIS_fnc_TwoWingDoorNoHandleOpen",
    "BIS_fnc_TwoWingDoorNoHandleClose",
    "BIS_fnc_TwoWingDoorOneHandleOpen",
    "BIS_fnc_TwoWingDoorOneHandleClose",
    "BIS_fnc_TwoWingDoorTwoHandleOpen",
    "BIS_fnc_TwoWingDoorTwoHandleClose",
    "BIS_fnc_TwoWingSlideDoorOpen",
    "BIS_fnc_TwoWingSlideDoorClose",
    "BIS_fnc_moduleMPTypeDefense",
    "BIS_fnc_moduleMPTypeSeize",
    "BIS_fnc_moduleMPTypeSectorControl",
    "BIS_fnc_moduleMPTypeGameMaster",
    "BIS_fnc_exportInventory",
    "BIS_fnc_saveInventory",
    "BIS_fnc_deleteInventory",
    "BIS_fnc_baseWeapon",
    "BIS_fnc_arsenal",
    "BIS_fnc_addVirtualItemCargo",
    "BIS_fnc_removeVirtualItemCargo",
    "BIS_fnc_getVirtualItemCargo",
    "BIS_fnc_addVirtualWeaponCargo",
    "BIS_fnc_removeVirtualWeaponCargo",
    "BIS_fnc_getVirtualWeaponCargo",
    "BIS_fnc_addVirtualMagazineCargo",
    "BIS_fnc_removeVirtualMagazineCargo",
    "BIS_fnc_getVirtualMagazineCargo",
    "BIS_fnc_addVirtualBackpackCargo",
    "BIS_fnc_removeVirtualBackpackCargo",
    "BIS_fnc_getVirtualBackpackCargo",
    "BIS_fnc_traceBullets",
    "BIS_fnc_getUnitByUid",
    "BIS_fnc_moveAction",
    "BIS_fnc_target",
    "BIS_fnc_VRDrawBorder",
    "BIS_fnc_VRDrawGrid",
    "BIS_fnc_VREffectKilled",
    "BIS_fnc_VRFadeIn",
    "BIS_fnc_VRFadeOut",
    "BIS_fnc_VRSpawnEffect",
    "BIS_fnc_VRSpawnSelector",
    "BIS_fnc_VRTimer",
    "BIS_fnc_VRHitpart",
    "BIS_fnc_VRCourseTargetDesignation1",
    "BIS_fnc_VRCourseTargetDesignation2",
    "BIS_fnc_VRCourseTargetDesignation3",
    "BIS_fnc_VRCourseLaunchers1",
    "BIS_fnc_VRCourseLaunchers2",
    "BIS_fnc_VRCourseLaunchers3",
    "BIS_fnc_VRCoursePlaceables1",
    "BIS_fnc_VRCoursePlaceables2",
    "BIS_fnc_VRCoursePlaceables3",
    "BIS_fnc_VRCourseBallistics1",
    "BIS_fnc_VRCourseBallistics2",
    "BIS_fnc_VRCourseBallistics3",
    "BIS_fnc_VRCourseBallistics4",
    "BIS_fnc_VRCourseCommandingMovement1",
    "BIS_fnc_VRCourseCommandingMovement2",
    "BIS_fnc_VRCourseCommandingBehaviour1",
    "BIS_fnc_VRCourseCommandingBehaviour2",
    "BIS_fnc_VRCourseCommandingBehaviour3",
    "BIS_fnc_VRCourseCommandingVehicles1",
    "BIS_fnc_VRCourseCommandingVehicles2",
    "BIS_fnc_VRCourseCommandingVehicles3",
    "BIS_fnc_VRCourseCommandingActions1",
    "BIS_fnc_VRCourseCommandingActions2",
    "BIS_fnc_VRCourseCommandingActions3",
    "BIS_fnc_animateTaskWaypoint",
    "BIS_fnc_VRCourseHeliBasics1",
    "BIS_fnc_VRCourseHeliBasics2",
    "BIS_fnc_VRCourseHeliBasics3",
    "BIS_fnc_VRCourseHeliSlingload1",
    "BIS_fnc_VRCourseHeliAdvanced1",
    "BIS_fnc_VRCourseHeliAdvanced2",
    "BIS_fnc_VRCourseHeliAdvanced3",
    "BIS_fnc_VRCourseHeliAdvanced4",
    "BIS_fnc_VRCourseHeliAdvanced5",
    "BIS_fnc_VRCourseHeliAdvanced6",
    "BIS_fnc_VRCourseHeliWeapons1",
    "BIS_fnc_VRCourseHeliWeapons2",
    "BIS_fnc_VRCourseHeliWeapons3",
    "BIS_fnc_VRCourseHeliWeapons4",
    "BIS_fnc_paramViewDistance",
    "BIS_fnc_didJip",
    "BIS_fnc_synchronizedObjects",
    "BIS_fnc_ffvUpdate",
    "BIS_fnc_ModuleMPTypeGroundSupport",
    "BIS_fnc_ModuleMPTypeGroundSupportBase",
    "BIS_fnc_baseVehicle",
    "BIS_fnc_exportVehicle",
    "BIS_fnc_garage",
    "BIS_fnc_garage3DEN",
    "BIS_fnc_initVehicleCrew",
    "BIS_fnc_initVehicle",
    "BIS_fnc_initVehicleKart",
    "BIS_fnc_loadVehicle",
    "BIS_fnc_saveVehicle",
    "BIS_fnc_setVehicleMass",
    "BIS_fnc_getVehicleCustomization",
    "BIS_fnc_VRCourseWeaponHandlingA1",
    "BIS_fnc_VRCourseWeaponHandlingA2",
    "BIS_fnc_VRCourseWeaponHandlingA3",
    "BIS_fnc_VRCourseWeaponHandlingB1",
    "BIS_fnc_VRCourseWeaponHandlingB2",
    "BIS_fnc_VRCourseWeaponHandlingB3",
    "BIS_fnc_VRCourseWeaponHandlingC2",
    "BIS_fnc_VRCourseWeaponHandlingC1",
    "BIS_fnc_dynamicGroups",
    "BIS_fnc_getIntersectionsUnderCursor",
    "BIS_fnc_getObjectBBD",
    "BIS_fnc_getName",
    "BIS_fnc_getNetMode",
    "BIS_fnc_lerp",
    "BIS_fnc_getAngleDelta",
    "BIS_fnc_EGObjectiveVisualizer",
    "BIS_fnc_EGObjectiveVisualizerDraw",
    "BIS_fnc_EGSpectator",
    "BIS_fnc_EGSpectatorCamera",
    "BIS_fnc_EGSpectatorCameraTick",
    "BIS_fnc_EGSpectatorCameraSetTarget",
    "BIS_fnc_EGSpectatorCameraResetTarget",
    "BIS_fnc_EGSpectatorCameraPrepareTarget",
    "BIS_fnc_EGSpectatorDraw2D",
    "BIS_fnc_EGSpectatorDraw3D",
    "BIS_fnc_EGSpectatorGetUnitsToDraw",
    "BIS_fnc_DataTerminalAnimate",
    "BIS_fnc_DataTerminalColor",
    "BIS_fnc_3DENExportOldSQM",
    "BIS_fnc_3DENExportTerrainBuilder",
    "BIS_fnc_3DENVisionMode",
    "BIS_fnc_3DENNotification",
    "BIS_fnc_3DENToolbar",
    "BIS_fnc_3DENGrid",
    "BIS_fnc_3DENCamera",
    "BIS_fnc_3DENInterface",
    "BIS_fnc_3DENModuleDescription",
    "BIS_fnc_3DENDrawLocations",
    "BIS_fnc_3DENListLocations",
    "BIS_fnc_3DENMissionPreview",
    "BIS_fnc_3DENStatusBar",
    "BIS_fnc_3DENTutorial",
    "BIS_fnc_3DENEntityMenu",
    "BIS_fnc_3DENControlsHint",
    "BIS_fnc_3DENShowMessage",
    "BIS_fnc_3DENFlashlight",
    "BIS_fnc_3DENIntel",
    "BIS_fnc_3DENDiagCreateList",
    "BIS_fnc_3DENDiagFonts",
    "BIS_fnc_3DENDiagMouseControl",
    "BIS_fnc_3DENExportAttributes",
    "BIS_fnc_initAmmoBox",
    "BIS_fnc_highlightControl",
    "BIS_fnc_initListNBoxSorting",
    "BIS_fnc_initSliderValue",
    "BIS_fnc_Carrier01Init",
    "BIS_fnc_Carrier01PosUpdate",
    "BIS_fnc_Carrier01EdenInit",
    "BIS_fnc_Carrier01EdenDelete",
    "BIS_fnc_Carrier01AnimateDeflectors",
    "BIS_fnc_Carrier01CatapultActionAdd",
    "BIS_fnc_Carrier01CatapultActionRemove",
    "BIS_fnc_Carrier01CatapultLockTo",
    "BIS_fnc_Carrier01CatapultID",
    "BIS_fnc_Carrier01CrewPlayAnim",
    "BIS_fnc_Carrier01CrewInAnim",
    "BIS_fnc_AircraftTailhook",
    "BIS_fnc_AircraftCatapultLaunch",
    "BIS_fnc_AircraftSystemsInit",
    "BIS_fnc_AircraftWingStateCheck",
    "BIS_fnc_PlaneEjection",
    "BIS_fnc_PlaneAiEject",
    "BIS_fnc_PlaneEjectionFX",
    "BIS_fnc_EjectionSeatRelease"
};


game_value exportFuncs(game_value arg) {




    /*
    Author: Karel Moricky

    Description:
    Export function descriptions to Community Wiki.
    Exported text will be copied to clipboard in XML format.
    Import it to wiki using https://community.bistudio.com/wiki?title=Special:Import
    If the page already exists, it will be replaced only when timestamp is newer.

    Parameter(s):
    0: ARRAY - functions filter in format [<tags>,<categories>,<functions>]
    tags: STRING or ARRAY of STRINGs - CfgFunctions tags (e.g., "BIS"). Use empty string for all of them.
    categories: STRING or ARRAY of STRINGs - categories (e.g., "Debug"). Use empty string for all of them.
    functions: STRING or ARRAY of STRINGs - specific function names (e.g., "BIS_fnc_log"). Use empty string for all of them.
    1 (Optional) STRING - timestamp in format "2013-09-12T16:00:00Z"

    Returns:
    STRING - exported text

    Example:
    Export all functions:		[] spawn bis_fnc_exportFunctionsToWiki;
    Export all Array functions:	["","Arrays"] spawn bis_fnc_exportFunctionsToWiki;
    Export specific functions:	["","",["BIS_fnc_log","BIS_fnc_param"]] spawn bis_fnc_exportFunctionsToWiki;
    */

    auto _indent = 0;
    std::string fulltext;
    std::string fulltext2;
    auto _fnc_addLine = [&](std::string text) {
        for (size_t i = 0; i < _indent; i++) {
            fulltext2 += "\t";
        }
        fulltext2 += text + "\r\n";
    };

    auto _fnc_addLineMain = [&](std::string text) {
        for (size_t i = 0; i < _indent; i++) {
            fulltext += "\t";
        }
        fulltext += text + "\r\n";
    };

    auto _cfgRoot = sqf::config_entry() >> "cfgFunctions";
    std::vector<std::string> _projects = { "arma2", "arma2oa", "tkoh", "arma3" };

    //--- Header
    _fnc_addLineMain("<mediawiki xml:lang=\"en\">");
    _indent = 1;
    auto flist = sqf::get_variable(sqf::ui_namespace(), "BIS_functions_list");
    game_value l = sqf::call(flist);
    auto& _functionsList = l.to_array();
    static auto bis_fnc_functionmeta = sqf::get_variable(sqf::mission_namespace(), "bis_fnc_functionmeta");
    std::vector<std::string> pages;
    for (auto& it : _functionsList) {
        std::string funcName(it);
        if (std::find(alreadyHave.begin(), alreadyHave.end(), funcName) != alreadyHave.end()) continue;
        auto _meta = sqf::call(bis_fnc_functionmeta, it);
        std::string _metaPath = _meta[0];
        if (_metaPath.find("A3\\") == std::string::npos) continue;
        auto _metaFormat = _meta[1];
        auto _metaTag = _meta[6];
        auto _metaCategory = _meta[7];
        auto _metaName = _meta[8];

        //--- Get header
        auto _file = sqf::load_file(_metaPath);
        auto fileCode = sqf::preprocess_file(_metaPath);

        auto _rowComment = 0;
        auto index = 0u;
        for (char ch : fileCode) {
            if (ch != 10) {
                _rowComment = index;
                break;
            }
            ++index;
        }
        auto _row = 0;
        std::vector<char> _rowTextArray;
        std::string _fileHeader = "";
        index = 0;
        for (char ch : _file) {
            std::string _chars;
            if (ch == '<')
                _chars = "&lt;";
            else if (ch == '>')
                _chars = "&gt;";
            else if (ch == '&')
                _chars = "&amp;";
            else _chars = ch;
            for (auto x : _chars) {
                _rowTextArray.push_back(x);
            }
            if (ch == 10) {
                auto _rowText = std::string(_rowTextArray.begin(), _rowTextArray.end());
                _rowTextArray.clear();

                if (_rowText.find("author") == std::string::npos) { _fileHeader = _fileHeader + _rowText; };
                _row = _row + 1;
            };
            if (_row == _rowComment) break;
            ++index;
        }
        if (_fileHeader.find("#include") != std::string::npos) { _fileHeader = ""; }; //--- Ignore when confused by macro file

        auto _description = (_fileHeader == "" || _metaFormat != ".sqf") ? "''N/A''" : sqf::format({ "&lt;pre&gt;%1&lt;/pre&gt;&lt;small&gt;''(Placeholder description extracted from the function header by [[BIS_fnc_exportFunctionsToWiki]])''&lt;/small&gt;", _fileHeader });

        auto _project = sqf::get_text(_cfgRoot >> _metaTag >> "project");

        if (_project == "") _project = sqf::to_lower(sqf::product_version().name_short);

        //--- Export
        _fnc_addLine("<page>");
        _indent = 2;
        _fnc_addLine(sqf::format({ "<title>%1</title>",it }));
        _fnc_addLine("<revision>");
        _indent = 3;

        _fnc_addLine(sqf::format({ "<timestamp>%1</timestamp>","2006-04-03T17:42:00Z" }));//---- Arma 3 release date
        _fnc_addLine("<contributor></contributor>");
        _fnc_addLine("<comment>Generated by BIS_fnc_exportFunctionsToWiki</comment>");

        //--- Exported text
        _fnc_addLine("<text>");
        _indent = 0;
        //--- Function template


        _fnc_addLine("{{Function|= Comments");
        _fnc_addLine("____________________________________________________________________________________________");
        _fnc_addLine("");
        _fnc_addLine(sqf::format({ "| %1 |= Game name", _project }));
        _fnc_addLine("");
        _fnc_addLine("|1.00|= Game version");
        _fnc_addLine("____________________________________________________________________________________________");
        _fnc_addLine("");
        _fnc_addLine(sqf::format({ "| %1 |= Description", _description }));
        _fnc_addLine("____________________________________________________________________________________________");
        _fnc_addLine("");
        _fnc_addLine(sqf::format({ "| &lt;!-- [] call [[%1]]; --&gt; |= Syntax", it }));
        _fnc_addLine("|p1= |= Parameter 1");
        _fnc_addLine("");
        _fnc_addLine("| |= Return value");
        _fnc_addLine("____________________________________________________________________________________________");
        _fnc_addLine("");
        _fnc_addLine("|x1= &lt;code&gt;&lt;/code&gt; |= ");
        _fnc_addLine("____________________________________________________________________________________________");
        _fnc_addLine("");
        _fnc_addLine("| |= See also");
        _fnc_addLine("");
        _fnc_addLine("}}");
        _fnc_addLine("");
        _fnc_addLine("&lt;h3 style=\"display:none\"&gt;Notes&lt;/h3&gt;");
        _fnc_addLine("&lt;dl class=\"command_description\"&gt;");
        _fnc_addLine("&lt;!-- Note Section BEGIN --&gt;");
        _fnc_addLine("");
        _fnc_addLine("&lt;!-- Note Section END --&gt;");
        _fnc_addLine("&lt;/dl&gt;");
        _fnc_addLine("");
        _fnc_addLine("&lt;h3 style=\"display:none\"&gt;Bottom Section&lt;/h3&gt;");
        //--- Categories
        _fnc_addLine(sqf::format({ "[[Category:Function Group: %2|{{uc:%1}}]]", _metaName, _metaCategory }));
        _fnc_addLine(sqf::format({ "[[Category:Functions|{{uc:%1}}]]", _metaName }));


        bool _compatible = false;
        for (auto& xxx : _projects) {
            if (xxx == _project) _compatible = true;
            if (_compatible) {
                _fnc_addLine(sqf::format({ "[[Category:{{Name|%2}}: Functions|{{uc:%1}}]]", _metaName, xxx }));
            }
        }

        _indent = 3;
        _fnc_addLine("</text>");
        _fnc_addLine("<minor/>");

        _indent = 2;
        _fnc_addLine("</revision>");
        _indent = 1;
        _fnc_addLine("</page>");



        pages.push_back(fulltext2);
        fulltext2.clear();
    }





    //--- Footer
    //_indent = 0;
    //_fnc_addLineMain("</mediawiki>");
    int Index = 0;
    for (int x1 = 0; x1 < 8; x1++) {
        std::ofstream file("P:\\file" + std::to_string(x1) + ".txt", std::ios_base::app | std::ios_base::binary);
        file << fulltext;
        for (int x2 = 0; Index < pages.size() && x2 < pages.size() / 7; x2++) {
            file << pages[Index];
            Index++;
        }
        file << "</mediawiki>";
    }


    return game_value();
}


game_value initFunctions(game_value arg) {
    return 0.f;
}

types::registered_sqf_function _bis_fnc_itemType;
game_value bis_fnc_itemType(game_value arg) {
    static std::map<std::string, std::pair<std::string, std::string>> magTypes{
        { "shotboundingmine" ,{ "Mine", "MineBounding" } },
        { "shotbullet" ,{ "Magazine", "Bullet" } },
        { "shotcm" ,{ "Magazine", "CounterMeasures" } },
        { "shotdeploy" ,{ "Magazine", "Artillery" } },
        { "shotdirectionalbomb" ,{ "Mine", "MineDirectional" } },
        { "shotgrenade" ,{ "Magazine", "Grenade" } },
        { "shotilluminating" ,{ "Magazine", "Flare" } },
        { "shotlaser" ,{ "Magazine", "Laser" } },
        { "shotmine" ,{ "Mine", "Mine" } },
        { "shotmissile" ,{ "Magazine", "Missile" } },
        { "shotrocket" ,{ "Magazine", "Rocket" } },
        { "shotshell" ,{ "Magazine", "Shell" } },
        { "shotsmoke" ,{ "Magazine", "SmokeShell" } },
        { "shotsmokex" ,{ "Magazine", "SmokeShell" } },
        { "shotspread" ,{ "Magazine", "ShotgunShell" } },
        { "shotsubmunitions" ,{ "Magazine", "Artillery" } }
    };

    static std::map<std::string, std::string> cursorType{
        { "arifle","AssaultRifle" },
        { "bomb","BombLauncher" },
        { "cannon","Cannon" },
        { "gl","GrenadeLauncher" },
        { "laserdesignator","LaserDesignator" },
        { "mg","MachineGun" },
        { "missile","MissileLauncher" },
        { "mortar","Mortar" },
        { "rocket","RocketLauncher" },
        { "sgun","Shotgun" },
        { "throw","Throw" },
        { "smg","SubmachineGun" },
        { "srifle","SniperRifle" }

    };

    std::string _item = arg;

    auto _cfgItem = (sqf::config_entry() >> "cfgweapons" >> _item);
    if (sqf::is_class(_cfgItem)) {
        auto _simulation = sqf::get_text(_cfgItem >> "simulation");
        std::transform(_simulation.begin(), _simulation.end(), _simulation.begin(), ::tolower);

        if (_simulation == "weapon") {
            auto _type = static_cast<int>(sqf::get_number(_cfgItem >> "type"));
            //_itemCategory = "Weapon";

            if (sqf::is_text(_cfgItem >> "type")) {
                auto text = sqf::get_text(_cfgItem >> "type");
                _type = stoi(text);
            }

            switch (_type) {
                case 1:
                case 4:
                case 5:
                case 65536: {
                    auto _cursor = sqf::get_text(_cfgItem >> "cursor");
                    std::transform(_cursor.begin(), _cursor.end(), _cursor.begin(), ::tolower);
                    if (_cursor == "" || _cursor == "emptycursor") {
                        _cursor = sqf::get_text(_cfgItem >> "cursorAim");
                        std::transform(_cursor.begin(), _cursor.end(), _cursor.begin(), ::tolower);
                    }
                    if (_cursor == "") {
                        return { "VehicleWeapon","Horn" };
                    }
                    std::string category = (_type > 4) ? "VehicleWeapon" : "Weapon";

                    auto found = cursorType.find(_cursor);
                    if (found != cursorType.end()) {
                        return { category, found->second };
                    }

                    switch (_type) {
                        case 1: return { category,"Rifle" };
                        case 4: return { category,"Launcher" };
                        case 65536: return { category,"VehicleWeapon" };
                        default:return { category,"UnknownWeapon" };
                    }
                }
                case 2: {
                    return { "Weapon","Handgun" };
                }
                case 4096: {
                    return { "Item","LaserDesignator" };
                }
                case 131072: {
                    auto _infoType = static_cast<int>(sqf::get_number(_cfgItem >> "itemInfo" >> "type"));
                    switch (_infoType) {
                        case 101: return { "Item","AccessoryMuzzle" };
                        case 201: return { "Item","AccessorySights" };
                        case 301: return { "Item","AccessoryPointer" };
                        case 302: return { "Item","AccessoryBipod" };
                        case 401: return { "Item","FirstAidKit" };
                        case 605: return { "Equipment","Headgear" };
                        case 619: return { "Item","Medikit" };
                        case 620: return { "Item","Toolkit" };
                        case 621: return { "Item","UAVTerminal" };
                        case 701: return { "Equipment","Vest" };
                        case 801: return { "Equipment","Uniform" };
                        default: return { "Item","UnknownEquipment" };
                    }
                }
                default: return { "Weapon","UnknownWeapon" };
            }
        }

        if (_simulation == "binocular")return { "Item","Binocular" };
        if (_simulation == "nvgoggles")return { "Item","NVGoggles" };
        if (_simulation == "itemcompass")return { "Item","Compass" };
        if (_simulation == "itemgps")return { "Item","GPS" };
        if (_simulation == "itemmap")return { "Item","Map" };
        if (_simulation == "itemminedetector")return { "Item","MineDetector" };
        if (_simulation == "itemradio")return { "Item","Radio" };
        if (_simulation == "itemwatch")return { "Item","Watch" };
        if (_simulation == "cmlauncher")return { "VehicleWeapon","CounterMeasuresLauncher" };
        return { "Item","Unknown" };
    }

    if (sqf::is_class(sqf::config_entry() >> "cfgmagazines" >> _item)) {
        auto _ammo = sqf::get_text(sqf::config_entry() >> "cfgammo" >> sqf::get_text(sqf::config_entry() >> "cfgmagazines" >> _item >> "ammo") >> "simulation");
        std::transform(_ammo.begin(), _ammo.end(), _ammo.begin(), ::tolower);

        auto found = magTypes.find(_ammo);
        if (found != magTypes.end()) {
            return { found->second.first, found->second.second };
        }

        return { "Magazine","UnknownMagazine" };
    }

    if (sqf::is_class(sqf::config_entry() >> "cfgvehicles" >> _item)) {
        if (sqf::get_number(sqf::config_entry() >> "cfgvehicles" >> _item >> "isBackpack") > 0)
            return { "Equipment", "Backpack" };
        return { "","" };
    }
    if (sqf::is_class(sqf::config_entry() >> "cfgGlasses" >> _item)) {
        return { "Equipment", "Glasses" };
    }
    return { "","" };
}



void tools::init() {
    _funcExport = client::host::functions.register_sqf_function_unary("Intercept_bis_fnc_exportFunctionsToWiki", "", userFunctionWrapper<exportFuncs>, types::__internal::GameDataType::NOTHING, types::__internal::GameDataType::ARRAY);
    _bis_fnc_itemType = client::host::functions.register_sqf_function_unary("Intercept_bis_fnc_itemType", "", userFunctionWrapper<bis_fnc_itemType>, types::__internal::GameDataType::ARRAY, types::__internal::GameDataType::STRING);

}

void tools::postInit() {
    auto orig = sqf::get_variable(sqf::mission_namespace(), "bis_fnc_itemType");
    auto c = static_cast<game_data_code*>(orig.data.getRef());
    c->is_final = false;
    sqf::set_variable(sqf::mission_namespace(), "bis_fnc_itemType2", orig);
    sqf::set_variable(sqf::mission_namespace(), "bis_fnc_itemType", sqf::compile("Intercept_bis_fnc_itemType _this;"));
}

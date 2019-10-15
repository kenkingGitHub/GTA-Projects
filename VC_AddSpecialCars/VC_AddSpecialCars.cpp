#include "plugin.h"
#include <unordered_set>
#include <string>
#include <fstream>
#include "eVehicleModel.h"
#include "ePedModel.h"
#include "CTheScripts.h"
#include "CWorld.h"
#include "CPopulation.h"
#include "CTimer.h"
#include "CWanted.h"
#include "CStreaming.h"
#include "CModelInfo.h"
#include "eWeaponModel.h"
#include "extensions\ScriptCommands.h"
#include "eScriptCommands.h"
#include "CCarAI.h"
#include "CCopPed.h"

int &/*CCarCtrl::*/MiamiViceCycle = *(int *)0xA0FD6C;
int &/*CCarCtrl::*/LastTimeMiamiViceGenerated = *(int *)0x9751A8;
int &/*CCranes::*/CarsCollectedMilitaryCrane = *(int *)0x9B6CE4;

class CCrane {
public:
    CEntity *m_pObject;
    CObject *m_pMagnet;
    float m_fPickupX1;
    float m_fPickupX2;
    float m_fPickupY1;
    float m_fPickupY2;
    CVector m_vecDropoffTarget;
    float m_fDropoffHeading;
    float m_fPickupAngle;
    float m_fDropoffAngle;
    float m_fPickupDistance;
    float m_fDropoffDistance;
    float m_fAngle;
    float m_fDistance;
    float m_fHeight;
    float m_fHookOffset;
    float m_fHookHeight;
    CVector m_vecHookInitPos;
    CVector m_vecHookCurPos;
    float m_fHookVelocityX;
    float m_fHookVelocityY;
    CVehicle *m_pVehiclePickedUp;
    int m_nUpdateTimer;
    bool m_bCraneActive;
    char m_bCraneStatus;
    char byte_76;
    bool m_bIsCrusher;
    bool m_bIsMilitaryCrane;
    bool byte_79;
    bool m_bNotMilitaryCrane;
    char _pad7B;
};

using namespace plugin;
using namespace std;

int m_nEmergencyServices;

unordered_set<unsigned int>
/*cop vehicles*/            Police_IDs, Fbiranch_IDs, Enforcer_IDs, Barracks_IDs,
/*cop peds*/                Cop_IDs, Swat_IDs, Fbi_IDs, Army_IDs,
/*cop weapons*/             CopWeapon_IDs, SwatWeapon_IDs, FbiWeapon_IDs, ArmyWeapon_IDs,
/*taxi cars*/               Taxi_IDs,
/*emergency cars*/          Ambulan_IDs, Firetruk_IDs,
/*emergency peds*/          Medic_IDs, Fireman_IDs;

bool isCop = false, isSwat = false, isFbi = false, isArmy = false, isMedic = false, isFireman = false,
isAmbulan = false, isFiretruck = false, isEnforcer = true, isFbiranch = true, isBarracks = true;

int randomEmergencyModel = 3, randomFbicar = 2, randomRoadBlocksTime = 0, spawnCarTime = 0, weaponAmmo;

class AddSpecialCars {
public:
    enum eSpawnCarState { STATE_FIND, STATE_WAIT, STATE_CREATE };
    enum eSpecialType { TYPE_MEDIC, TYPE_FIREMAN, TYPE_COP, TYPE_SWAT, TYPE_FBI, TYPE_ARMY };

    static eSpawnCarState m_currentState;
    static short outCount;
    static CVector carPos;
    static float carAngle;
    static CAutoPilot pilot;
    static int currentCopModel, currentSwatModel, currentFbiModel, currentArmyModel, currentWeaponModel;
    static unsigned int currentSpecialModelForSiren, currentSpecialModelForOccupants, currentFiretrukModel, currentRoadBlockModel;
    static unsigned int jmp_53A913, jmp_5945D9, jmp_444040;

    static unsigned int GetRandomModel(unordered_set<unsigned int> IDS) {
        vector<unsigned int> ids;
        for (auto id : IDS)
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static int __stdcall GetSpecialModelForSiren(unsigned int model) {
        if (model == MODEL_POLICE || Police_IDs.find(model) != Police_IDs.end())
            return MODEL_POLICE;
        else if (model == MODEL_TAXI || Taxi_IDs.find(model) != Taxi_IDs.end())
            return MODEL_TAXI;
        else if (model == MODEL_AMBULAN || Ambulan_IDs.find(model) != Ambulan_IDs.end())
            return MODEL_AMBULAN;
        else if (model == MODEL_FBICAR || model == MODEL_FBIRANCH || Fbiranch_IDs.find(model) != Fbiranch_IDs.end())
            return MODEL_FBIRANCH;
        else if (model == MODEL_FIRETRUK || Firetruk_IDs.find(model) != Firetruk_IDs.end())
            return MODEL_FIRETRUK;
        else if (model == MODEL_ENFORCER || Enforcer_IDs.find(model) != Enforcer_IDs.end())
            return MODEL_ENFORCER;
        return model;
    }

    static int __stdcall GetSpecialModelForOccupants(unsigned int model) {
        if (model == MODEL_POLICE || Police_IDs.find(model) != Police_IDs.end())
            return MODEL_POLICE;
        else if (model == MODEL_AMBULAN || Ambulan_IDs.find(model) != Ambulan_IDs.end())
            return MODEL_AMBULAN;
        else if (model == MODEL_FBICAR || model == MODEL_FBIRANCH || Fbiranch_IDs.find(model) != Fbiranch_IDs.end())
            return MODEL_FBIRANCH;
        else if (model == MODEL_FIRETRUK || Firetruk_IDs.find(model) != Firetruk_IDs.end())
            return MODEL_FIRETRUK;
        else if (model == MODEL_BARRACKS || Barracks_IDs.find(model) != Barracks_IDs.end())
            return MODEL_BARRACKS;
        else if (model == MODEL_ENFORCER || Enforcer_IDs.find(model) != Enforcer_IDs.end())
            return MODEL_ENFORCER;
        return model;
    }

    static int __stdcall GetFiretrukModel(unsigned int model) {
        if (model == MODEL_FIRETRUK || Firetruk_IDs.find(model) != Firetruk_IDs.end())
            return MODEL_FIRETRUK;
        return model;
    }

    static int __stdcall GetRoadBlockModel(unsigned int model) {
        if (model == MODEL_ENFORCER || Enforcer_IDs.find(model) != Enforcer_IDs.end())
            return MODEL_ENFORCER;
        else if (model == MODEL_FBIRANCH || Fbiranch_IDs.find(model) != Fbiranch_IDs.end())
            return MODEL_FBIRANCH;
        else if (model == MODEL_BARRACKS || Barracks_IDs.find(model) != Barracks_IDs.end())
            return MODEL_BARRACKS;
        return model;
    }

    static int __stdcall GetSpecialModel(eSpecialType type) {
        int result;
        switch (type) {
        case TYPE_MEDIC:
            result = MODEL_MEDIC;
            break;
        case TYPE_FIREMAN:
            result = MODEL_FIREMAN;
            break;
        case TYPE_COP:
            result = MODEL_COP;
            break;
        case TYPE_SWAT:
            result = MODEL_SWAT;
            break;
        case TYPE_FBI:
            result = MODEL_FBI;
            break;
        case TYPE_ARMY:
            result = MODEL_ARMY;
            break;
        }
        return result;
    }

    static int __stdcall GetCurrentPedModel(unordered_set<unsigned int> IDs, eSpecialType type) {
        if (IDs.size()) {
            unsigned int pedId = GetRandomModel(IDs);
            if (CModelInfo::IsPedModel(pedId) && LoadModel(pedId))
                return pedId;
            else
                return GetSpecialModel(type);
        }
        else
            return GetSpecialModel(type);
    }

    static int __stdcall GetCurrentCopModel() {
        if (Cop_IDs.size()) {
            if (isCop) {
                isCop = false; return MODEL_COP;
            }
            else {
                unsigned int copId = GetRandomModel(Cop_IDs); 
                if (CModelInfo::IsPedModel(copId) && LoadModel(copId)) {
                    isCop = true; return copId;
                }
                else {
                    isCop = false; return MODEL_COP;
                }
            }
        }
        else
            return MODEL_COP;
    }

    static int __stdcall GetCurrentSwatModel() {
        if (Swat_IDs.size()) {
            if (isSwat) {
                isSwat = false; return MODEL_SWAT;
            }
            else {
                unsigned int swatId = GetRandomModel(Swat_IDs); 
                if (CModelInfo::IsPedModel(swatId) && LoadModel(swatId)) {
                    isSwat = true; return swatId;
                }
                else {
                    isSwat = false; return MODEL_SWAT;
                }
            }
        }
        else
            return MODEL_SWAT;
    }

    static int __stdcall GetCurrentFbiModel() {
        if (Fbi_IDs.size()) {
            if (isFbi) {
                isFbi = false; return MODEL_FBI;
            }
            else {
                unsigned int fbiId = GetRandomModel(Fbi_IDs); 
                if (CModelInfo::IsPedModel(fbiId) && LoadModel(fbiId)) {
                    isFbi = true; return fbiId;
                }
                else {
                    isFbi = false; return MODEL_FBI;
                }
            }
        }
        else
            return MODEL_FBI;
    }

    static int __stdcall GetCurrentArmyModel() {
        if (Army_IDs.size()) {
            if (isArmy) {
                isArmy = false; return MODEL_ARMY;
            }
            else {
                unsigned int armyId = GetRandomModel(Army_IDs); 
                if (CModelInfo::IsPedModel(armyId) && LoadModel(armyId)) {
                    isArmy = true; return armyId;
                }
                else {
                    isArmy = false; return MODEL_ARMY;
                }
            }
        }
        else
            return MODEL_ARMY;
    }

    static int __stdcall GetCurrentWeaponModel(int type) {
        int result = 25; unsigned int model;
        switch (type) {
        case 1: model = GetRandomModel(SwatWeapon_IDs); break;
        case 2: model = GetRandomModel(FbiWeapon_IDs);  break;
        case 3: model = GetRandomModel(ArmyWeapon_IDs); break;
        }
        switch (model) {
        case MODEL_CHROMEGUN:
            if (LoadModel(MODEL_CHROMEGUN))
                result = 19;
            break;
        case MODEL_SHOTGSPA:
            if (LoadModel(MODEL_SHOTGSPA))
                result = 20;
            break;
        case MODEL_BUDDYSHOT:
            if (LoadModel(MODEL_BUDDYSHOT))
                result = 21;
            break;
        case MODEL_TEC9:
            if (LoadModel(MODEL_TEC9))
                result = 22;
            break;
        case MODEL_UZI:
            if (LoadModel(MODEL_UZI))
                result = 23;
            break;
        case MODEL_INGRAMSL:
            if (LoadModel(MODEL_INGRAMSL))
                result = 24;
            break;
        case MODEL_MP5LNG:
            if (LoadModel(MODEL_MP5LNG))
                result = 25;
            break;
        case MODEL_M4:
            if (LoadModel(MODEL_M4))
                result = 26;
            break;
        default: 
            if (type == 1)
                result = 23;
            else
                result = 25;  
            break;
        }
        return result;
    }

    static void Patch_58BE1F(); // Siren
    static void Patch_53A905(); // AddPedInCar
    static void Patch_5945CE(); // FireTruckControl
    static void Patch_444034(); // RoadBlockCopsForCar
    static void Patch_4ED743(); // RandomCop
    static void Patch_4ED7C2(); // RandomSwat
    static void Patch_4ED811(); // RandomFbi
    static void Patch_4ED833(); // RandomArmy

    // CCranes::DoesMilitaryCraneHaveThisOneAlready
    static bool __cdecl DoesMilitaryCraneHaveThisOneAlready(int model) {
        if (model == MODEL_FIRETRUK || Firetruk_IDs.find(model) != Firetruk_IDs.end()) {
            if (CarsCollectedMilitaryCrane & 1)
                return true;
        }
        else if (model == MODEL_AMBULAN || Ambulan_IDs.find(model) != Ambulan_IDs.end()) {
            if (CarsCollectedMilitaryCrane & 2)
                return true;
        }
        else if (model == MODEL_ENFORCER || Enforcer_IDs.find(model) != Enforcer_IDs.end()) {
            if (CarsCollectedMilitaryCrane & 4)
                return true;
        }
        else if (model == MODEL_FBICAR || model == MODEL_FBIRANCH || Fbiranch_IDs.find(model) != Fbiranch_IDs.end()) {
            if (CarsCollectedMilitaryCrane & 8)
                return true;
        }
        else if (model == MODEL_RHINO) {
            if (CarsCollectedMilitaryCrane & 0x10)
                return true;
        }
        else if (model == MODEL_BARRACKS || Barracks_IDs.find(model) != Barracks_IDs.end()) {
            if (CarsCollectedMilitaryCrane & 0x20)
                return true;
        }
        else if (model == MODEL_POLICE || Police_IDs.find(model) != Police_IDs.end()) {
            if (CarsCollectedMilitaryCrane & 0x40)
                return true;
        }
        return false;
    }

    // CCranes::RegisterCarForMilitaryCrane
    static void __cdecl RegisterCarForMilitaryCrane(int model) {
        if (model == MODEL_FIRETRUK || Firetruk_IDs.find(model) != Firetruk_IDs.end())
            CarsCollectedMilitaryCrane |= 1;
        else if (model == MODEL_AMBULAN || Ambulan_IDs.find(model) != Ambulan_IDs.end())
            CarsCollectedMilitaryCrane |= 2;
        else if (model == MODEL_ENFORCER || Enforcer_IDs.find(model) != Enforcer_IDs.end())
            CarsCollectedMilitaryCrane |= 4;
        else if (model == MODEL_FBICAR || model == MODEL_FBIRANCH || Fbiranch_IDs.find(model) != Fbiranch_IDs.end())
            CarsCollectedMilitaryCrane |= 8;
        else if (model == MODEL_RHINO)
            CarsCollectedMilitaryCrane |= 0x10;
        else if (model == MODEL_BARRACKS || Barracks_IDs.find(model) != Barracks_IDs.end())
            CarsCollectedMilitaryCrane |= 0x20;
        else if (model == MODEL_POLICE || Police_IDs.find(model) != Police_IDs.end())
            CarsCollectedMilitaryCrane |= 0x40;
    }

    // CCrane::DoesCranePickUpThisCarType
    static bool __fastcall DoesCranePickUpThisCarType(CCrane *_this, int, int model) {
        bool result;

        if (_this->m_bIsCrusher) {
            result = model != MODEL_FIRETRUK && !(Firetruk_IDs.find(model) != Firetruk_IDs.end())
                && model != MODEL_TRASH && model != MODEL_SECURICA && model != MODEL_BUS && model != MODEL_RHINO;
        }
        else if (_this->m_bIsMilitaryCrane) {
            result = model == MODEL_FIRETRUK || Firetruk_IDs.find(model) != Firetruk_IDs.end()
                || model == MODEL_ENFORCER || Enforcer_IDs.find(model) != Enforcer_IDs.end()
                || model == MODEL_AMBULAN || Ambulan_IDs.find(model) != Ambulan_IDs.end()
                || model == MODEL_FBICAR || model == MODEL_FBIRANCH || Fbiranch_IDs.find(model) != Fbiranch_IDs.end()
                || model == MODEL_RHINO || model == MODEL_BARRACKS || model == MODEL_POLICE
                || Barracks_IDs.find(model) != Barracks_IDs.end()
                || Police_IDs.find(model) != Police_IDs.end();
        }
        else
            result = true;
        return result;
    }

    // CVehicle::IsLawEnforcementVehicle
    static bool __fastcall IsLawEnforcementVehicle(CVehicle *_this) {
        bool result; result = false;
        if (_this->m_nModelIndex == MODEL_FBICAR || _this->m_nModelIndex == MODEL_FBIRANCH || Fbiranch_IDs.find(_this->m_nModelIndex) != Fbiranch_IDs.end())
            result = true;
        else if (_this->m_nModelIndex == MODEL_ENFORCER || Enforcer_IDs.find(_this->m_nModelIndex) != Enforcer_IDs.end())
            result = true;
        else if (_this->m_nModelIndex == MODEL_PREDATOR || _this->m_nModelIndex == MODEL_RHINO || _this->m_nModelIndex == MODEL_VICECHEE)
            result = true;
        else if (_this->m_nModelIndex == MODEL_BARRACKS || Barracks_IDs.find(_this->m_nModelIndex) != Barracks_IDs.end())
            result = true;
        else if (_this->m_nModelIndex == MODEL_POLICE || Police_IDs.find(_this->m_nModelIndex) != Police_IDs.end())
            result = true;
        return result;
    }

    // CCarAI::AddPoliceCarOccupants
    static void __cdecl AddPoliceCarOccupants(CVehicle *vehicle)  {
        if (!vehicle->m_nVehicleFlags.bIsLawEnforcementVeh) {
            vehicle->m_nVehicleFlags.bIsLawEnforcementVeh = 1;
            if (Police_IDs.find(vehicle->m_nModelIndex) != Police_IDs.end() || Barracks_IDs.find(vehicle->m_nModelIndex) != Barracks_IDs.end()) {
                vehicle->SetUpDriver();
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
                    vehicle->SetupPassenger(0);
                return;
            }
            else if (Fbiranch_IDs.find(vehicle->m_nModelIndex) != Fbiranch_IDs.end() || Enforcer_IDs.find(vehicle->m_nModelIndex) != Enforcer_IDs.end()) {
                vehicle->SetUpDriver();
                vehicle->SetupPassenger(0);
                vehicle->SetupPassenger(1);
                vehicle->SetupPassenger(2);
                return;
            }
            else if (vehicle->m_nModelIndex == MODEL_VICECHEE) {
                CPed *driver = vehicle->SetUpDriver();
                driver->m_nPedFlags.bMiamiViceCop = 1;
                CPed *passenger = vehicle->SetupPassenger(0);
                passenger->m_nPedFlags.bMiamiViceCop = 1;
                CPopulation::NumMiamiViceCops += 2;
                /*CCarCtrl::*/MiamiViceCycle = (/*CCarCtrl::*/MiamiViceCycle + 1) % 4;
                /*CCarCtrl::*/LastTimeMiamiViceGenerated = CTimer::m_snTimeInMilliseconds;
                return;
            }
            switch (vehicle->m_nModelIndex) {
            case MODEL_PREDATOR:
                vehicle->SetUpDriver();
                break;
            case MODEL_POLICE:
            case MODEL_RHINO:
            case MODEL_BARRACKS:
                vehicle->SetUpDriver();
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
                    vehicle->SetupPassenger(0);
                break;
            case MODEL_FBICAR:
            case MODEL_FBIRANCH:
            case MODEL_ENFORCER:
                vehicle->SetUpDriver();
                vehicle->SetupPassenger(0);
                vehicle->SetupPassenger(1);
                vehicle->SetupPassenger(2);
                break;
            default:
                return;
            }
        }
    }

    // CCarCtrl::ChoosePoliceCarModel
    static int __cdecl ChoosePoliceCarModel() {
        CPlayerPed *player = FindPlayerPed();
        if (player) {
            if (player->m_pWanted->AreMiamiViceRequired()
                && CTimer::m_snTimeInMilliseconds > /*CCarCtrl::*/LastTimeMiamiViceGenerated + 120000
                && CStreaming::ms_aInfoForModel[MODEL_VICECHEE].m_nLoadState == LOADSTATE_LOADED)
            {
                switch (/*CCarCtrl::*/MiamiViceCycle) {
                case 0:
                    if (CStreaming::ms_aInfoForModel[MODEL_VICE1].m_nLoadState != LOADSTATE_LOADED || CStreaming::ms_aInfoForModel[MODEL_VICE2].m_nLoadState != LOADSTATE_LOADED)
                        goto LABEL_17;
                    return MODEL_VICECHEE;
                case 1:
                    if (CStreaming::ms_aInfoForModel[MODEL_VICE3].m_nLoadState != LOADSTATE_LOADED || CStreaming::ms_aInfoForModel[MODEL_VICE4].m_nLoadState != LOADSTATE_LOADED)
                        goto LABEL_17;
                    return MODEL_VICECHEE;
                case 2:
                    if (CStreaming::ms_aInfoForModel[MODEL_VICE5].m_nLoadState != LOADSTATE_LOADED || CStreaming::ms_aInfoForModel[MODEL_VICE6].m_nLoadState != LOADSTATE_LOADED)
                        goto LABEL_17;
                    return MODEL_VICECHEE;
                case 3:
                    if (CStreaming::ms_aInfoForModel[MODEL_VICE7].m_nLoadState != LOADSTATE_LOADED || CStreaming::ms_aInfoForModel[MODEL_VICE8].m_nLoadState != LOADSTATE_LOADED)
                        goto LABEL_17;
                    return MODEL_VICECHEE;
                default: goto LABEL_17;
                }
            }
            else {
            LABEL_17:
                if (player->m_pWanted->AreSwatRequired()
                    && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == LOADSTATE_LOADED
                    && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == LOADSTATE_LOADED)
                {
                    if (plugin::Random(0, 1)) {
                        unsigned int enforcerId = GetRandomModel(Enforcer_IDs); 
                        if (CModelInfo::IsCarModel(enforcerId)) {
                            if (LoadModel(enforcerId))
                                return enforcerId;
                            else
                                return MODEL_ENFORCER;
                        }
                        else
                            return MODEL_ENFORCER;
                    }
                    else
                        return MODEL_ENFORCER;
                }
                else
                {
                    if (player->m_pWanted->AreFbiRequired()
                        && CStreaming::ms_aInfoForModel[MODEL_FBIRANCH].m_nLoadState == LOADSTATE_LOADED
                        && CStreaming::ms_aInfoForModel[MODEL_FBI].m_nLoadState == LOADSTATE_LOADED)
                    {
                        unsigned int fbicarId;
                        if (randomFbicar < 2)
                            randomFbicar++;
                        else
                            randomFbicar = 0;
                        switch (randomFbicar) {
                        case 0:
                            fbicarId = GetRandomModel(Fbiranch_IDs); 
                            if (CModelInfo::IsCarModel(fbicarId)) {
                                if (LoadModel(fbicarId))
                                    return fbicarId;
                                else
                                    return MODEL_FBIRANCH;
                            }
                            else
                                return MODEL_FBIRANCH;
                        case 1:
                            if (LoadModel(MODEL_FBICAR))
                                return MODEL_FBICAR;
                            else
                                return MODEL_FBIRANCH;
                        case 2:  return MODEL_FBIRANCH;
                        default: return MODEL_FBIRANCH;
                        }
                    }
                    else {
                        if (player->m_pWanted->AreArmyRequired()
                            && CStreaming::ms_aInfoForModel[MODEL_RHINO].m_nLoadState == LOADSTATE_LOADED
                            && CStreaming::ms_aInfoForModel[MODEL_BARRACKS].m_nLoadState == LOADSTATE_LOADED
                            && CStreaming::ms_aInfoForModel[MODEL_ARMY].m_nLoadState == LOADSTATE_LOADED)
                        {
                            unsigned int barracksId;
                            int randomArmy = plugin::Random(0, 2);
                            switch (randomArmy) {
                            case 0:
                                barracksId = GetRandomModel(Barracks_IDs); 
                                if (CModelInfo::IsCarModel(barracksId)) {
                                    if (LoadModel(barracksId))
                                        return barracksId;
                                    else
                                        return MODEL_BARRACKS;
                                }
                                else
                                    return MODEL_BARRACKS;
                            case 1: return MODEL_BARRACKS;
                            case 2: return MODEL_RHINO;
                            default: return MODEL_BARRACKS;
                            }
                        }
                        else {
                            if (plugin::Random(0, 1)) {
                                unsigned int policeId = GetRandomModel(Police_IDs); 
                                if (CModelInfo::IsCarModel(policeId)) {
                                    if (LoadModel(policeId))
                                        return policeId;
                                    else
                                        return MODEL_POLICE;
                                }
                                else
                                    return MODEL_POLICE;
                            }
                            else
                                return MODEL_POLICE;
                        }
                    }
                }
            }
        }
        return MODEL_POLICE;
    }

    // CVehicle::UsesSiren
    static bool __fastcall UsesSiren(CVehicle *_this) {
        bool result;
        if (Police_IDs.find(_this->m_nModelIndex) != Police_IDs.end()
            || Fbiranch_IDs.find(_this->m_nModelIndex) != Fbiranch_IDs.end()
            || Enforcer_IDs.find(_this->m_nModelIndex) != Enforcer_IDs.end()
            || Firetruk_IDs.find(_this->m_nModelIndex) != Firetruk_IDs.end()
            || Ambulan_IDs.find(_this->m_nModelIndex) != Ambulan_IDs.end())
            return true;

        switch (_this->m_nModelIndex) {
        case MODEL_FIRETRUK:
        case MODEL_AMBULAN:
        case MODEL_FBICAR:
        case MODEL_MRWHOOP:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_POLICE:
        case MODEL_FBIRANCH:
        case MODEL_VICECHEE:
            result = true;
            break;
        default:
            result = false;
            break;
        }
        return result;
    }

    static void __fastcall OpcodePlayerDrivingTaxiVehicle(CRunningScript *script) {
        script->CollectParameters(&script->m_nIp, 1);
        bool isTaxiModel = false;

        CPlayerPed * player = CWorld::Players[CTheScripts::ScriptParams[0].uParam].m_pPed;
        if (player->m_bInVehicle) {
            unsigned int model = player->m_pVehicle->m_nModelIndex;
            if (model == MODEL_TAXI || model == MODEL_CABBIE || model == MODEL_ZEBRA || model == MODEL_KAUFMAN || Taxi_IDs.find(model) != Taxi_IDs.end())
                isTaxiModel = true;
        }
        script->UpdateCompareFlag(isTaxiModel);
    }

    static void __fastcall OpcodeIsPlayerInModel(CRunningScript *script) {
        script->CollectParameters(&script->m_nIp, 2);
        bool inModel = false;

        CPlayerPed * player = CWorld::Players[CTheScripts::ScriptParams[0].uParam].m_pPed;
        if (player->m_bInVehicle) {
            unsigned int model = player->m_pVehicle->m_nModelIndex;
            if (CTheScripts::ScriptParams[1].uParam == MODEL_POLICE || CTheScripts::ScriptParams[1].uParam == MODEL_ENFORCER
                || CTheScripts::ScriptParams[1].uParam == MODEL_RHINO || CTheScripts::ScriptParams[1].uParam == MODEL_FBICAR
                || CTheScripts::ScriptParams[1].uParam == MODEL_FBIRANCH) 
            {
                if (model == MODEL_RHINO || model == MODEL_POLICE || Police_IDs.find(model) != Police_IDs.end()
                    || model == MODEL_BARRACKS || Barracks_IDs.find(model) != Barracks_IDs.end()
                    || model == MODEL_FBICAR || model == MODEL_FBIRANCH || Fbiranch_IDs.find(model) != Fbiranch_IDs.end()
                    || model == MODEL_ENFORCER || Enforcer_IDs.find(model) != Enforcer_IDs.end())
                    inModel = true; // Vigilante
            }
            else if (CTheScripts::ScriptParams[1].uParam == MODEL_AMBULAN) {
                if (model == MODEL_AMBULAN || Ambulan_IDs.find(model) != Ambulan_IDs.end())
                    inModel = true; // Paramedic
            }
            else if (CTheScripts::ScriptParams[1].uParam == MODEL_FIRETRUK) {
                if (model == MODEL_FIRETRUK || Firetruk_IDs.find(model) != Firetruk_IDs.end())
                    inModel = true; // Firefighter
            }
            else if (model == CTheScripts::ScriptParams[1].uParam)
                inModel = true;
        }
        script->UpdateCompareFlag(inModel);
    }

    static bool LoadModel(int model) {
        unsigned char oldFlags = CStreaming::ms_aInfoForModel[model].m_nFlags;
        CStreaming::RequestModel(model, GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);
        if (CStreaming::ms_aInfoForModel[model].m_nLoadState == LOADSTATE_LOADED) {
            if (!(oldFlags & GAME_REQUIRED)) {
                CStreaming::SetModelIsDeletable(model);
                CStreaming::SetModelTxdIsDeletable(model);
            }
            return true;
        }
        return false;
    }

    static CVehicle *GetRandomCar(float x1, float y1, float x2, float y2) {
        std::vector<CVehicle *> vehicles;
        for (auto vehicle : CPools::ms_pVehiclePool) {
            if (vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE && vehicle->m_pDriver && vehicle->m_placement.IsWithinArea(x1, y1, x2, y2))
                vehicles.push_back(vehicle);
        }
        return vehicles.empty() ? nullptr : vehicles[plugin::Random(0, vehicles.size() - 1)];
    }

    static void __stdcall ConstructorCopPed(CCopPed *cop, eCopType copType, int type) {
        int copModel; 

        cop->m_copType = copType;
        switch (copType) {
        default:
        case COP_TYPE_CITYCOP:
            if (isCop) {
                copModel = CStreaming::GetDefaultCopModel(); isCop = false;
            }
            else {
                copModel = GetAdditionalCopModel(); isCop = true;
            }
            goto LABEL_COP;
        case COP_TYPE_CSHER:
            copModel = MODEL_CSHER;
        LABEL_COP:
            cop->SetModelIndex(copModel);
            currentWeaponType = GetCurrentWeaponType(0);
            weaponAmmo = GetWeaponAmmo(currentWeaponType);
            cop->GiveWeapon(currentWeaponType, weaponAmmo, true);
            cop->GiveWeapon(WEAPON_NIGHTSTICK, 1000, true);
            cop->GiveWeapon(WEAPON_PISTOL, 1000, true);
            CPlayerPed *player = FindPlayerPed();
            if (player) {
                if (player->m_pWanted->m_nWantedLevel > 1)
                    cop->SetCurrentWeapon(currentWeaponType);
                else
                    cop->m_nActiveWeaponSlot = 0;
            }
            else
                cop->m_nActiveWeaponSlot = 0;
            cop->m_fArmour = 0.0f;
            cop->m_nWepSkills = 30;
            cop->m_nWeaponAccuracy = 60;
            break;
        case COP_TYPE_LAPDM1:
            if (isCopbiker) {
                copModel = CStreaming::ms_DefaultCopBikerModel; isCopbiker = false;
            }
            else {
                copModel = GetCurrentPedModel(CopBiker_IDs, TYPE_COPBIKER); isCopbiker = true;
            }
            cop->SetModelIndex(copModel);
            cop->GiveWeapon(WEAPON_NIGHTSTICK, 1000, 1);
            cop->GiveDelayedWeapon(WEAPON_PISTOL, 1000);
            cop->m_nActiveWeaponSlot = 0;
            cop->m_fArmour = 0.0f;
            cop->m_nWepSkills = 30;
            cop->m_nWeaponAccuracy = 60;
            break;
        case COP_TYPE_SWAT1:
        case COP_TYPE_SWAT2:
            if (isSwat) {
                copModel = MODEL_SWAT; isSwat = false;
            }
            else {
                copModel = GetCurrentPedModel(Swat_IDs, TYPE_SWAT); isSwat = true;
            }
            cop->SetModelIndex(copModel);
            currentWeaponType = GetCurrentWeaponType(1);
            weaponAmmo = GetWeaponAmmo(currentWeaponType);
            cop->GiveWeapon(currentWeaponType, weaponAmmo, true);
            cop->GiveWeapon(WEAPON_MICRO_UZI, 1000, true);
            cop->SetCurrentWeapon(currentWeaponType);
            cop->m_fArmour = 50.0f;
            cop->m_nWepSkills = 70;
            cop->m_nWeaponAccuracy = 68;
            break;
        case COP_TYPE_FBI:
            if (isFbi) {
                copModel = MODEL_FBI; isFbi = false;
            }
            else {
                copModel = GetCurrentPedModel(Fbi_IDs, TYPE_FBI); isFbi = true;
            }
            cop->SetModelIndex(copModel);
            currentWeaponType = GetCurrentWeaponType(2);
            weaponAmmo = GetWeaponAmmo(currentWeaponType);
            cop->GiveWeapon(currentWeaponType, weaponAmmo, true);
            cop->GiveWeapon(WEAPON_MP5, 1000, true);
            cop->SetCurrentWeapon(currentWeaponType);
            cop->m_fArmour = 100.0f;
            cop->m_nWepSkills = 60;
            cop->m_nWeaponAccuracy = 76;
            break;
        case COP_TYPE_ARMY:
            if (isArmy) {
                copModel = MODEL_ARMY; isArmy = false;
            }
            else {
                copModel = GetCurrentPedModel(Army_IDs, TYPE_ARMY); isArmy = true;
            }
            cop->SetModelIndex(copModel);
            currentWeaponType = GetCurrentWeaponType(3);
            weaponAmmo = GetWeaponAmmo(currentWeaponType);
            cop->GiveWeapon(currentWeaponType, weaponAmmo, true);
            cop->GiveWeapon(WEAPON_M4, 1000, true);
            cop->SetCurrentWeapon(currentWeaponType);
            cop->m_fArmour = 100.0f;
            cop->m_nWepSkills = 80;
            cop->m_nWeaponAccuracy = 84;
            break;
        }
    }


    AddSpecialCars() {
        std::ifstream stream(PLUGIN_PATH("SpecialCars.dat"));
        if (!stream.is_open())
            return;
        for (string line; getline(stream, line); ) {
            if (!line.compare("police")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Police_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("fbiranch")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Fbiranch_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("enforcer")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Enforcer_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("taxi")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Taxi_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("ambulan")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Ambulan_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("firetruk")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Firetruk_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("barracks")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Barracks_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("cop")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Cop_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("swat")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Swat_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("fbi")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Fbi_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("army")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Army_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("swatweapon")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        SwatWeapon_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("fbiweapon")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        FbiWeapon_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("armyweapon")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        ArmyWeapon_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("emergency")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        m_nEmergencyServices = stoi(line);
                }
            }
        }
        
        patch::RedirectJump(0x5A7F90, DoesMilitaryCraneHaveThisOneAlready);
        patch::RedirectJump(0x5A7EF0, RegisterCarForMilitaryCrane);
        patch::RedirectJump(0x5A8040, DoesCranePickUpThisCarType);
        patch::RedirectJump(0x5B7D60, IsLawEnforcementVehicle);
        patch::RedirectJump(0x419BB0, AddPoliceCarOccupants);
        patch::RedirectJump(0x426850, ChoosePoliceCarModel);
        patch::RedirectJump(0x5B8520, UsesSiren);

        patch::RedirectJump(0x58BE1F, Patch_58BE1F);
        patch::RedirectJump(0x53A905, Patch_53A905);
        patch::RedirectJump(0x5945CE, Patch_5945CE);
        patch::RedirectJump(0x444034, Patch_444034);

        patch::RedirectJump(0x4ED743, Patch_4ED743);
        patch::RedirectJump(0x4ED7C2, Patch_4ED7C2);
        patch::RedirectJump(0x4ED811, Patch_4ED811);
        patch::RedirectJump(0x4ED833, Patch_4ED833);

        patch::RedirectCall(0x45600E, OpcodePlayerDrivingTaxiVehicle);
        patch::Nop(0x456013, 0x5B); // or jump 0x45606E
        patch::RedirectCall(0x44525F, OpcodeIsPlayerInModel);
        patch::Nop(0x445264, 0x46); // or jump 0x4452AA
        
        Events::gameProcessEvent += [] {
            CPlayerPed *player = FindPlayerPed();
            if (player) {
                if (CTimer::m_snTimeInMilliseconds > (randomRoadBlocksTime + 30000)) {
                    randomRoadBlocksTime = CTimer::m_snTimeInMilliseconds;
                    if (player->m_pWanted->m_nWantedLevel == 3) {
                        unsigned int policeId = GetRandomModel(Police_IDs); 
                        if (CModelInfo::IsCarModel(policeId) && LoadModel(policeId)) {
                            if (patch::GetShort(0x4436F1) == MODEL_POLICE)
                                patch::SetShort(0x4436F1, policeId, true);
                            else
                                patch::SetShort(0x4436F1, MODEL_POLICE, true);
                        }
                    }
                    // armyRoadBlocks
                    if (player->m_pWanted->AreArmyRequired()) {
                        unsigned int barracksId = GetRandomModel(Barracks_IDs); 
                        if (CModelInfo::IsCarModel(barracksId) && LoadModel(barracksId)) {
                            if (patch::GetShort(0x4436A4) == MODEL_BARRACKS)
                                patch::SetShort(0x4436A4, barracksId, true);
                            else
                                patch::SetShort(0x4436A4, MODEL_BARRACKS, true);
                        }
                    }
                    // swatRoadBlocks
                    if (player->m_pWanted->AreSwatRequired()) {
                        unsigned int enforcerId = GetRandomModel(Enforcer_IDs); 
                        if (CModelInfo::IsCarModel(enforcerId) && LoadModel(enforcerId)) {
                            if (patch::GetShort(0x4436E5) == MODEL_ENFORCER)
                                patch::SetShort(0x4436E5, enforcerId, true);
                            else
                                patch::SetShort(0x4436E5, MODEL_ENFORCER, true);
                        }
                    }
                    // fbiRoadBlocks
                    if (player->m_pWanted->AreFbiRequired()) {
                        unsigned int fbiranchId = GetRandomModel(Fbiranch_IDs); 
                        if (CModelInfo::IsCarModel(fbiranchId) && LoadModel(fbiranchId)) {
                            if (patch::GetShort(0x4436C5) == MODEL_FBIRANCH)
                                patch::SetShort(0x4436C5, fbiranchId, true);
                            else
                                patch::SetShort(0x4436C5, MODEL_FBIRANCH, true);
                        }
                    }
                }
                // Spawn Cars
                if (m_nEmergencyServices) {
                    unsigned int ambulanId, firetrukId;
                    switch (m_currentState) {
                    case STATE_FIND:
                        if (CTimer::m_snTimeInMilliseconds > (spawnCarTime + 100000) && !CTheScripts::IsPlayerOnAMission()) {
                            CVector onePoint = player->TransformFromObjectSpace(CVector(20.0f, 150.0f, 0.0f));
                            CVector twoPoint = player->TransformFromObjectSpace(CVector(-20.0f, 70.0f, 0.0f));
                            CVehicle *car = GetRandomCar(onePoint.x, onePoint.y, twoPoint.x, twoPoint.y);
                            if (car) {
                                carPos = car->m_placement.pos;
                                carAngle = car->GetHeading() / 57.295776f;
                                pilot = car->m_autoPilot;
                                m_currentState = STATE_WAIT;
                            }
                        }

                        break;
                    case STATE_WAIT:
                        if (DistanceBetweenPoints(player->GetPosition(), carPos) < 170.0f) {
                            CVector cornerA, cornerB;
                            cornerA.x = carPos.x - 5.0f;
                            cornerA.y = carPos.y - 7.0f;
                            cornerA.z = carPos.z - 3.0f;
                            cornerB.x = carPos.x + 5.0f;
                            cornerB.y = carPos.y + 7.0f;
                            cornerB.z = carPos.z + 3.0f;
                            outCount = 1;
                            CWorld::FindObjectsIntersectingCube(cornerA, cornerB, &outCount, 2, 0, 0, 1, 1, 1, 0);
                            if (outCount == 0 && (DistanceBetweenPoints(player->GetPosition(), carPos) > 70.0f))
                                m_currentState = STATE_CREATE;
                        }
                        else
                            m_currentState = STATE_FIND;
                        break;
                    case STATE_CREATE:
                        int modelCar, modelPed;
                        if (randomEmergencyModel < 3)
                            randomEmergencyModel++;
                        else
                            randomEmergencyModel = 0;
                        switch (randomEmergencyModel) {
                        case 0:
                            ambulanId = GetRandomModel(Ambulan_IDs);
                            if (CModelInfo::IsCarModel(ambulanId) && LoadModel(ambulanId))
                                modelCar = ambulanId;
                            else
                                modelCar = MODEL_AMBULAN;
                            modelPed = MODEL_MEDIC;
                            break;
                        case 1:
                            firetrukId = GetRandomModel(Firetruk_IDs);
                            if (CModelInfo::IsCarModel(firetrukId) && LoadModel(firetrukId))
                                modelCar = firetrukId;
                            else
                                modelCar = MODEL_FIRETRUK;
                            modelPed = MODEL_FIREMAN;
                            break;
                        case 2: modelCar = MODEL_AMBULAN; modelPed = MODEL_MEDIC; break;
                        case 3: modelCar = MODEL_FIRETRUK; modelPed = MODEL_FIREMAN; break;
                        default: modelCar = MODEL_AMBULAN; modelPed = MODEL_MEDIC; break;
                        }
                        if (LoadModel(modelCar) && LoadModel(modelPed)) {
                            CVehicle *vehicle = nullptr;
                            vehicle = new CAutomobile(modelCar, 1);
                            if (vehicle) {
                                spawnCarTime = CTimer::m_snTimeInMilliseconds;
                                vehicle->SetPosition(carPos);
                                vehicle->m_placement.SetHeading(carAngle);
                                vehicle->m_nState = 4;
                                CWorld::Add(vehicle);
                                CTheScripts::ClearSpaceForMissionEntity(carPos, vehicle);
                                reinterpret_cast<CAutomobile *>(vehicle)->PlaceOnRoadProperly();
                                if (modelPed == MODEL_MEDIC)
                                    CCarAI::AddAmbulanceOccupants(vehicle);
                                else
                                    CCarAI::AddFiretruckOccupants(vehicle);
                                Command<COMMAND_CAR_GOTO_COORDINATES>(CPools::GetVehicleRef(vehicle), 0.0f, 0.0f, 0.0f);
                                vehicle->m_autoPilot = pilot;
                                if (plugin::Random(0, 1)) {
                                    vehicle->m_nSirenOrAlarm = true;
                                    vehicle->m_autoPilot.m_nDrivingStyle = DRIVINGSTYLE_AVOID_CARS;
                                    vehicle->m_autoPilot.m_nCruiseSpeed = 25;
                                }
                                else
                                    vehicle->m_nSirenOrAlarm = false;
                            }
                        }
                        m_currentState = STATE_FIND;
                        break;
                    }
                }
            }
        };

        //Events::drawingEvent += [] {
        //    gamefont::Print({
        //        Format("armyCarBlok = %d", patch::GetShort(0x4436A4)),
        //        Format("swatCarBlok = %d", patch::GetShort(0x4436E5)),
        //        Format("fbiCarBlok = %d", patch::GetShort(0x4436C5)),
        //        Format("copCarBlok = %d", patch::GetShort(0x4436F1))
        //        //Format("swatBlok = %d, %d", patch::GetShort(0x5DDD90), patch::GetShort(0x461339)),
        //        //Format("fbiBlok = %d, %d", patch::GetShort(0x5DDDD0), patch::GetShort(0x461353)),
        //        //Format("armyBlok = %d, %d", patch::GetShort(0x5DDE10), patch::GetShort(0x46136D))
        //    }, 10, 300, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);
        //};

    }
} test;

AddSpecialCars::eSpawnCarState AddSpecialCars::m_currentState = STATE_FIND;
short AddSpecialCars::outCount = 0;
CVector AddSpecialCars::carPos = { 0.0f, 0.0f, 0.0f };
float AddSpecialCars::carAngle = 0.0f;
CAutoPilot AddSpecialCars::pilot;
unsigned int AddSpecialCars::currentSpecialModelForSiren;
unsigned int AddSpecialCars::currentSpecialModelForOccupants;
unsigned int AddSpecialCars::currentFiretrukModel;
unsigned int AddSpecialCars::currentRoadBlockModel;
int AddSpecialCars::currentCopModel;
int AddSpecialCars::currentSwatModel;
int AddSpecialCars::currentFbiModel;
int AddSpecialCars::currentArmyModel;
int AddSpecialCars::currentWeaponModel;
unsigned int AddSpecialCars::jmp_53A913;
unsigned int AddSpecialCars::jmp_5945D9;
unsigned int AddSpecialCars::jmp_444040;

void __declspec(naked) AddSpecialCars::Patch_58BE1F() { // Siren
    __asm {
        movsx eax, word ptr[ebp + 0x5C]
        pushad
        push eax
        call GetSpecialModelForSiren
        mov currentSpecialModelForSiren, eax
        popad
        mov eax, currentSpecialModelForSiren
        mov ecx, eax
        mov edx, 0x58BE25
        jmp edx
    }
}

void __declspec(naked) AddSpecialCars::Patch_53A905() { // AddPedInCar
    __asm {
        movsx edx, word ptr[eax + 5Ch]
        mov eax, edx
        pushad
        push eax
        call GetSpecialModelForOccupants
        mov currentSpecialModelForOccupants, eax
        popad
        mov eax, currentSpecialModelForOccupants
        pop ecx
        mov edi, eax
        sub eax, 137
        mov jmp_53A913, 0x53A913
        jmp jmp_53A913
    }
}

void __declspec(naked) AddSpecialCars::Patch_5945CE() { // FireTruckControl
    __asm {
        movsx eax, word ptr[ebp + 0x5C]
        pushad
        push eax
        call GetFiretrukModel
        mov currentFiretrukModel, eax
        popad
        mov eax, currentFiretrukModel
        fldz
        cmp  eax, 137
        mov jmp_5945D9, 0x5945D9
        jmp jmp_5945D9
    }
}

void __declspec(naked) AddSpecialCars::Patch_444034() { // RoadBlockCopsForCar
    __asm {
        movsx   eax, word ptr[ebx + 0x5C]
        pushad
        push eax
        call GetRoadBlockModel
        mov currentRoadBlockModel, eax
        popad
        mov eax, currentRoadBlockModel
        add     esp, 12
        sub     eax, 157
        mov jmp_444040, 0x444040
        jmp jmp_444040
    }
}

void __declspec(naked) AddSpecialCars::Patch_4ED743() { // CCopPed::CCopPed
    __asm {
        mov  edx, [esp + 20]
        pushad
        push edx
        push ebx
        push eax
        call ConstructorCopPed
        popad
        mov edx, 0x4ED8F5
        jmp edx
    }
}

void __declspec(naked) AddSpecialCars::Patch_4ED7C2() { // RandomSwat
    __asm {
        pushad
        call GetCurrentSwatModel
        mov currentSwatModel, eax
        popad
        push currentSwatModel
        call dword ptr[ebx + 0Ch]
        pushad
        push 1
        call GetCurrentWeaponModel
        mov currentWeaponModel, eax
        popad
        mov ecx, [esp + 4]
        push 1
        push 1000
        push currentWeaponModel
        call CPed::GiveWeapon
        mov ecx, [esp + 4]
        push 1
        push 7
        push 17
        call CPed::GiveWeapon
        mov ecx, [esp + 4]
        push 17
        mov edx, 0x4ED7DD
        jmp edx
    }
}

void __declspec(naked) AddSpecialCars::Patch_4ED811() { // RandomFbi
    __asm {
        pushad
        call GetCurrentFbiModel
        mov currentFbiModel, eax
        popad
        push currentFbiModel
        call dword ptr[ebx + 0Ch]
        pushad
        push 2
        call GetCurrentWeaponModel
        mov currentWeaponModel, eax
        popad
        mov ecx, [esp + 4]
        push 1
        push 1000
        push currentWeaponModel
        call CPed::GiveWeapon
        mov ecx, [esp + 4]
        push 1
        push 7
        push 17
        call CPed::GiveWeapon
        push 17
        mov edx, 0x4ED828
        jmp edx
    }
}

void __declspec(naked) AddSpecialCars::Patch_4ED833() { // RandomArmy
    __asm {
        pushad
        call GetCurrentArmyModel
        mov currentArmyModel, eax
        popad
        push currentArmyModel
        call dword ptr[ebx + 0Ch]
        pushad
        push 3
        call GetCurrentWeaponModel
        mov currentWeaponModel, eax
        popad
        mov ecx, [esp + 4]
        push 1
        push 1000
        push currentWeaponModel
        call CPed::GiveWeapon
        mov ecx, [esp + 4]
        push 1
        push 7
        push 17
        call CPed::GiveWeapon
        mov ecx, [esp + 4]
        push 17
        mov edx, 0x4ED84E
        jmp edx
    }
}

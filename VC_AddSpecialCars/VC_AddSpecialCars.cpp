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
#include "eEntityStatus.h"

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

int m_nEmergencyServices, m_nTime;

unordered_set<unsigned int>
/*cop vehicles*/            Police_IDs, Fbiranch_IDs, Enforcer_IDs, Barracks_IDs,
/*cop peds*/                Cop_IDs, Swat_IDs, Fbi_IDs, Army_IDs,
/*cop weapons*/             SwatWeapon_IDs, FbiWeapon_IDs, ArmyWeapon_IDs,
/*taxi cars*/               Taxi_IDs,
/*emergency cars*/          Ambulan_IDs, Firetruk_IDs,
/*emergency peds*/          Medic_IDs, Fireman_IDs;

bool isCop = false, isSwat = false, isFbi = false, isArmy = false, isMedic = false, isFireman = false,
isAmbulanceModel = false, isAmbulan = false, isFiretruk = false, isEnforcer = true, isFbiranch = true, isBarracks = true;

int randomFbicar = 2, randomRoadBlocksTime = 0, m_nGenerateEmergencyServicesTime = 0, medicModel, firemanModel;

class AddSpecialCars {
public:
    enum eSpawnCarState { STATE_FIND, STATE_WAIT, STATE_CREATE };
    enum eSpecialModel { TYPE_AMBULAN, TYPE_MEDIC, TYPE_FIRETRUK, TYPE_FIREMAN, TYPE_COP, TYPE_SWAT, TYPE_FBI, TYPE_ARMY };

    static eSpawnCarState m_currentState;
    static short outCount;
    static CVector carPos;
    static float carAngle;
    static CAutoPilot pilot;
    static unsigned int currentSpecialModelForSiren, currentSpecialModelForOccupants, currentFiretrukModel, currentRoadBlockModel;
    static unsigned int jmp_53A913, jmp_5945D9, jmp_444040;

    static eWeaponType currentWeaponType;

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
        else if (model == MODEL_BARRACKS || model == MODEL_PATRIOT || Barracks_IDs.find(model) != Barracks_IDs.end())
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

    static int __stdcall GetSpecialModel(eSpecialModel type) {
        int result;
        switch (type) {
        case TYPE_AMBULAN:  result = MODEL_AMBULAN;  break;
        case TYPE_MEDIC:    result = MODEL_MEDIC;    break;
        case TYPE_FIRETRUK: result = MODEL_FIRETRUK; break;
        case TYPE_FIREMAN:  result = MODEL_FIREMAN;  break;
        case TYPE_COP:      result = MODEL_COP;      break;
        case TYPE_SWAT:     result = MODEL_SWAT;     break;
        case TYPE_FBI:      result = MODEL_FBI;      break;
        case TYPE_ARMY:     result = MODEL_ARMY;     break;
        }
        return result;
    }

    static int __stdcall GetCurrentPedModel(unordered_set<unsigned int> IDs, eSpecialModel type) {
        if (IDs.size()) {
            unsigned int pedId = GetRandomModel(IDs);
            if (CModelInfo::IsPedModel(pedId)) {
                if (CStreaming::ms_aInfoForModel[pedId].m_nLoadState == LOADSTATE_LOADED)
                    return pedId;
                else {
                    if (LoadModel(pedId))
                        return pedId;
                    else
                        return GetSpecialModel(type);
                }
            }
            else
                return GetSpecialModel(type);
        }
        else
            return GetSpecialModel(type);
    }

    static int __stdcall GetCurrentVehicleModel(unordered_set<unsigned int> IDs, eSpecialModel type) {
        int vehicleId = GetRandomModel(IDs);
        if (CModelInfo::IsCarModel(vehicleId)) {
            if (CStreaming::ms_aInfoForModel[vehicleId].m_nLoadState == LOADSTATE_LOADED)
                return vehicleId;
            else {
                if (LoadModel(vehicleId))
                    return vehicleId;
                else
                    return GetSpecialModel(type);
            }
        }
        else
            return GetSpecialModel(type);
    }

    static void Patch_58BE1F(); // Siren
    static void Patch_53A905(); // AddPedInCar
    static void Patch_5945CE(); // FireTruckControl
    static void Patch_444034(); // RoadBlockCopsForCar
    static void Patch_4ED743(); // RandomCop
    static void Patch_4EEB73(); // RandomMedic
    static void Patch_4EEB9C(); // RandomFireman

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
            case MODEL_PATRIOT:
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
                            int randomArmy = plugin::Random(0, 3);
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
                            case 3: 
                                if (LoadModel(MODEL_PATRIOT))
                                    return MODEL_PATRIOT;
                                else
                                    return MODEL_BARRACKS;
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

    // CVehicle::SetDriver
    static void __fastcall SetDriver(CVehicle *_this, int, CPed *driver) {
        _this->m_pDriver = driver;
        _this->m_pDriver->RegisterReference((CEntity**)&_this->m_pDriver);

        if (_this->m_nVehicleFlags.b08 && driver == FindPlayerPed()) {
            _this->m_nVehicleFlags.b08 = false; //bFreebies
            unsigned int model = driver->m_pVehicle->m_nModelIndex;
            if (model == MODEL_AMBULAN || Ambulan_IDs.find(model) != Ambulan_IDs.end())
                FindPlayerPed()->m_fHealth = max(FindPlayerPed()->m_fHealth, min(FindPlayerPed()->m_fHealth + 20.0f, CWorld::Players[0].m_nMaxHealth));
            else if (model == MODEL_TAXI || model == MODEL_CABBIE || model == MODEL_ZEBRA || model == MODEL_KAUFMAN || Taxi_IDs.find(model) != Taxi_IDs.end())
                CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 12;
            else if (model == MODEL_ENFORCER || Enforcer_IDs.find(model) != Enforcer_IDs.end())
                driver->m_fArmour = max(driver->m_fArmour, CWorld::Players[0].m_nMaxArmour);
            else if (model == MODEL_CADDY) {
                if (!(driver->IsPlayer() && ((CPlayerPed*)driver)->DoesPlayerWantNewWeapon(WEAPONTYPE_GOLFCLUB, true)))
                    CStreaming::RequestModel(MODEL_GOLFCLUB, 1);
            }
            else if (model == MODEL_POLICE || Police_IDs.find(model) != Police_IDs.end()) {
                CStreaming::RequestModel(MODEL_CHROMEGUN, 1);
                _this->m_nVehicleFlags.b08 = true;
            }
        }

        if (_this->m_nVehicleClass == VEHICLE_BIKE)
            _this->ApplyMoveForce(-0.02f*driver->m_fMass * _this->m_placement.up.x, 
                                  -0.02f*driver->m_fMass * _this->m_placement.up.y,
                                  -0.02f*driver->m_fMass * _this->m_placement.up.z);
        else
            _this->ApplyTurnForce(0.0f, 0.0f, -0.02f*driver->m_fMass,
                driver->GetPosition().x - _this->GetPosition().x,
                driver->GetPosition().y - _this->GetPosition().y,
                0.0f);
    }

    // CVehicle::RemoveDriver
    static void __fastcall RemoveDriver(CVehicle *_this) {

        _this->m_nState = STATUS_ABANDONED;
        if (_this->m_pDriver == FindPlayerPed()) {
            unsigned int model = _this->m_nModelIndex;
            if ((model == MODEL_POLICE || Police_IDs.find(model) != Police_IDs.end()) && CStreaming::ms_aInfoForModel[MODEL_CHROMEGUN].m_nLoadState == LOADSTATE_LOADED) {
                if (_this->m_nVehicleFlags.b08) {
                    if (((CPlayerPed*)_this->m_pDriver)->DoesPlayerWantNewWeapon(WEAPONTYPE_SHOTGUN, true))
                        _this->m_pDriver->GiveWeapon(WEAPONTYPE_SHOTGUN, 5, true);
                    else
                        _this->m_pDriver->GrantAmmo(WEAPONTYPE_SHOTGUN, 5);
                    _this->m_nVehicleFlags.b08 = false; //bFreebies
                }
                CStreaming::SetModelIsDeletable(MODEL_CHROMEGUN);
            }
            else if (model == MODEL_CADDY && CStreaming::ms_aInfoForModel[MODEL_GOLFCLUB].m_nLoadState == LOADSTATE_LOADED) {
                if (((CPlayerPed*)_this->m_pDriver)->DoesPlayerWantNewWeapon(WEAPONTYPE_GOLFCLUB, true))
                    _this->m_pDriver->GiveWeapon(WEAPONTYPE_GOLFCLUB, 1, true);
                CStreaming::SetModelIsDeletable(MODEL_GOLFCLUB);
            }
        }
        _this->m_pDriver = nullptr;
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
                || CTheScripts::ScriptParams[1].uParam == MODEL_FBIRANCH || CTheScripts::ScriptParams[1].uParam == MODEL_PATRIOT)
            {
                if (model == MODEL_RHINO || model == MODEL_POLICE || Police_IDs.find(model) != Police_IDs.end()
                    || model == MODEL_BARRACKS || model == MODEL_PATRIOT || Barracks_IDs.find(model) != Barracks_IDs.end()
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

    static eWeaponType __stdcall GetCurrentWeaponType(int type) {
        eWeaponType result; unsigned int model;
        switch (type) {
        case 1: model = GetRandomModel(SwatWeapon_IDs); result = WEAPONTYPE_UZI; break;
        case 2: model = GetRandomModel(FbiWeapon_IDs);  result = WEAPONTYPE_MP5; break;
        case 3: model = GetRandomModel(ArmyWeapon_IDs); result = WEAPONTYPE_M4; break;
        }
        switch (model) {
        case MODEL_COLT45:
            if (LoadModel(MODEL_COLT45))
                result = WEAPONTYPE_PISTOL;
            break;
        case MODEL_CHROMEGUN:
            if (LoadModel(MODEL_CHROMEGUN))
                result = WEAPONTYPE_SHOTGUN;
            break;
        case MODEL_SHOTGSPA:
            if (LoadModel(MODEL_SHOTGSPA))
                result = WEAPONTYPE_SPAS12_SHOTGUN;
            break;
        case MODEL_BUDDYSHOT:
            if (LoadModel(MODEL_BUDDYSHOT))
                result = WEAPONTYPE_STUBBY_SHOTGUN;
            break;
        case MODEL_UZI:
            if (LoadModel(MODEL_UZI))
                result = WEAPONTYPE_UZI;
            break;
        case MODEL_MP5LNG:
            if (LoadModel(MODEL_MP5LNG))
                result = WEAPONTYPE_MP5;
            break;
        case MODEL_M4:
            if (LoadModel(MODEL_M4))
                result = WEAPONTYPE_M4;
            break;
        default:
            switch (type) {
            case 1: result = WEAPONTYPE_UZI; break;
            case 2: result = WEAPONTYPE_MP5;  break;
            case 3: result = WEAPONTYPE_M4; break;
            }
        }
        return result;
    }

    static void __stdcall ConstructorCopPed(CCopPed *cop, eCopType copType, int type) {
        int copModel; 
        cop->m_copType = copType;
        switch (copType) {
        default: break;
        case COP_TYPE_COP:
            if (isCop) {
                copModel = MODEL_COP; isCop = false;
            }
            else {
                copModel = GetCurrentPedModel(Cop_IDs, TYPE_COP); isCop = true;
            }
            cop->SetModelIndex(copModel);
            cop->GiveWeapon(WEAPONTYPE_NIGHTSTICK, 1000, true);
            cop->GiveDelayedWeapon(WEAPONTYPE_PISTOL, 1000);
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
            cop->GiveWeapon(currentWeaponType, 1000, true);
            cop->GiveWeapon(WEAPONTYPE_PISTOL, 7, true);
            cop->SetCurrentWeapon(WEAPONTYPE_PISTOL);
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
            cop->GiveWeapon(currentWeaponType, 1000, true);
            cop->GiveWeapon(WEAPONTYPE_PISTOL, 7, true);
            cop->SetCurrentWeapon(WEAPONTYPE_PISTOL);
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
            cop->GiveWeapon(currentWeaponType, 1000, true);
            cop->GiveWeapon(WEAPONTYPE_PISTOL, 7, true);
            cop->SetCurrentWeapon(WEAPONTYPE_PISTOL);
            cop->m_fArmour = 100.0f;
            cop->m_nWepSkills = 80;
            cop->m_nWeaponAccuracy = 84;
            break;
        case COP_TYPE_VICE:
            switch (type) {
            case 0: copModel = MODEL_VICE1;  goto LABEL_TYPE_VICE;
            case 1: copModel = MODEL_VICE2;  goto LABEL_TYPE_VICE;
            case 2: copModel = MODEL_VICE3;  goto LABEL_TYPE_VICE;
            case 3: copModel = MODEL_VICE4;  goto LABEL_TYPE_VICE;
            case 4: copModel = MODEL_VICE5;  goto LABEL_TYPE_VICE;
            case 5: copModel = MODEL_VICE6;  goto LABEL_TYPE_VICE;
            case 6: copModel = MODEL_VICE7;  goto LABEL_TYPE_VICE;
            case 7: copModel = MODEL_VICE8;
            }
            LABEL_TYPE_VICE:
                cop->SetModelIndex(copModel);
                cop->GiveDelayedWeapon(WEAPONTYPE_UZI, 1000);
                cop->SetCurrentWeapon(WEAPONTYPE_UZI);
                cop->m_fArmour = 100.0f;
                cop->m_nWepSkills = 60;
                cop->m_nWeaponAccuracy = 76;
            break;
        }
    }

    static bool __stdcall IsCarSprayable(int model) {
        bool result;
        if (Police_IDs.find(model) != Police_IDs.end()
            || Fbiranch_IDs.find(model) != Fbiranch_IDs.end()
            || Enforcer_IDs.find(model) != Enforcer_IDs.end()
            || Firetruk_IDs.find(model) != Firetruk_IDs.end()
            || Ambulan_IDs.find(model) != Ambulan_IDs.end()
            || Barracks_IDs.find(model) != Barracks_IDs.end())
            return false;

        switch (model) {
        case MODEL_FIRETRUK:
        case MODEL_AMBULAN:
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_BUS:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case MODEL_COACH:
            result = false;
            break;
        default:
            result = true;
            break;
        }
        return result;
    }

    static int __stdcall RandomMedic() {
        int result;
        if (isMedic) {
            result = MODEL_MEDIC; isMedic = false;
        }
        else {
            result = GetCurrentPedModel(Medic_IDs, TYPE_MEDIC); isMedic = true;
        }
        return result;
    }

    static int __stdcall RandomFireman() {
        int result;
        if (isFireman) {
            result = MODEL_FIREMAN; isFireman = false;
        }
        else {
            result = GetCurrentPedModel(Fireman_IDs, TYPE_FIREMAN); isFireman = true;
        }
        return result;
    }

    static int __stdcall RandomAmbulan() {
        int result;
        if (isAmbulan) {
            result = MODEL_AMBULAN; isAmbulan = false;
        }
        else {
            result = GetCurrentVehicleModel(Ambulan_IDs, TYPE_AMBULAN); isAmbulan = true;
        }
        return result;
    }

    static int __stdcall RandomFiretruk() {
        int result;
        if (isFiretruk) {
            result = MODEL_FIRETRUK; isFiretruk = false;
        }
        else {
            result = GetCurrentVehicleModel(Firetruk_IDs, TYPE_FIRETRUK); isFiretruk = true;
        }
        return result;
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
            if (!line.compare("medic")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Medic_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("fireman")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        Fireman_IDs.insert(stoi(line));
                }
            }
            if (!line.compare("emergency")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        m_nEmergencyServices = stoi(line);
                }
            }
            if (!line.compare("time")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        m_nTime = stoi(line);
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

        patch::RedirectJump(0x5B89F0, SetDriver); // update 26.07.2021
        patch::RedirectJump(0x5B8920, RemoveDriver);

        patch::RedirectJump(0x58BE1F, Patch_58BE1F);
        patch::RedirectJump(0x53A905, Patch_53A905);
        patch::RedirectJump(0x5945CE, Patch_5945CE);
        patch::RedirectJump(0x444034, Patch_444034);

        patch::RedirectJump(0x4ED743, Patch_4ED743);
        patch::RedirectJump(0x4EEB73, Patch_4EEB73);
        patch::RedirectJump(0x4EEB9C, Patch_4EEB9C);

        patch::RedirectCall(0x45600E, OpcodePlayerDrivingTaxiVehicle);
        patch::Nop(0x456013, 0x5B); // or jump 0x45606E
        patch::RedirectCall(0x44525F, OpcodeIsPlayerInModel);
        patch::Nop(0x445264, 0x46); // or jump 0x4452AA
        
        patch::SetUChar(0x430CC6, 0x50); // push eax
        patch::RedirectCall(0x430CC7, IsCarSprayable); // call
        patch::RedirectJump(0x430CCC, reinterpret_cast<void *>(0x430CF9)); // jump

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
                    switch (m_currentState) {
                    case STATE_FIND:
                        if (CTimer::m_snTimeInMilliseconds > (m_nGenerateEmergencyServicesTime + m_nTime * 1000) && !CTheScripts::IsPlayerOnAMission()) {
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
                        if (isAmbulanceModel) {
                            modelCar = RandomAmbulan();
                            modelPed = MODEL_MEDIC;
                        }
                        else {
                            modelCar = RandomFiretruk();
                            modelPed = MODEL_FIREMAN;
                        }
                        if (LoadModel(modelCar) && LoadModel(modelPed)) {
                            CVehicle *vehicle = nullptr;
                            vehicle = new CAutomobile(modelCar, 1);
                            if (vehicle) {
                                m_nGenerateEmergencyServicesTime = CTimer::m_snTimeInMilliseconds;
                                vehicle->SetPosition(carPos);
                                vehicle->m_placement.SetHeading(carAngle);
                                vehicle->m_nState = 4;
                                CWorld::Add(vehicle);
                                CTheScripts::ClearSpaceForMissionEntity(carPos, vehicle);
                                reinterpret_cast<CAutomobile *>(vehicle)->PlaceOnRoadProperly();
                                if (isAmbulanceModel) {
                                    CCarAI::AddAmbulanceOccupants(vehicle); isAmbulanceModel = false;
                                }
                                else {
                                    CCarAI::AddFiretruckOccupants(vehicle); isAmbulanceModel = true;
                                }
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
unsigned int AddSpecialCars::jmp_53A913;
unsigned int AddSpecialCars::jmp_5945D9;
unsigned int AddSpecialCars::jmp_444040;
eWeaponType AddSpecialCars::currentWeaponType;

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

void __declspec(naked) AddSpecialCars::Patch_4EEB73() { // Medic
    __asm {
        pushad
        call RandomMedic
        mov medicModel, eax
        popad
        mov eax, medicModel
        push eax
        call dword ptr[ebx + 12]
        mov edx, 0x4EEB78
        jmp edx
    }
}

void __declspec(naked) AddSpecialCars::Patch_4EEB9C() { // Fireman
    __asm {
        pushad
        call RandomFireman
        mov firemanModel, eax
        popad
        mov eax, firemanModel
        push eax
        call dword ptr[ebx + 12]
        mov edx, 0x4EEBA1
        jmp edx
    }
}

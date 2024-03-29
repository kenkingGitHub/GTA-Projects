#include "plugin.h"
#include "CWanted.h"
#include "CStreaming.h"
#include "CWorld.h"
#include "eVehicleModel.h"
#include "ePedModel.h"
#include "cMusicManager.h"
#include "CTheScripts.h"
#include "CCivilianPed.h"
#include "CTimer.h"
#include "CModelInfo.h"
#include "extensions\ScriptCommands.h"
#include "eScriptCommands.h"
#include "CCarAI.h"
#include <unordered_set>
#include <string>
#include <fstream>

#define MODEL_COP_a 83
#define MODEL_COP_b 84
#define MODEL_COP_c 85
#define MODEL_SWAT_a 86
#define MODEL_FBI_a 87
#define MODEL_ARMY_a 88

bool &bReplayEnabled = *(bool *)0x617CAC;

int &/*CCranes::*/CarsCollectedMilitaryCrane = *(int *)0x8F6248;

class CCrane {
public:
    CEntity *m_pObject;
    CObject *m_pMagnet;
    int m_nAudioEntity;
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
    char byte_7A;
    bool m_bIsCrusher;
    bool m_bIsMilitaryCrane;
    bool byte_7D;
    bool m_bNotMilitaryCrane;
    char _pad_7F;
};

using namespace plugin;
using namespace std;

class AddSpecialCars {
public:
    enum eSpawnCarState { STATE_FIND, STATE_WAIT, STATE_CREATE };

    static eSpawnCarState m_currentState;
    static short outCount;
    static CVector carPos;
    static float carAngle;
    static CAutoPilot pilot;
    
    static unsigned int currentSpecialModelForSiren, currentSpecialModelForOccupants;
    static unsigned int randomPolice, currentFiretrukModel, currentRoadBlockModel;
    static unsigned int jmp_5373DE, jmp_4F5860, jmp_531FF1, jmp_4378D1, jmp_56C23A, jmp_56C4B3;
    static unsigned int currentModel_Patch_56C234, currentModel_Patch_56C4AD;

    static unordered_set<unsigned int> &GetPoliceModels() {
        static std::unordered_set<unsigned int> policeIds;
        return policeIds;
    }

    static unordered_set<unsigned int> &GetFbicarModels() {
        static std::unordered_set<unsigned int> fbicarIds;
        return fbicarIds;
    }

    static unordered_set<unsigned int> &GetEnforcerModels() {
        static unordered_set<unsigned int> enforcerIds;
        return enforcerIds;
    }

    static unordered_set<unsigned int> &GetTaxiModels() {
        static std::unordered_set<unsigned int> taxiIds;
        return taxiIds;
    }

    static unordered_set<unsigned int> &GetAmbulanModels() {
        static unordered_set<unsigned int> ambulanIds;
        return ambulanIds;
    }

    static unordered_set<unsigned int> &GetFiretrukModels() {
        static unordered_set<unsigned int> firetrukIds;
        return firetrukIds;
    }

    static unordered_set<unsigned int> &GetBarracksModels() {
        static unordered_set<unsigned int> barracksIds;
        return barracksIds;
    }

    static unordered_set<unsigned int> &GetCopModels() {
        static unordered_set<unsigned int> copIds;
        return copIds;
    }

    static unordered_set<unsigned int> &GetSwatModels() {
        static unordered_set<unsigned int> swatIds;
        return swatIds;
    }

    static unordered_set<unsigned int> &GetFbiModels() {
        static unordered_set<unsigned int> fbiIds;
        return fbiIds;
    }

    static unordered_set<unsigned int> &GetArmyModels() {
        static unordered_set<unsigned int> armyIds;
        return armyIds;
    }

    static unsigned int GetRandomPolice() {
        vector<unsigned int> ids;
        for (auto id : GetPoliceModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomFbicar() {
        vector<unsigned int> ids;
        for (auto id : GetFbicarModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomEnforcer() {
        vector<unsigned int> ids;
        for (auto id : GetEnforcerModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomBarracks() {
        vector<unsigned int> ids;
        for (auto id : GetBarracksModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomAmbulance() {
        vector<unsigned int> ids;
        for (auto id : GetAmbulanModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomFiretruk() {
        vector<unsigned int> ids;
        for (auto id : GetFiretrukModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomCop() {
        vector<unsigned int> ids;
        for (auto id : GetCopModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomSwat() {
        vector<unsigned int> ids;
        for (auto id : GetSwatModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomFbi() {
        vector<unsigned int> ids;
        for (auto id : GetFbiModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static unsigned int GetRandomArmy() {
        vector<unsigned int> ids;
        for (auto id : GetArmyModels())
            ids.push_back(id);
        return ids.empty() ? 0 : ids[plugin::Random(0, ids.size() - 1)];
    }

    static int __stdcall GetSpecialModelForSiren(unsigned int model) {
        if (model == MODEL_POLICE || GetPoliceModels().find(model) != GetPoliceModels().end())
            return MODEL_POLICE;
        else if (model == MODEL_TAXI || GetTaxiModels().find(model) != GetTaxiModels().end())
            return MODEL_TAXI;
        else if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end())
            return MODEL_AMBULAN;
        else if (model == MODEL_FBICAR || GetFbicarModels().find(model) != GetFbicarModels().end())
            return MODEL_FBICAR;
        else if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end())
            return MODEL_FIRETRUK;
        else if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
            return MODEL_ENFORCER;
        return model;
    }

    static int __stdcall GetSpecialModelForOccupants(unsigned int model) {
        if (model == MODEL_POLICE || GetPoliceModels().find(model) != GetPoliceModels().end())
            return MODEL_POLICE;
        else if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end())
            return MODEL_AMBULAN;
        else if (model == MODEL_FBICAR || GetFbicarModels().find(model) != GetFbicarModels().end())
            return MODEL_FBICAR;
        else if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end())
            return MODEL_FIRETRUK;
        else if (model == MODEL_BARRACKS || GetBarracksModels().find(model) != GetBarracksModels().end())
            return MODEL_BARRACKS;
        else if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
            return MODEL_ENFORCER;
        return model;
    }

    static int __stdcall GetFiretrukModel(unsigned int model) {
        if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end())
            return MODEL_FIRETRUK;
        return model;
    }

    static int __stdcall GetRoadBlockModel(unsigned int model) {
        if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
            return MODEL_ENFORCER;
        else if (model == MODEL_FBICAR || GetFbicarModels().find(model) != GetFbicarModels().end())
            return MODEL_FBICAR;
        else if (model == MODEL_BARRACKS || GetBarracksModels().find(model) != GetBarracksModels().end())
            return MODEL_BARRACKS;
        return model;
    }

    static void Patch_5373D7(); // Siren
    static void Patch_4F5857(); // AddPedInCar
    static void Patch_531FE8(); // FireTruckControl
    static void Patch_4378C4(); // RoadBlockCopsForCar
    static void Patch_56C234();
    static void Patch_56C4AD();

    static bool __stdcall GetModel_Patch_56C234(unsigned int model) {
        bool result; result = false;
        if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end())
            result = true;
        else if (model == MODEL_PREDATOR || model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
            result = true;
        else if (model == MODEL_POLICE || GetPoliceModels().find(model) != GetPoliceModels().end())
            result = true;
        return result;
    }

    static bool __stdcall GetModel_Patch_56C4AD(unsigned int model) {
        bool result; result = false;
        if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end())
            result = true;
        else if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end())
            result = true;
        else if (model == MODEL_FBICAR || GetFbicarModels().find(model) != GetFbicarModels().end())
            result = true;
        else if (model == MODEL_PREDATOR || model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
            result = true;
        else if (model == MODEL_POLICE || GetPoliceModels().find(model) != GetPoliceModels().end())
            result = true;
        return result;
    }

    // CCranes::DoesMilitaryCraneHaveThisOneAlready
    static bool __cdecl DoesMilitaryCraneHaveThisOneAlready(int model) {
        if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end()) {
            if (!(CarsCollectedMilitaryCrane & 1))
                return false;
            return true;
        }
        else if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end()) {
            if (!(CarsCollectedMilitaryCrane & 2))
                return false;
            return true;
        }
        else if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end()) {
            if (!(CarsCollectedMilitaryCrane & 4))
                return false;
            return true;
        }
        else if (model == MODEL_FBICAR || GetFbicarModels().find(model) != GetFbicarModels().end()) {
            if (!(CarsCollectedMilitaryCrane & 8))
                return false;
            return true;
        }
        else if (model == MODEL_RHINO) {
            if (!(CarsCollectedMilitaryCrane & 0x10))
                return false;
            return true;
        }
        else if (model == MODEL_BARRACKS || GetBarracksModels().find(model) != GetBarracksModels().end()) {
            if (!(CarsCollectedMilitaryCrane & 0x20))
                return false;
            return true;
        }
        else if (model == MODEL_POLICE || GetPoliceModels().find(model) != GetPoliceModels().end()) {
            if (!(CarsCollectedMilitaryCrane & 0x40))
                return false;
            return true;
        }
        return false;
    }

    // CCranes::RegisterCarForMilitaryCrane
    static void __cdecl RegisterCarForMilitaryCrane(int model) {
        if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end()) 
            CarsCollectedMilitaryCrane |= 1;
        else if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end())
            CarsCollectedMilitaryCrane |= 2;
        else if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
            CarsCollectedMilitaryCrane |= 4;
        else if (model == MODEL_FBICAR || GetFbicarModels().find(model) != GetFbicarModels().end())
            CarsCollectedMilitaryCrane |= 8;
        else if (model == MODEL_RHINO)
            CarsCollectedMilitaryCrane |= 0x10;
        else if (model == MODEL_BARRACKS || GetBarracksModels().find(model) != GetBarracksModels().end())
            CarsCollectedMilitaryCrane |= 0x20;
        else if (model == MODEL_POLICE || GetPoliceModels().find(model) != GetPoliceModels().end())
            CarsCollectedMilitaryCrane |= 0x40;
    }

    // CCrane::DoesCranePickUpThisCarType
    static bool __fastcall DoesCranePickUpThisCarType(CCrane *_this, int, int model) {
        bool result; 

        if (_this->m_bIsCrusher) {
            result = model != MODEL_FIRETRUK && !(GetFiretrukModels().find(model) != GetFiretrukModels().end())
                && model != MODEL_TRASH && model != MODEL_BLISTA
                && model != MODEL_SECURICA && model != MODEL_BUS
                && model != MODEL_DODO && model != MODEL_RHINO;
        }
        else if (_this->m_bIsMilitaryCrane) {
            result = model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end()
                || model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end()
                || model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end()
                || model == MODEL_FBICAR || GetFbicarModels().find(model) != GetFbicarModels().end()
                || model == MODEL_RHINO || model == MODEL_BARRACKS || model == MODEL_POLICE
                || GetBarracksModels().find(model) != GetBarracksModels().end()
                || GetPoliceModels().find(model) != GetPoliceModels().end();
        }
        else
            result = true;
        return result;
    }

    // CVehicle::IsVehicleNormal
    static bool __fastcall IsVehicleNormal(CVehicle *_this) {
        bool result;  result = false;
        if (_this->m_pDriver && !_this->m_nNumPassengers && _this->m_nState != 5) {
            if (_this->m_nModelIndex == MODEL_TAXI || _this->m_nModelIndex == MODEL_CABBIE || _this->m_nModelIndex == MODEL_BORGNINE || GetTaxiModels().find(_this->m_nModelIndex) != GetTaxiModels().end())
                return result;
            else if (_this->m_nModelIndex == MODEL_FIRETRUK || GetFiretrukModels().find(_this->m_nModelIndex) != GetFiretrukModels().end())
                return result;
            else if (_this->m_nModelIndex == MODEL_AMBULAN || GetAmbulanModels().find(_this->m_nModelIndex) != GetAmbulanModels().end())
                return result;
            else if (_this->m_nModelIndex == MODEL_BUS || _this->m_nModelIndex == MODEL_COACH || _this->m_nModelIndex == MODEL_RHINO || _this->m_nModelIndex == MODEL_DODO || _this->m_nModelIndex == MODEL_RCBANDIT)
                return result;
            else if (_this->m_nModelIndex == MODEL_POLICE || GetPoliceModels().find(_this->m_nModelIndex) != GetPoliceModels().end())
                return result;
            else if (_this->m_nModelIndex == MODEL_ENFORCER || GetEnforcerModels().find(_this->m_nModelIndex) != GetEnforcerModels().end())
                return result;
            else if (_this->m_nModelIndex == MODEL_BARRACKS || GetBarracksModels().find(_this->m_nModelIndex) != GetBarracksModels().end())
                return result;
            else
                result = true;
        }
        return result;
    }

    // CVehicle::IsLawEnforcementVehicle
    static bool __fastcall IsLawEnforcementVehicle(CVehicle *_this) {
        bool result; result = false;
        if (_this->m_nModelIndex == MODEL_FBICAR || GetFbicarModels().find(_this->m_nModelIndex) != GetFbicarModels().end())
            result = true;
        else if (_this->m_nModelIndex == MODEL_ENFORCER || GetEnforcerModels().find(_this->m_nModelIndex) != GetEnforcerModels().end())
            result = true;
        else if (_this->m_nModelIndex == MODEL_PREDATOR || _this->m_nModelIndex == MODEL_RHINO)
            result = true;
        else if (_this->m_nModelIndex == MODEL_BARRACKS || GetBarracksModels().find(_this->m_nModelIndex) != GetBarracksModels().end())
            result = true;
        else if (_this->m_nModelIndex == MODEL_POLICE || GetPoliceModels().find(_this->m_nModelIndex) != GetPoliceModels().end())
            result = true;
        return result;
    }

    // CCarAI::AddPoliceCarOccupants
    static void __cdecl AddPoliceCarOccupants(CVehicle *vehicle) {
        if (!vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated) {
            vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated = 1;
            if (GetPoliceModels().find(vehicle->m_nModelIndex) != GetPoliceModels().end() || GetBarracksModels().find(vehicle->m_nModelIndex) != GetBarracksModels().end()) {
                vehicle->SetUpDriver();
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
                    vehicle->SetupPassenger(0);
                return;
            }
            else if (GetFbicarModels().find(vehicle->m_nModelIndex) != GetFbicarModels().end() || GetEnforcerModels().find(vehicle->m_nModelIndex) != GetEnforcerModels().end()) {
                vehicle->SetUpDriver();
                vehicle->SetupPassenger(0);
                vehicle->SetupPassenger(1);
                vehicle->SetupPassenger(2);
                return;
            }
            switch (vehicle->m_nModelIndex) {
            case MODEL_POLICE:
            case MODEL_RHINO:
            case MODEL_BARRACKS:
                vehicle->SetUpDriver();
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
                    vehicle->SetupPassenger(0);
                break;
            case MODEL_FBICAR:
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
            if (player->m_pWanted->AreSwatRequired()
                && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == LOADSTATE_LOADED
                && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == LOADSTATE_LOADED)
            {
                if (plugin::Random(0, 1)) {
                    unsigned int enforcerId = GetRandomEnforcer();
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
                    && CStreaming::ms_aInfoForModel[MODEL_FBICAR].m_nLoadState == LOADSTATE_LOADED
                    && CStreaming::ms_aInfoForModel[MODEL_FBI].m_nLoadState == LOADSTATE_LOADED)
                {
                    if (plugin::Random(0, 1)) {
                        unsigned int fbicarId = GetRandomFbicar();
                        if (CModelInfo::IsCarModel(fbicarId)) {
                            if (LoadModel(fbicarId))
                                return fbicarId;
                            else
                                return MODEL_FBICAR;
                        }
                        else
                            return MODEL_FBICAR;
                    }
                    else
                        return MODEL_FBICAR;
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
                            barracksId = GetRandomBarracks();
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
                            unsigned int policeId = GetRandomPolice();
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
        return MODEL_POLICE;
    }

    // cMusicManager::UsesPoliceRadio
    static bool __fastcall UsesPoliceRadio(cMusicManager *_this, int, CVehicle *vehicle) {
        bool result;
        if (GetPoliceModels().find(vehicle->m_nModelIndex) != GetPoliceModels().end() 
            || GetBarracksModels().find(vehicle->m_nModelIndex) != GetBarracksModels().end()
            || GetFbicarModels().find(vehicle->m_nModelIndex) != GetFbicarModels().end()
            || GetEnforcerModels().find(vehicle->m_nModelIndex) != GetEnforcerModels().end())
            return true;

        switch (vehicle->m_nModelIndex) {
        case MODEL_FBICAR:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case MODEL_POLICE:
            result = true;
            break;
        default:
            result = false;
            break;
        }
        return result;
    }

    // CVehicle::UsesSiren
    static bool __fastcall UsesSiren(CVehicle *_this, int, int model) {
        bool result;
        if (GetPoliceModels().find(model) != GetPoliceModels().end()
            || GetFbicarModels().find(model) != GetFbicarModels().end()
            || GetEnforcerModels().find(model) != GetEnforcerModels().end()
            || GetFiretrukModels().find(model) != GetFiretrukModels().end()
            || GetAmbulanModels().find(model) != GetAmbulanModels().end())
            return true;

        switch (model) {
        case MODEL_FIRETRUK:
        case MODEL_AMBULAN:
        case MODEL_FBICAR:
        case MODEL_MRWHOOP:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_POLICE:
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
            unsigned int model = driver->m_pVehicle->m_nModelIndex;
            if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end())
                FindPlayerPed()->m_fHealth = min(FindPlayerPed()->m_fHealth + 20.0f, 100.0f);
            else if (model == MODEL_POLICE || GetPoliceModels().find(model) != GetPoliceModels().end())
                driver->GiveWeapon(WEAPONTYPE_SHOTGUN, 5);
            else if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
                driver->m_fArmour = max(driver->m_fArmour, 100.0f);
            else if (model == MODEL_TAXI || model == MODEL_CABBIE || model == MODEL_BORGNINE || GetTaxiModels().find(model) != GetTaxiModels().end())
                CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 25;
            _this->m_nVehicleFlags.b08 = false; //bFreebies
        }

        _this->ApplyTurnForce(0.0f, 0.0f, -0.2f*driver->m_fMass,
            driver->GetPosition().x  - _this->GetPosition().x,
            driver->GetPosition().y - _this->GetPosition().y,
            0.0f);
    }

    // cMusicManager::PlayerInCar
    static bool __fastcall PlayerInCar(cMusicManager *_this) {
        int action;
        bool result;

        if (FindPlayerVehicle()) {
            action = FindPlayerPed()->m_nState;
            if (action != 51 && action != 54 && action != 56) {
                CVehicle *vehicle = FindPlayerVehicle();
                if (vehicle) {
                    if (vehicle->m_nState == 5)
                        result = false;
                    else {
                        if (GetFiretrukModels().find(vehicle->m_nModelIndex) != GetFiretrukModels().end() || GetAmbulanModels().find(vehicle->m_nModelIndex) != GetAmbulanModels().end())
                            return false;
                        switch (vehicle->m_nModelIndex) {
                        case MODEL_FIRETRUK:
                        case MODEL_AMBULAN:
                        case MODEL_MRWHOOP:
                        case MODEL_PREDATOR:
                        case MODEL_TRAIN:
                        case MODEL_SPEEDER:
                        case MODEL_REEFER:
                        case MODEL_GHOST:
                            result = false;
                            break;
                        default:
                            goto LABEL_11;
                        }
                    }
                }
                else {
                LABEL_11:
                    result = true;
                }
            }
            else
                result = false;
        }
        else
            result = false;

        return result;
    }

    // CGarages::IsCarSprayable
    static bool __cdecl IsCarSprayable(CAutomobile *car) {
        bool result;
        if (GetPoliceModels().find(car->m_nModelIndex) != GetPoliceModels().end()
            || GetBarracksModels().find(car->m_nModelIndex) != GetBarracksModels().end()
            || GetEnforcerModels().find(car->m_nModelIndex) != GetEnforcerModels().end()
            || GetFiretrukModels().find(car->m_nModelIndex) != GetFiretrukModels().end()
            || GetAmbulanModels().find(car->m_nModelIndex) != GetAmbulanModels().end())
            return false;

        switch (car->m_nModelIndex) {
        case MODEL_FIRETRUK:
        case MODEL_AMBULAN:
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_BUS:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case MODEL_DODO:
        case MODEL_COACH:
            result = false;
            break;
        default:
            result = true;
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
            if (model == MODEL_TAXI || model == MODEL_CABBIE || model == MODEL_BORGNINE || GetTaxiModels().find(model) != GetTaxiModels().end())
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
                || CTheScripts::ScriptParams[1].uParam == MODEL_RHINO || CTheScripts::ScriptParams[1].uParam == MODEL_FBICAR) {
                if (model == MODEL_RHINO || model == MODEL_POLICE || GetPoliceModels().find(model) != GetPoliceModels().end()
                    || model == MODEL_BARRACKS || GetBarracksModels().find(model) != GetBarracksModels().end()
                    || model == MODEL_FBICAR || GetFbicarModels().find(model) != GetFbicarModels().end()
                    || model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
                    inModel = true; // Vigilante
            }
            else if (CTheScripts::ScriptParams[1].uParam == MODEL_AMBULAN) {
                if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end()) 
                    inModel = true; // Paramedic
            }
            else if (CTheScripts::ScriptParams[1].uParam == MODEL_FIRETRUK) {
                if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end()) 
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

    static void SetPedLoadState(unsigned int modelIndex) {
        if (LoadModel(modelIndex)) {
            CCivilianPed *ped = nullptr;
            ped = new CCivilianPed(PEDTYPE_COP, modelIndex);
        }
    }
    
    static CVehicle *GetRandomCar(float x1, float y1, float x2, float y2) {
        std::vector<CVehicle *> vehicles;
        for (auto vehicle : CPools::ms_pVehiclePool) {
            if (vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE && vehicle->m_pDriver && vehicle->IsWithinArea(x1, y1, x2, y2))
                vehicles.push_back(vehicle);
        }
        return vehicles.empty() ? nullptr : vehicles[plugin::Random(0, vehicles.size() - 1)];
    }

    AddSpecialCars() {
        std::ifstream stream(PLUGIN_PATH("SpecialCars.dat"));
        if (!stream.is_open())
            return;
        for (string line; getline(stream, line); ) {
            if (!line.compare("police")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetPoliceModels().insert(stoi(line));
                }
            }
            if (!line.compare("fbicar")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetFbicarModels().insert(stoi(line));
                }
            }
            if (!line.compare("enforcer")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetEnforcerModels().insert(stoi(line));
                }
            }
            if (!line.compare("taxi")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetTaxiModels().insert(stoi(line));
                }
            }
            if (!line.compare("ambulan")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetAmbulanModels().insert(stoi(line));
                }
            }
            if (!line.compare("firetruk")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetFiretrukModels().insert(stoi(line));
                }
            }
            if (!line.compare("barracks")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetBarracksModels().insert(stoi(line));
                }
            }
            /*if (!line.compare("cop")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetCopModels().insert(stoi(line));
                }
            }
            if (!line.compare("swat")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetSwatModels().insert(stoi(line));
                }
            }
            if (!line.compare("fbi")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetFbiModels().insert(stoi(line));
                }
            }
            if (!line.compare("army")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetArmyModels().insert(stoi(line));
                }
            }*/
        }

        patch::RedirectJump(0x544B00, DoesMilitaryCraneHaveThisOneAlready);
        patch::RedirectJump(0x544B80, RegisterCarForMilitaryCrane);
        patch::RedirectJump(0x544A90, DoesCranePickUpThisCarType);
        patch::RedirectJump(0x552880, IsLawEnforcementVehicle);
        patch::RedirectJump(0x415C60, AddPoliceCarOccupants);
        patch::RedirectJump(0x4181F0, ChoosePoliceCarModel);
        patch::RedirectJump(0x57E6A0, UsesPoliceRadio);
        patch::RedirectJump(0x552200, UsesSiren);
        patch::RedirectJump(0x57E4B0, PlayerInCar);
        patch::RedirectJump(0x426700, IsCarSprayable);
        patch::RedirectJump(0x5527E0, IsVehicleNormal);

        patch::RedirectJump(0x551F20, SetDriver); // update 23.07.2021

        patch::RedirectCall(0x446A93, OpcodePlayerDrivingTaxiVehicle);
        patch::Nop(0x446A98, 0x4C); // or jump 0x446AE4

        patch::RedirectCall(0x43DA19, OpcodeIsPlayerInModel);
        patch::Nop(0x43DA1E, 0x40); // or jump 0x43DA5E

        patch::RedirectJump(0x5373D7, Patch_5373D7);
        patch::RedirectJump(0x4F5857, Patch_4F5857);
        patch::RedirectJump(0x531FE8, Patch_531FE8);
        patch::RedirectJump(0x4378C4, Patch_4378C4);
        patch::RedirectJump(0x56C234, Patch_56C234);
        patch::RedirectJump(0x56C4AD, Patch_56C4AD);
        
        // fix ID 154, 155, 159
        patch::SetUChar(0x52D0B3, 140, true);
        patch::SetUChar(0x52D0BA, 140, true);
        patch::SetUChar(0x53315A, 140, true);
        patch::SetUChar(0x533160, 140, true);
        patch::SetUChar(0x53425A, 140, true);
        patch::SetUChar(0x534260, 140, true);
        patch::SetUChar(0x534246, 140, true);
        patch::SetUChar(0x533154, 140, true);
        patch::SetUChar(0x534316, 140, true);
        patch::SetUChar(0x53431C, 140, true);

        static int spawnCarTime = 0;
        static int randomCopTime = 0;
        static unsigned int randomCop = 3;
        static unsigned int randomModel = 3;
        static unsigned int modelCop, modelSwat, modelFbi, modelArmy;

        Events::gameProcessEvent += [] {
            bReplayEnabled = false;
            if (CModelInfo::IsPedModel(MODEL_COP_a)) {
                if (CStreaming::ms_aInfoForModel[MODEL_COP_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetPedLoadState(MODEL_COP_a);
            }
            if (CModelInfo::IsPedModel(MODEL_COP_b)) {
                if (CStreaming::ms_aInfoForModel[MODEL_COP_b].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetPedLoadState(MODEL_COP_b);
            }
            if (CModelInfo::IsPedModel(MODEL_COP_c)) {
                if (CStreaming::ms_aInfoForModel[MODEL_COP_c].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetPedLoadState(MODEL_COP_c);
            }
            if (CModelInfo::IsPedModel(MODEL_SWAT_a)) {
                if (CStreaming::ms_aInfoForModel[MODEL_SWAT_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetPedLoadState(MODEL_SWAT_a);
            }
            if (CModelInfo::IsPedModel(MODEL_FBI_a)) {
                if (CStreaming::ms_aInfoForModel[MODEL_FBI_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetPedLoadState(MODEL_FBI_a);
            }
            if (CModelInfo::IsPedModel(MODEL_ARMY_a)) {
                if (CStreaming::ms_aInfoForModel[MODEL_ARMY_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetPedLoadState(MODEL_ARMY_a);
            }
            
            CPlayerPed *player = FindPlayerPed();
            if (player) {
                // Random Cops
                if (CTimer::m_snTimeInMilliseconds > (randomCopTime + 30000)) {
                    randomCopTime = CTimer::m_snTimeInMilliseconds;
                    // cop
                    if (randomCop < 3)
                        randomCop++;
                    else
                        randomCop = 0;
                    switch (randomCop) {
                    case 0: modelCop = MODEL_COP; break;
                    case 1:
                        if (CStreaming::ms_aInfoForModel[MODEL_COP_a].m_nLoadState == LOADSTATE_LOADED)
                            modelCop = MODEL_COP_a;
                        else
                            modelCop = MODEL_COP;
                        break;
                    case 2:
                        if (CStreaming::ms_aInfoForModel[MODEL_COP_b].m_nLoadState == LOADSTATE_LOADED)
                            modelCop = MODEL_COP_b;
                        else
                            modelCop = MODEL_COP;
                        break;
                    case 3:
                        if (CStreaming::ms_aInfoForModel[MODEL_COP_c].m_nLoadState == LOADSTATE_LOADED)
                            modelCop = MODEL_COP_c;
                        else
                            modelCop = MODEL_COP;
                        break;
                    default: modelCop = MODEL_COP;  break;
                    }
                    patch::SetChar(0x4C11F2, modelCop, true);
                    // swat
                    if (player->m_pWanted->AreSwatRequired()) {
                        if (plugin::Random(0, 1)) {
                            if (CStreaming::ms_aInfoForModel[MODEL_SWAT_a].m_nLoadState == LOADSTATE_LOADED)
                                modelSwat = MODEL_SWAT_a;
                            else
                                modelSwat = MODEL_SWAT;
                        }
                        else
                            modelSwat = MODEL_SWAT;
                        patch::SetChar(0x4C1241, modelSwat, true);
                        patch::SetChar(0x4378DB, modelSwat, true); // RoadBlocks
                    }
                    // fbi
                    if (player->m_pWanted->AreFbiRequired()) {
                        if (plugin::Random(0, 1)) {
                            if (CStreaming::ms_aInfoForModel[MODEL_FBI_a].m_nLoadState == LOADSTATE_LOADED)
                                modelFbi = MODEL_FBI_a;
                            else
                                modelFbi = MODEL_FBI;
                        }
                        else
                            modelFbi = MODEL_FBI;
                        patch::SetChar(0x4C12A0, modelFbi, true);
                        patch::SetChar(0x4378E7, modelFbi, true); // RoadBlocks
                    }
                    // army
                    if (player->m_pWanted->AreArmyRequired()) {
                        if (plugin::Random(0, 1)) {
                            if (CStreaming::ms_aInfoForModel[MODEL_ARMY_a].m_nLoadState == LOADSTATE_LOADED)
                                modelArmy = MODEL_ARMY_a;
                            else
                                modelArmy = MODEL_ARMY;
                        }
                        else
                            modelArmy = MODEL_ARMY;
                        patch::SetChar(0x4C12FC, modelArmy, true);
                        patch::SetChar(0x4378F3, modelArmy, true); // RoadBlocks
                    }
                    // armyRoadBlocks
                    if (player->m_pWanted->AreArmyRequired()) {
                        unsigned int barracksId = GetRandomBarracks();
                        if (CModelInfo::IsCarModel(barracksId) && LoadModel(barracksId)) {
                            if (patch::GetShort(0x43719C) == MODEL_BARRACKS)
                                patch::SetShort(0x43719C, barracksId, true);
                            else
                                patch::SetShort(0x43719C, MODEL_BARRACKS, true);
                        }
                    }
                    // swatRoadBlocks
                    if (player->m_pWanted->AreSwatRequired()) {
                        unsigned int enforcerId = GetRandomEnforcer();
                        if (CModelInfo::IsCarModel(enforcerId) && LoadModel(enforcerId)) {
                            if (patch::GetShort(0x4371D8) == MODEL_ENFORCER)
                                patch::SetShort(0x4371D8, enforcerId, true);
                            else
                                patch::SetShort(0x4371D8, MODEL_ENFORCER, true);
                        }
                    }
                    // fbiRoadBlocks
                    if (player->m_pWanted->AreFbiRequired()) {
                        unsigned int fbicarId = GetRandomFbicar();
                        if (CModelInfo::IsCarModel(fbicarId) && LoadModel(fbicarId)) {
                            if (patch::GetShort(0x4371BA) == MODEL_FBICAR)
                                patch::SetShort(0x4371BA, fbicarId, true);
                            else
                                patch::SetShort(0x4371BA, MODEL_FBICAR, true);
                        }
                    }
                }
                // Spawn Cars
                unsigned int ambulanId, firetrukId;
                switch (m_currentState) {
                case STATE_FIND:
                    if (CTimer::m_snTimeInMilliseconds > (spawnCarTime + 100000) && !CTheScripts::IsPlayerOnAMission()) {
                        CVector onePoint = player->TransformFromObjectSpace(CVector(20.0f, 130.0f, 0.0f));
                        CVector twoPoint = player->TransformFromObjectSpace(CVector(-20.0f, 60.0f, 0.0f));
                        CVehicle *car = GetRandomCar(onePoint.x, onePoint.y, twoPoint.x, twoPoint.y);
                        if (car) {
                            carPos = car->m_matrix.pos;
                            carAngle = car->GetHeading() / 57.295776f;
                            pilot = car->m_autoPilot;
                            m_currentState = STATE_WAIT;
                        }
                    }

                    break;
                case STATE_WAIT:
                    if (DistanceBetweenPoints(player->GetPosition(), carPos) < 150.0f) {
                        CVector cornerA, cornerB;
                        cornerA.x = carPos.x - 5.0f;
                        cornerA.y = carPos.y - 7.0f;
                        cornerA.z = carPos.z - 3.0f;
                        cornerB.x = carPos.x + 5.0f;
                        cornerB.y = carPos.y + 7.0f;
                        cornerB.z = carPos.z + 3.0f;
                        outCount = 1;
                        CWorld::FindObjectsIntersectingCube(cornerA, cornerB, &outCount, 2, 0, 0, 1, 1, 1, 0);
                        if (outCount == 0 && (DistanceBetweenPoints(player->GetPosition(), carPos) > 60.0f))
                            m_currentState = STATE_CREATE;
                    }
                    else
                        m_currentState = STATE_FIND;
                    break;
                case STATE_CREATE:
                    int modelCar, modelPed;
                    if (randomModel < 3)
                        randomModel++;
                    else
                        randomModel = 0;
                    switch (randomModel) {
                    case 0:
                        ambulanId = GetRandomAmbulance();
                        if (CModelInfo::IsCarModel(ambulanId) && LoadModel(ambulanId))
                            modelCar = ambulanId;
                        else
                            modelCar = MODEL_AMBULAN;
                        modelPed = MODEL_MEDIC;
                        break;
                    case 1:
                        firetrukId = GetRandomFiretruk();
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
                            vehicle->SetHeading(carAngle);
                            vehicle->m_nState = 4;
                            CWorld::Add(vehicle);
                            CTheScripts::ClearSpaceForMissionEntity(carPos, vehicle);
                            reinterpret_cast<CAutomobile *>(vehicle)->PlaceOnRoadProperly();
                            if (modelPed == MODEL_MEDIC)
                                CCarAI::AddAmbulanceOccupants(vehicle);
                            else
                                CCarAI::AddFiretruckOccupants(vehicle);
                            Command<COMMAND_CAR_GOTO_COORDINATES>(CPools::GetVehicleRef(vehicle), -1183.0f, 286.7f, 3.8f);
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
        };
    }
}test;

AddSpecialCars::eSpawnCarState AddSpecialCars::m_currentState = STATE_FIND;
short AddSpecialCars::outCount = 0;
CVector AddSpecialCars::carPos = { 0.0f, 0.0f, 0.0f };
float AddSpecialCars::carAngle = 0.0f;
CAutoPilot AddSpecialCars::pilot;

unsigned int AddSpecialCars::currentSpecialModelForSiren;
unsigned int AddSpecialCars::currentSpecialModelForOccupants;
unsigned int AddSpecialCars::randomPolice = 0;
unsigned int AddSpecialCars::currentFiretrukModel;
unsigned int AddSpecialCars::currentRoadBlockModel;
unsigned int AddSpecialCars::jmp_5373DE;
unsigned int AddSpecialCars::jmp_4F5860;
unsigned int AddSpecialCars::jmp_531FF1;
unsigned int AddSpecialCars::jmp_4378D1;
unsigned int AddSpecialCars::jmp_56C23A;
unsigned int AddSpecialCars::jmp_56C4B3;
unsigned int AddSpecialCars::currentModel_Patch_56C234;
unsigned int AddSpecialCars::currentModel_Patch_56C4AD;

void __declspec(naked) AddSpecialCars::Patch_5373D7() { // Siren
    __asm {
        movsx eax, word ptr[ebp + 0x5C]
        pushad
        push eax
        call GetSpecialModelForSiren
        mov currentSpecialModelForSiren, eax
        popad
        mov eax, currentSpecialModelForSiren
        lea edx, [eax - 0x61]
        mov jmp_5373DE, 0x5373DE
        jmp jmp_5373DE
    }
}

void __declspec(naked) AddSpecialCars::Patch_4F5857() { // AddPedInCar
    __asm {
        movsx   eax, word ptr[ebx + 0x5C]
        pushad
        push eax
        call GetSpecialModelForOccupants
        mov currentSpecialModelForOccupants, eax
        popad
        mov eax, currentSpecialModelForOccupants
        pop     ecx
        pop     ecx
        sub     eax, 61h
        mov jmp_4F5860, 0x4F5860
        jmp jmp_4F5860
    }
}

void __declspec(naked) AddSpecialCars::Patch_531FE8() { // FireTruckControl
    __asm {
        movsx eax, word ptr[ebp + 0x5C]
        pushad
        push eax
        call GetFiretrukModel
        mov currentFiretrukModel, eax
        popad
        mov eax, currentFiretrukModel
        fldz
        cmp     eax, 61h
        mov jmp_531FF1, 0x531FF1
        jmp jmp_531FF1
    }
}

void __declspec(naked) AddSpecialCars::Patch_4378C4() { // RoadBlockCopsForCar
    __asm {
        movsx   eax, word ptr[ebx + 0x5C]
        pushad
        push eax
        call GetRoadBlockModel
        mov currentRoadBlockModel, eax
        popad
        mov eax, currentRoadBlockModel
        add     esp, 0Ch
        sub     eax, 6Bh
        cmp     eax, 10h
        mov jmp_4378D1, 0x4378D1
        jmp jmp_4378D1
    }
}

void __declspec(naked) AddSpecialCars::Patch_56C234() {
    __asm {
        movsx   eax, word ptr[ebp + 5Ch]
        pushad
        push eax
        call GetModel_Patch_56C234
        mov currentModel_Patch_56C234, eax
        popad
        mov eax, currentModel_Patch_56C234
        mov jmp_56C23A, 0x56C23A
        jmp jmp_56C23A
    }
}

void __declspec(naked) AddSpecialCars::Patch_56C4AD() {
    __asm {
        movsx   eax, word ptr[ebp + 5Ch]
        pushad
        push eax
        call GetModel_Patch_56C4AD
        mov currentModel_Patch_56C4AD, eax
        popad
        mov eax, currentModel_Patch_56C4AD
        mov jmp_56C4B3, 0x56C4B3
        jmp jmp_56C4B3
    }
}

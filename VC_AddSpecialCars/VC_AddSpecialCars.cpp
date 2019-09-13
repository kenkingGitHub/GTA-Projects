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

class AddSpecialCars {
public:
    enum eSpawnCarState { STATE_FIND, STATE_WAIT, STATE_CREATE };

    static eSpawnCarState m_currentState;
    static short outCount;
    static CVector carPos;
    static float carAngle;
    static CAutoPilot pilot;
    static int randomFbicar;
    static unsigned int currentSpecialModelForSiren, currentSpecialModelForOccupants;
    static unsigned int jmp_53A913;

    static unordered_set<unsigned int> &GetPoliceModels() {
        static std::unordered_set<unsigned int> policeIds;
        return policeIds;
    }

    static unordered_set<unsigned int> &GetFbiranchModels() {
        static unordered_set<unsigned int> fbiranchIds;
        return fbiranchIds;
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

    static unsigned int GetRandomFbiranch() {
        vector<unsigned int> ids;
        for (auto id : GetFbiranchModels())
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
        else if (model == MODEL_FBICAR || model == MODEL_FBIRANCH || GetFbiranchModels().find(model) != GetFbiranchModels().end())
            return MODEL_FBIRANCH;
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
        else if (model == MODEL_FBICAR || model == MODEL_FBIRANCH || GetFbiranchModels().find(model) != GetFbiranchModels().end())
            return MODEL_FBIRANCH;
        else if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end())
            return MODEL_FIRETRUK;
        else if (model == MODEL_BARRACKS || GetBarracksModels().find(model) != GetBarracksModels().end())
            return MODEL_BARRACKS;
        else if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end())
            return MODEL_ENFORCER;
        return model;
    }

    static void Patch_58BE1F(); // Siren
    static void Patch_53A905(); // AddPedInCar

    // CCranes::DoesMilitaryCraneHaveThisOneAlready
    static bool __cdecl DoesMilitaryCraneHaveThisOneAlready(int model) {
        if (model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end()) {
            if (CarsCollectedMilitaryCrane & 1)
                return true;
        }
        else if (model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end()) {
            if (CarsCollectedMilitaryCrane & 2)
                return true;
        }
        else if (model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end()) {
            if (CarsCollectedMilitaryCrane & 4)
                return true;
        }
        else if (model == MODEL_FBICAR || model == MODEL_FBIRANCH || GetFbiranchModels().find(model) != GetFbiranchModels().end()) {
            if (CarsCollectedMilitaryCrane & 8)
                return true;
        }
        else if (model == MODEL_RHINO) {
            if (CarsCollectedMilitaryCrane & 0x10)
                return true;
        }
        else if (model == MODEL_BARRACKS || GetBarracksModels().find(model) != GetBarracksModels().end()) {
            if (CarsCollectedMilitaryCrane & 0x20)
                return true;
        }
        else if (model == MODEL_POLICE || GetPoliceModels().find(model) != GetPoliceModels().end()) {
            if (CarsCollectedMilitaryCrane & 0x40)
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
        else if (model == MODEL_FBICAR || model == MODEL_FBIRANCH || GetFbiranchModels().find(model) != GetFbiranchModels().end())
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
                && model != MODEL_TRASH && model != MODEL_SECURICA && model != MODEL_BUS && model != MODEL_RHINO;
        }
        else if (_this->m_bIsMilitaryCrane) {
            result = model == MODEL_FIRETRUK || GetFiretrukModels().find(model) != GetFiretrukModels().end()
                || model == MODEL_ENFORCER || GetEnforcerModels().find(model) != GetEnforcerModels().end()
                || model == MODEL_AMBULAN || GetAmbulanModels().find(model) != GetAmbulanModels().end()
                || model == MODEL_FBICAR || model == MODEL_FBIRANCH || GetFbiranchModels().find(model) != GetFbiranchModels().end()
                || model == MODEL_RHINO || model == MODEL_BARRACKS || model == MODEL_POLICE
                || GetBarracksModels().find(model) != GetBarracksModels().end()
                || GetPoliceModels().find(model) != GetPoliceModels().end();
        }
        else
            result = true;
        return result;
    }

    // CVehicle::IsLawEnforcementVehicle
    static bool __fastcall IsLawEnforcementVehicle(CVehicle *_this) {
        bool result; result = false;
        if (_this->m_nModelIndex == MODEL_FBICAR || _this->m_nModelIndex == MODEL_FBIRANCH || GetFbiranchModels().find(_this->m_nModelIndex) != GetFbiranchModels().end())
            result = true;
        else if (_this->m_nModelIndex == MODEL_ENFORCER || GetEnforcerModels().find(_this->m_nModelIndex) != GetEnforcerModels().end())
            result = true;
        else if (_this->m_nModelIndex == MODEL_PREDATOR || _this->m_nModelIndex == MODEL_RHINO || _this->m_nModelIndex == MODEL_VICECHEE)
            result = true;
        else if (_this->m_nModelIndex == MODEL_BARRACKS || GetBarracksModels().find(_this->m_nModelIndex) != GetBarracksModels().end())
            result = true;
        else if (_this->m_nModelIndex == MODEL_POLICE || GetPoliceModels().find(_this->m_nModelIndex) != GetPoliceModels().end())
            result = true;
        return result;
    }

    // CCarAI::AddPoliceCarOccupants
    static void __cdecl AddPoliceCarOccupants(CVehicle *vehicle)  {
        if (!vehicle->m_nVehicleFlags.bIsLawEnforcementVeh) {
            vehicle->m_nVehicleFlags.bIsLawEnforcementVeh = 1;
            if (GetPoliceModels().find(vehicle->m_nModelIndex) != GetPoliceModels().end() || GetBarracksModels().find(vehicle->m_nModelIndex) != GetBarracksModels().end()) {
                vehicle->SetUpDriver();
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
                    vehicle->SetupPassenger(0);
                return;
            }
            else if (GetFbiranchModels().find(vehicle->m_nModelIndex) != GetFbiranchModels().end() || GetEnforcerModels().find(vehicle->m_nModelIndex) != GetEnforcerModels().end()) {
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
                            fbicarId = GetRandomFbiranch();
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
        }
        return MODEL_POLICE;
    }

    // CVehicle::UsesSiren
    static bool __fastcall UsesSiren(CVehicle *_this) {
        bool result;
        if (GetPoliceModels().find(_this->m_nModelIndex) != GetPoliceModels().end()
            || GetFbiranchModels().find(_this->m_nModelIndex) != GetFbiranchModels().end()
            || GetEnforcerModels().find(_this->m_nModelIndex) != GetEnforcerModels().end()
            || GetFiretrukModels().find(_this->m_nModelIndex) != GetFiretrukModels().end()
            || GetAmbulanModels().find(_this->m_nModelIndex) != GetAmbulanModels().end())
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
            if (model == MODEL_TAXI || model == MODEL_CABBIE || model == MODEL_ZEBRA || model == MODEL_KAUFMAN || GetTaxiModels().find(model) != GetTaxiModels().end())
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
                if (model == MODEL_RHINO || model == MODEL_POLICE || GetPoliceModels().find(model) != GetPoliceModels().end()
                    || model == MODEL_BARRACKS || GetBarracksModels().find(model) != GetBarracksModels().end()
                    || model == MODEL_FBICAR || model == MODEL_FBIRANCH || GetFbiranchModels().find(model) != GetFbiranchModels().end()
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
            if (!line.compare("fbiranch")) {
                while (getline(stream, line) && line.compare("end")) {
                    if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                        GetFbiranchModels().insert(stoi(line));
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
        
        patch::RedirectJump(0x5A7F90, DoesMilitaryCraneHaveThisOneAlready);
        patch::RedirectJump(0x5A7EF0, RegisterCarForMilitaryCrane);
        patch::RedirectJump(0x5A8040, DoesCranePickUpThisCarType);
        patch::RedirectJump(0x5B7D60, IsLawEnforcementVehicle);
        patch::RedirectJump(0x419BB0, AddPoliceCarOccupants);
        patch::RedirectJump(0x426850, ChoosePoliceCarModel);
        patch::RedirectJump(0x5B8520, UsesSiren);

        patch::RedirectJump(0x58BE1F, Patch_58BE1F);
        patch::RedirectJump(0x53A905, Patch_53A905);

        patch::RedirectCall(0x45600E, OpcodePlayerDrivingTaxiVehicle);
        patch::Nop(0x456013, 0x5B); // or jump 0x45606E
        patch::RedirectCall(0x44525F, OpcodeIsPlayerInModel);
        patch::Nop(0x445264, 0x46); // or jump 0x4452AA
        

    }
} test;

AddSpecialCars::eSpawnCarState AddSpecialCars::m_currentState = STATE_FIND;
short AddSpecialCars::outCount = 0;
CVector AddSpecialCars::carPos = { 0.0f, 0.0f, 0.0f };
float AddSpecialCars::carAngle = 0.0f;
CAutoPilot AddSpecialCars::pilot;
int AddSpecialCars::randomFbicar = 2;
unsigned int AddSpecialCars::currentSpecialModelForSiren;
unsigned int AddSpecialCars::currentSpecialModelForOccupants;
unsigned int AddSpecialCars::jmp_53A913;

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

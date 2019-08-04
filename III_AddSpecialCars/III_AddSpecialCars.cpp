#include "plugin.h"
#include "CWanted.h"
#include "CStreaming.h"
#include "CWorld.h"
#include "eVehicleModel.h"
#include "ePedModel.h"
#include "eVehicleIndex.h"
#include "cMusicManager.h"
#include "cAudioManager.h"
#include "CTheScripts.h"
#include "CCivilianPed.h"
#include "CTimer.h"
#include "CModelInfo.h"
#include "extensions\ScriptCommands.h"
#include "eScriptCommands.h"
#include "CCarAI.h"
#include "CTheCarGenerators.h"
#include <unordered_set>
#include <string>
#include <fstream>

#define MODEL_POLICE_a 151
#define MODEL_POLICE_b 152
#define MODEL_POLICE_c 153
#define MODEL_POLICE_d 154
#define MODEL_ENFORCER_a 155
#define MODEL_AMBULAN_a 156
#define MODEL_FBICAR_a 157
#define MODEL_FIRETRUK_a 158
#define MODEL_BARRACKS_a 159
#define MODEL_COP_a 83
#define MODEL_COP_b 84
#define MODEL_COP_c 85
#define MODEL_SWAT_a 86
#define MODEL_FBI_a 87
#define MODEL_ARMY_a 88

bool &bReplayEnabled = *(bool *)0x617CAC;

using namespace plugin;

class AddSpecialCars {
public:
    enum eSpawnCarState { STATE_FIND, STATE_WAIT, STATE_CREATE };

    static eSpawnCarState m_currentState;
    static short outCount;
    static CVector carPos;
    static float carAngle;
    static CAutoPilot pilot;
    
    static unsigned int currentSpecialModelForSiren;
    static unsigned int currentSpecialModelForOccupants;
    static unsigned int randomPolice;
    static unsigned int currentFiretrukModel;
    
    static unsigned int jmp_5373DE;
    static unsigned int jmp_4F5860;
    static unsigned int jmp_531FF1;
    
    static std::unordered_set<unsigned int> &GetTaxiModels() {
        static std::unordered_set<unsigned int> taxiIds;
        return taxiIds;
    }

    static int __stdcall GetSpecialModelForSiren(unsigned int model) {
        if (model == MODEL_POLICE || model == MODEL_POLICE_a || model == MODEL_POLICE_b || model == MODEL_POLICE_c || model == MODEL_POLICE_d)
            return MODEL_POLICE;
        else if (model == MODEL_TAXI || GetTaxiModels().find(model) != GetTaxiModels().end())
            return MODEL_TAXI;
        else if (model == MODEL_AMBULAN || model == MODEL_AMBULAN_a)
            return MODEL_AMBULAN;
        else if (model == MODEL_FBICAR || model == MODEL_FBICAR_a)
            return MODEL_FBICAR;
        else if (model == MODEL_FIRETRUK || model == MODEL_FIRETRUK_a)
            return MODEL_FIRETRUK;
        else if (model == MODEL_ENFORCER || model == MODEL_ENFORCER_a)
            return MODEL_ENFORCER;
        return model;
    }

    static int __stdcall GetSpecialModelForOccupants(unsigned int model) {
        if (model == MODEL_POLICE || model == MODEL_POLICE_a || model == MODEL_POLICE_b || model == MODEL_POLICE_c || model == MODEL_POLICE_d)
            return MODEL_POLICE;
        else if (model == MODEL_AMBULAN || model == MODEL_AMBULAN_a)
            return MODEL_AMBULAN;
        else if (model == MODEL_FBICAR || model == MODEL_FBICAR_a)
            return MODEL_FBICAR;
        else if (model == MODEL_FIRETRUK || model == MODEL_FIRETRUK_a)
            return MODEL_FIRETRUK;
        else if (model == MODEL_BARRACKS || model == MODEL_BARRACKS_a)
            return MODEL_BARRACKS;
        else if (model == MODEL_ENFORCER || model == MODEL_ENFORCER_a)
            return MODEL_ENFORCER;
        return model;
    }

    static int __stdcall GetFiretrukModel(unsigned int model) {
        if (model == MODEL_FIRETRUK || model == MODEL_FIRETRUK_a)
            return MODEL_FIRETRUK;
        return model;
    }

    static void Patch_5373D7(); // Siren
    static void Patch_4F5857(); // AddPedInCar
    static void Patch_531FE8(); // FireTruckControl
    
    // CVehicle::IsVehicleNormal
    static bool __fastcall IsVehicleNormal(CVehicle *_this) {
        bool result; 

        result = FALSE;
        if (_this->m_pDriver && !_this->m_nNumPassengers && _this->m_nState != 5) {
            if (GetTaxiModels().find(_this->m_nModelIndex) != GetTaxiModels().end())
                return result;
            switch (_this->m_nModelIndex) {
            default:
                result = TRUE;
                break;
            case MODEL_FIRETRUK:
            case MODEL_FIRETRUK_a:
            case MODEL_AMBULAN:
            case MODEL_AMBULAN_a:
            case MODEL_TAXI:
            case MODEL_POLICE:
            case MODEL_POLICE_a:
            case MODEL_POLICE_b:
            case MODEL_POLICE_c:
            case MODEL_POLICE_d:
            case MODEL_ENFORCER:
            case MODEL_ENFORCER_a:
            case MODEL_BUS:
            case MODEL_RHINO:
            case MODEL_BARRACKS:
            case MODEL_BARRACKS_a:
            case MODEL_DODO:
            case MODEL_COACH:
            case MODEL_CABBIE:
            case MODEL_RCBANDIT:
            case MODEL_BORGNINE:
                return result;
            }
        }
        return result;
    }

    // CVehicle::IsLawEnforcementVehicle
    static bool __fastcall IsLawEnforcementVehicle(CVehicle *_this) {
        bool result;

        switch (_this->m_nModelIndex) {
        case MODEL_FBICAR:
        case MODEL_FBICAR_a:
        case MODEL_ENFORCER:
        case MODEL_ENFORCER_a:
        case MODEL_PREDATOR:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case MODEL_BARRACKS_a:
        case MODEL_POLICE:
        case MODEL_POLICE_a:
        case MODEL_POLICE_b:
        case MODEL_POLICE_c:
        case MODEL_POLICE_d:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    // CCarAI::AddPoliceCarOccupants
    static void __cdecl AddPoliceCarOccupants(CVehicle *vehicle) {
        if (!vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated) {
            vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated = 1;
            switch (vehicle->m_nModelIndex) {
            case MODEL_POLICE_a:
            case MODEL_POLICE_b:
            case MODEL_POLICE:
            case MODEL_RHINO:
            case MODEL_BARRACKS:
            case MODEL_BARRACKS_a:
                vehicle->SetUpDriver();
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
                    vehicle->SetupPassenger(0);
                break;
            case MODEL_POLICE_c:
            case MODEL_POLICE_d:
                vehicle->SetUpDriver();
                vehicle->SetupPassenger(0);
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1) {
                    vehicle->SetupPassenger(1);
                    vehicle->SetupPassenger(2);
                }
                break;
            case MODEL_FBICAR:
            case MODEL_FBICAR_a:
            case MODEL_ENFORCER:
            case MODEL_ENFORCER_a:
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
        int result;

        CPlayerPed *player = FindPlayerPed();
        if (player) {
            
            if (CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_pWanted->m_nWantedLevel > 1 && CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_pWanted->m_nWantedLevel < 4) {
                if (randomPolice < 3)
                    randomPolice++;
                else
                    randomPolice = 0;
                switch (randomPolice) {
                case 0: result = MODEL_POLICE; break;
                case 1:
                    if (CStreaming::ms_aInfoForModel[MODEL_POLICE_a].m_nLoadState == LOADSTATE_LOADED)
                        result = MODEL_POLICE_a;
                    else
                        result = MODEL_POLICE;
                    break;
                case 2:
                    if (CStreaming::ms_aInfoForModel[MODEL_POLICE_b].m_nLoadState == LOADSTATE_LOADED)
                        result = MODEL_POLICE_b;
                    else
                        result = MODEL_POLICE;
                    break;
                case 3:
                    if (CStreaming::ms_aInfoForModel[MODEL_POLICE_c].m_nLoadState == LOADSTATE_LOADED)
                        result = MODEL_POLICE_c;
                    else
                        result = MODEL_POLICE;
                    break;
                default: result = MODEL_POLICE;  break;
                }
            }

            else if (player->m_pWanted->AreSwatRequired()
                && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == LOADSTATE_LOADED
                && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == LOADSTATE_LOADED)
            {
                int randomSwat = plugin::Random(0, 2);
                switch (randomSwat) {
                case 0:
                    if (CStreaming::ms_aInfoForModel[MODEL_POLICE_d].m_nLoadState == LOADSTATE_LOADED)
                        result = MODEL_POLICE_d;
                    else
                        result = MODEL_POLICE;
                    break;
                case 1:
                    if (CStreaming::ms_aInfoForModel[MODEL_ENFORCER_a].m_nLoadState == LOADSTATE_LOADED)
                        result = MODEL_ENFORCER_a;
                    else
                        result = MODEL_ENFORCER;
                    break;
                case 2: result = MODEL_ENFORCER; break;
                default: result = MODEL_POLICE;  break;
                }
            }
            else
            {
                if (player->m_pWanted->AreFbiRequired()
                    && CStreaming::ms_aInfoForModel[MODEL_FBICAR].m_nLoadState == LOADSTATE_LOADED
                    && CStreaming::ms_aInfoForModel[MODEL_FBI].m_nLoadState == LOADSTATE_LOADED)
                {
                    int randomFbi = plugin::Random(0, 1);
                    if (randomFbi) {
                        if (CStreaming::ms_aInfoForModel[MODEL_FBICAR_a].m_nLoadState == LOADSTATE_LOADED)
                            result = MODEL_FBICAR_a;
                        else
                            result = MODEL_FBICAR;
                    }
                    else
                        result = MODEL_FBICAR;
                }
                else {
                    if (player->m_pWanted->AreArmyRequired()
                        && CStreaming::ms_aInfoForModel[MODEL_RHINO].m_nLoadState == LOADSTATE_LOADED
                        && CStreaming::ms_aInfoForModel[MODEL_BARRACKS].m_nLoadState == LOADSTATE_LOADED
                        && CStreaming::ms_aInfoForModel[MODEL_ARMY].m_nLoadState == LOADSTATE_LOADED)
                    {
                        int randomArmy = plugin::Random(0, 4);
                        switch (randomArmy) {
                        case 0: 
                        case 1:
                            if (CStreaming::ms_aInfoForModel[MODEL_BARRACKS_a].m_nLoadState == LOADSTATE_LOADED)
                                result = MODEL_BARRACKS_a;
                            else
                                result = MODEL_BARRACKS;
                            break;
                        case 2:
                        case 3:
                            result = MODEL_BARRACKS; break;
                        case 4:
                            result = MODEL_RHINO; break;
                        default: result = MODEL_POLICE;  break;
                        }
                    }
                    else {
                        if (randomPolice < 3)
                            randomPolice++;
                        else
                            randomPolice = 0;
                        switch (randomPolice) {
                        case 0: result = MODEL_POLICE; break;
                        case 1:
                            if (CStreaming::ms_aInfoForModel[MODEL_POLICE_a].m_nLoadState == LOADSTATE_LOADED)
                                result = MODEL_POLICE_a;
                            else
                                result = MODEL_POLICE;
                            break;
                        case 2:
                            if (CStreaming::ms_aInfoForModel[MODEL_POLICE_b].m_nLoadState == LOADSTATE_LOADED)
                                result = MODEL_POLICE_b;
                            else
                                result = MODEL_POLICE;
                            break;
                        case 3:
                            if (CStreaming::ms_aInfoForModel[MODEL_POLICE_c].m_nLoadState == LOADSTATE_LOADED)
                                result = MODEL_POLICE_c;
                            else
                                result = MODEL_POLICE;
                            break;
                        default: result = MODEL_POLICE;  break;
                        }
                    }
                }
            }
        }
        return result;
    }

    // cMusicManager::UsesPoliceRadio
    static bool __fastcall UsesPoliceRadio(cMusicManager *_this, int, CVehicle *vehicle) {
        bool result;

        switch (vehicle->m_nModelIndex) {
        case MODEL_FBICAR:
        case MODEL_FBICAR_a:
        case MODEL_ENFORCER:
        case MODEL_ENFORCER_a:
        case MODEL_PREDATOR:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case MODEL_BARRACKS_a:
        case MODEL_POLICE:
        case MODEL_POLICE_a:
        case MODEL_POLICE_b:
        case MODEL_POLICE_c:
        case MODEL_POLICE_d:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    // CVehicle::UsesSiren
    static bool __fastcall UsesSiren(CVehicle *_this, int, int vehicleModel) {
        bool result;

        switch (vehicleModel) {
        case MODEL_FIRETRUK:
        case MODEL_FIRETRUK_a:
        case MODEL_AMBULAN:
        case MODEL_AMBULAN_a:
        case MODEL_FBICAR:
        case MODEL_FBICAR_a:
        case MODEL_MRWHOOP:
        case MODEL_ENFORCER:
        case MODEL_ENFORCER_a:
        case MODEL_PREDATOR:
        case MODEL_POLICE:
        case MODEL_POLICE_a:
        case MODEL_POLICE_b:
        case MODEL_POLICE_c:
        case MODEL_POLICE_d:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
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
                        result = FALSE;
                    else {
                        switch (vehicle->m_nModelIndex) {
                        case MODEL_FIRETRUK:
                        case MODEL_FIRETRUK_a:
                        case MODEL_AMBULAN:
                        case MODEL_AMBULAN_a:
                        case MODEL_MRWHOOP:
                        case MODEL_PREDATOR:
                        case MODEL_TRAIN:
                        case MODEL_SPEEDER:
                        case MODEL_REEFER:
                        case MODEL_GHOST:
                            result = FALSE;
                            break;
                        default:
                            goto LABEL_11;
                        }
                    }
                }
                else {
                LABEL_11:
                    result = TRUE;
                }
            }
            else
                result = FALSE;
        }
        else
            result = FALSE;

        return result;
    }

    // cAudioManager::UsesSiren
    static bool __fastcall UsesSirenAudio(cAudioManager *_this, int, int index) {
        bool result;

        switch (index) { // eVehicleIndex
        case VEHICLE_FIRETRUK:
        case VEHICLE_AMBULAN:
        case VEHICLE_FBICAR:
        case VEHICLE_POLICE:
        case VEHICLE_ENFORCER:
        case VEHICLE_PREDATOR:
        case VEHICLE_151:
        case VEHICLE_152:
        case VEHICLE_153:
        case VEHICLE_154:
        case VEHICLE_155:
        case VEHICLE_156:
        case VEHICLE_157:
        case VEHICLE_158:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    // cAudioManager::UsesSirenSwitching
    static bool __fastcall UsesSirenSwitching(cAudioManager *_this, int, int index) {
        bool result;

        switch (index) { // eVehicleIndex
        case VEHICLE_AMBULAN:
        case VEHICLE_POLICE:
        case VEHICLE_ENFORCER:
        case VEHICLE_PREDATOR:
        case VEHICLE_151:
        case VEHICLE_152:
        case VEHICLE_153:
        case VEHICLE_154:
        case VEHICLE_155:
        case VEHICLE_156:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    // CGarages::IsCarSprayable
    static bool __cdecl IsCarSprayable(CAutomobile *car) {
        bool result;

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
        case MODEL_POLICE_a:
        case MODEL_POLICE_b:
        case MODEL_POLICE_c:
        case MODEL_POLICE_d:
        case MODEL_AMBULAN_a:
        case MODEL_FIRETRUK_a:
        case MODEL_BARRACKS_a:
        case MODEL_ENFORCER_a:
            result = FALSE;
            break;
        default:
            result = TRUE;
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
                switch (model) { // Vigilante
                case MODEL_POLICE:
                case MODEL_POLICE_a:
                case MODEL_POLICE_b:
                case MODEL_POLICE_c:
                case MODEL_POLICE_d:
                case MODEL_ENFORCER:
                case MODEL_ENFORCER_a:
                case MODEL_RHINO:
                case MODEL_FBICAR:
                case MODEL_FBICAR_a:
                case MODEL_BARRACKS:
                case MODEL_BARRACKS_a:
                    inModel = true;
                    break;
                default:
                    inModel = false;
                    break;
                }
            }
            else if (CTheScripts::ScriptParams[1].uParam == MODEL_AMBULAN) {
                if (model == MODEL_AMBULAN_a || model == MODEL_AMBULAN) // Paramedic
                    inModel = true;
            }
            else if (CTheScripts::ScriptParams[1].uParam == MODEL_FIRETRUK) {
                if (model == MODEL_FIRETRUK_a || model == MODEL_FIRETRUK) // Firefighter
                    inModel = true;
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

    static void SetVehicleLoadState(unsigned int modelIndex) {
        if (LoadModel(modelIndex)) {
            CVehicle *vehicle = nullptr;
            vehicle = new CAutomobile(modelIndex, 1);
        }
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
        std::ifstream stream(PLUGIN_PATH("taxi.dat"));
        if (!stream.is_open())
            return;
        for (std::string line; getline(stream, line); ) {
            if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                GetTaxiModels().insert(std::stoi(line));
        }

        patch::RedirectJump(0x552880, IsLawEnforcementVehicle);
        patch::RedirectJump(0x415C60, AddPoliceCarOccupants);
        patch::RedirectJump(0x4181F0, ChoosePoliceCarModel);
        patch::RedirectJump(0x57E6A0, UsesPoliceRadio);
        patch::RedirectJump(0x552200, UsesSiren);
        patch::RedirectJump(0x57E4B0, PlayerInCar);
        patch::RedirectJump(0x56C3C0, UsesSirenAudio);
        patch::RedirectJump(0x56C3F0, UsesSirenSwitching);
        patch::RedirectJump(0x426700, IsCarSprayable);
        patch::RedirectJump(0x5527E0, IsVehicleNormal);

        patch::RedirectCall(0x446A93, OpcodePlayerDrivingTaxiVehicle);
        patch::Nop(0x446A98, 0x4C); // или сделать jump на 0x446AE4

        patch::RedirectCall(0x43DA19, OpcodeIsPlayerInModel);
        patch::Nop(0x43DA1E, 0x40); // или сделать jump на 0x43DA5E

        patch::RedirectJump(0x5373D7, Patch_5373D7);
        patch::RedirectJump(0x4F5857, Patch_4F5857);
        patch::RedirectJump(0x531FE8, Patch_531FE8);
        
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
        static unsigned int modelCop, modelSwat, modelFbi, modelArmy;

        Events::gameProcessEvent += [] {
            bReplayEnabled = false;
            if (CModelInfo::IsCarModel(MODEL_POLICE_a)) {
                if (CStreaming::ms_aInfoForModel[MODEL_POLICE_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetVehicleLoadState(MODEL_POLICE_a);
            }
            if (CModelInfo::IsCarModel(MODEL_POLICE_b)) {
                if (CStreaming::ms_aInfoForModel[MODEL_POLICE_b].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetVehicleLoadState(MODEL_POLICE_b);
            }
            if (CModelInfo::IsCarModel(MODEL_POLICE_c)) {
                if (CStreaming::ms_aInfoForModel[MODEL_POLICE_c].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetVehicleLoadState(MODEL_POLICE_c);
            }
            if (CModelInfo::IsCarModel(MODEL_POLICE_d)) {
                if (CStreaming::ms_aInfoForModel[MODEL_POLICE_d].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetVehicleLoadState(MODEL_POLICE_d);
            }
            if (CModelInfo::IsCarModel(MODEL_AMBULAN_a)) {
                if (CStreaming::ms_aInfoForModel[MODEL_AMBULAN_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetVehicleLoadState(MODEL_AMBULAN_a);
            }
            if (CModelInfo::IsCarModel(MODEL_FBICAR_a)) {
                if (CStreaming::ms_aInfoForModel[MODEL_FBICAR_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetVehicleLoadState(MODEL_FBICAR_a);
            }
            if (CModelInfo::IsCarModel(MODEL_FIRETRUK_a)) {
                if (CStreaming::ms_aInfoForModel[MODEL_FIRETRUK_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetVehicleLoadState(MODEL_FIRETRUK_a);
            }
            if (CModelInfo::IsCarModel(MODEL_BARRACKS_a)) {
                if (CStreaming::ms_aInfoForModel[MODEL_BARRACKS_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                    SetVehicleLoadState(MODEL_BARRACKS_a);
            }
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
                if (plugin::Random(0, 1)) {
                    if (CStreaming::ms_aInfoForModel[MODEL_SWAT_a].m_nLoadState == LOADSTATE_LOADED)
                        modelSwat = MODEL_SWAT_a;
                    else
                        modelSwat = MODEL_SWAT;
                }
                else
                    modelSwat = MODEL_SWAT;
                patch::SetChar(0x4C1241, modelSwat, true);
                // fbi
                if (plugin::Random(0, 1)) {
                    if (CStreaming::ms_aInfoForModel[MODEL_FBI_a].m_nLoadState == LOADSTATE_LOADED)
                        modelFbi = MODEL_FBI_a;
                    else
                        modelFbi = MODEL_FBI;
                }
                else
                    modelFbi = MODEL_FBI;
                patch::SetChar(0x4C12A0, modelFbi, true);
                // army
                if (plugin::Random(0, 1)) {
                    if (CStreaming::ms_aInfoForModel[MODEL_ARMY_a].m_nLoadState == LOADSTATE_LOADED)
                        modelArmy = MODEL_ARMY_a;
                    else
                        modelArmy = MODEL_ARMY;
                }
                else
                    modelArmy = MODEL_ARMY;
                patch::SetChar(0x4C12FC, modelArmy, true);
            }
            // Spawn Cars
            CPlayerPed *player = FindPlayerPed();
            if (player) {
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
                    int randomModel = plugin::Random(0, 3);
                    switch (randomModel) {
                    case 0:
                        if (CModelInfo::IsCarModel(MODEL_AMBULAN_a))
                            modelCar = MODEL_AMBULAN_a;
                        else
                            modelCar = MODEL_AMBULAN;
                        modelPed = MODEL_MEDIC;
                        break;
                    case 1:
                        if (CModelInfo::IsCarModel(MODEL_FIRETRUK_a))
                            modelCar = MODEL_FIRETRUK_a;
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

unsigned int AddSpecialCars::jmp_5373DE;
unsigned int AddSpecialCars::jmp_4F5860;
unsigned int AddSpecialCars::jmp_531FF1;

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
        movsx   eax, word ptr[ebx + 5Ch]
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

class MyCarGenerator {
public:
    MyCarGenerator() {
        static CdeclEvent<AddressList<0x582E6C, H_CALL, 0x48C7CC, H_CALL>, PRIORITY_AFTER, ArgPickNone, void()> myOnInitGame;

        static float angle_portland_police, angle_staunton_police, angle_shoreside_police, angle_staunton_ambulance,
            angle_shoreside_ambulance, angle_shoreside_firetruck, angle_portland_firetruck, angle_staunton_firetruck;
        angle_portland_police = angle_staunton_police = angle_shoreside_police = angle_staunton_ambulance = angle_shoreside_ambulance 
            = angle_shoreside_firetruck = angle_portland_firetruck = angle_staunton_firetruck = 0.0f;
        static CVector portland_police = { 1148.76f, -690.0f, 14.0f };
        static CVector staunton_police = { 347.0f, -1170.25f, 22.0f };
        static CVector shoreside_police = { -1261.84f, -21.49f, 58.5f };
        static CVector staunton_ambulance = { 185.19f, -41.0f, 17.0f };
        static CVector shoreside_ambulance = { -1271.68f, -143.18f, 58.83f };
        static CVector shoreside_firetruck = { -839.38f, -452.39f, 12.0f };
        static CVector portland_firetruck = { 1107.5f, -36.57f, 7.5f };
        static CVector staunton_firetruck = { -83.88f, -457.54f, 17.0f };

        myOnInitGame += [] {
            if (CTheCarGenerators::NumOfCarGenerators < 160) {
                bool alreadyRegisteredA, alreadyRegisteredB, alreadyRegisteredC, alreadyRegisteredD, 
                    alreadyRegisteredE, alreadyRegisteredF, alreadyRegisteredG, alreadyRegisteredH;
                alreadyRegisteredA = alreadyRegisteredB = alreadyRegisteredC = alreadyRegisteredD 
                    = alreadyRegisteredE = alreadyRegisteredF = alreadyRegisteredG = alreadyRegisteredH = false;
                for (int i = 0; i < CTheCarGenerators::NumOfCarGenerators; i++) {
                    if (CModelInfo::IsCarModel(MODEL_POLICE_a)) {
                        CCarGenerator &carGenA = CTheCarGenerators::CarGeneratorArray[i];
                        if (carGenA.m_nEnabled && DistanceBetweenPoints(portland_police, carGenA.m_vecPos) < 1.0f && carGenA.m_fAngle == angle_portland_police && carGenA.m_nModelId == MODEL_POLICE_a)
                            alreadyRegisteredA = true;
                    }
                    if (CModelInfo::IsCarModel(MODEL_POLICE_b)) {
                        CCarGenerator &carGenB = CTheCarGenerators::CarGeneratorArray[i];
                        if (carGenB.m_nEnabled && DistanceBetweenPoints(staunton_police, carGenB.m_vecPos) < 1.0f && carGenB.m_fAngle == angle_staunton_police && carGenB.m_nModelId == MODEL_POLICE_b)
                            alreadyRegisteredB = true;
                    }
                    if (CModelInfo::IsCarModel(MODEL_POLICE_c)) {
                        CCarGenerator &carGenC = CTheCarGenerators::CarGeneratorArray[i];
                        if (carGenC.m_nEnabled && DistanceBetweenPoints(shoreside_police, carGenC.m_vecPos) < 1.0f && carGenC.m_fAngle == angle_shoreside_police && carGenC.m_nModelId == MODEL_POLICE_c)
                            alreadyRegisteredC = true;
                    }
                    if (CModelInfo::IsCarModel(MODEL_AMBULAN_a)) {
                        CCarGenerator &carGenD = CTheCarGenerators::CarGeneratorArray[i];
                        if (carGenD.m_nEnabled && DistanceBetweenPoints(staunton_ambulance, carGenD.m_vecPos) < 1.0f && carGenD.m_fAngle == angle_staunton_ambulance && carGenD.m_nModelId == MODEL_AMBULAN_a)
                            alreadyRegisteredD = true;
                        CCarGenerator &carGenE = CTheCarGenerators::CarGeneratorArray[i];
                        if (carGenE.m_nEnabled && DistanceBetweenPoints(shoreside_ambulance, carGenE.m_vecPos) < 1.0f && carGenE.m_fAngle == angle_shoreside_ambulance && carGenE.m_nModelId == MODEL_AMBULAN_a)
                            alreadyRegisteredE = true;
                    }
                    if (CModelInfo::IsCarModel(MODEL_FIRETRUK_a)) {
                        CCarGenerator &carGenF = CTheCarGenerators::CarGeneratorArray[i];
                        if (carGenF.m_nEnabled && DistanceBetweenPoints(shoreside_firetruck, carGenF.m_vecPos) < 1.0f && carGenF.m_fAngle == angle_shoreside_firetruck && carGenF.m_nModelId == MODEL_FIRETRUK_a)
                            alreadyRegisteredF = true;
                        CCarGenerator &carGenG = CTheCarGenerators::CarGeneratorArray[i];
                        if (carGenG.m_nEnabled && DistanceBetweenPoints(portland_firetruck, carGenG.m_vecPos) < 1.0f && carGenG.m_fAngle == angle_portland_firetruck && carGenG.m_nModelId == MODEL_FIRETRUK_a)
                            alreadyRegisteredG = true;
                        CCarGenerator &carGenH = CTheCarGenerators::CarGeneratorArray[i];
                        if (carGenH.m_nEnabled && DistanceBetweenPoints(staunton_firetruck, carGenH.m_vecPos) < 1.0f && carGenH.m_fAngle == angle_staunton_firetruck && carGenH.m_nModelId == MODEL_FIRETRUK_a)
                            alreadyRegisteredH = true;
                    }
                }
                if (!alreadyRegisteredA && CModelInfo::IsCarModel(MODEL_POLICE_a)) {
                    unsigned int carGenAId = CTheCarGenerators::CreateCarGenerator(portland_police.x, portland_police.y, portland_police.z, 0.0f, MODEL_POLICE_a, -1, -1, 0, 0, 0, 0, 10000);
                    CTheCarGenerators::CarGeneratorArray[carGenAId].SwitchOn();
                }
                if (!alreadyRegisteredB && CModelInfo::IsCarModel(MODEL_POLICE_b)) {
                    unsigned int carGenBId = CTheCarGenerators::CreateCarGenerator(staunton_police.x, staunton_police.y, staunton_police.z, 0.0f, MODEL_POLICE_b, -1, -1, 0, 0, 0, 0, 10000);
                    CTheCarGenerators::CarGeneratorArray[carGenBId].SwitchOn();
                }
                if (!alreadyRegisteredC && CModelInfo::IsCarModel(MODEL_POLICE_c)) {
                    unsigned int carGenCId = CTheCarGenerators::CreateCarGenerator(shoreside_police.x, shoreside_police.y, shoreside_police.z, 173.23f, MODEL_POLICE_c, -1, -1, 0, 0, 0, 0, 10000);
                    CTheCarGenerators::CarGeneratorArray[carGenCId].SwitchOn();
                }
                if (!alreadyRegisteredD && CModelInfo::IsCarModel(MODEL_AMBULAN_a)) {
                    unsigned int carGenDId = CTheCarGenerators::CreateCarGenerator(staunton_ambulance.x, staunton_ambulance.y, staunton_ambulance.z, 0.0f, MODEL_AMBULAN_a, -1, -1, 0, 0, 0, 0, 10000);
                    CTheCarGenerators::CarGeneratorArray[carGenDId].SwitchOn();
                }
                if (!alreadyRegisteredE && CModelInfo::IsCarModel(MODEL_AMBULAN_a)) {
                    unsigned int carGenEId = CTheCarGenerators::CreateCarGenerator(shoreside_ambulance.x, shoreside_ambulance.y, shoreside_ambulance.z, 0.0f, MODEL_AMBULAN_a, -1, -1, 0, 0, 0, 0, 10000);
                    CTheCarGenerators::CarGeneratorArray[carGenEId].SwitchOn();
                }
                if (!alreadyRegisteredF && CModelInfo::IsCarModel(MODEL_FIRETRUK_a)) {
                    unsigned int carGenFId = CTheCarGenerators::CreateCarGenerator(shoreside_firetruck.x, shoreside_firetruck.y, shoreside_firetruck.z, 90.0f, MODEL_FIRETRUK_a, -1, -1, 0, 0, 0, 0, 10000);
                    CTheCarGenerators::CarGeneratorArray[carGenFId].SwitchOn();
                }
                if (!alreadyRegisteredG && CModelInfo::IsCarModel(MODEL_FIRETRUK_a)) {
                    unsigned int carGenGId = CTheCarGenerators::CreateCarGenerator(portland_firetruck.x, portland_firetruck.y, portland_firetruck.z, 270.0f, MODEL_FIRETRUK_a, -1, -1, 0, 0, 0, 0, 10000);
                    CTheCarGenerators::CarGeneratorArray[carGenGId].SwitchOn();
                }
                if (!alreadyRegisteredH && CModelInfo::IsCarModel(MODEL_FIRETRUK_a)) {
                    unsigned int carGenHId = CTheCarGenerators::CreateCarGenerator(staunton_firetruck.x, staunton_firetruck.y, staunton_firetruck.z, 90.0f, MODEL_FIRETRUK_a, -1, -1, 0, 0, 0, 0, 10000);
                    CTheCarGenerators::CarGeneratorArray[carGenHId].SwitchOn();
                }
            }
        };
    }
}instance;

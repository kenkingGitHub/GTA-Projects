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
#include <unordered_set>
#include <string>
#include <fstream>

#define MODEL_POLICE_a 151
#define MODEL_POLICE_b 152
#define MODEL_POLICE_c 153
#define MODEL_AMBULAN_a 156
#define MODEL_FIRETRUK_a 158
#define MODEL_FBICAR_a 157
#define MODEL_COP_a 83
#define MODEL_COP_b 84
#define MODEL_COP_c 85

using namespace plugin;

class AddSpecialCars {
public:
    static unsigned int CurrentSpecialModelForSiren;
    static unsigned int CurrentSpecialModelForOccupants;
    static unsigned int RandomPolice;
    static unsigned int CurrentFiretrukModel;
    
    static unsigned int jmp_5373DE;
    static unsigned int jmp_4F5860;
    static unsigned int jmp_531FF1;
    
    static std::unordered_set<unsigned int> &GetTaxiModels() {
        static std::unordered_set<unsigned int> taxiIds;
        return taxiIds;
    }

    static int __stdcall GetSpecialModelForSiren(unsigned int model) {
        if (model == MODEL_POLICE || model == MODEL_POLICE_a || model == MODEL_POLICE_b || model == MODEL_POLICE_c)
            return MODEL_POLICE;
        else if (model == MODEL_TAXI || GetTaxiModels().find(model) != GetTaxiModels().end())
            return MODEL_TAXI;
        else if (model == MODEL_AMBULAN || model == MODEL_AMBULAN_a)
            return MODEL_AMBULAN;
        else if (model == MODEL_FBICAR || model == MODEL_FBICAR_a)
            return MODEL_FBICAR;
        else if (model == MODEL_FIRETRUK || model == MODEL_FIRETRUK_a)
            return MODEL_FIRETRUK;
        return model;
    }

    static int __stdcall GetSpecialModelForOccupants(unsigned int model) {
        if (model == MODEL_POLICE || model == MODEL_POLICE_a || model == MODEL_POLICE_b || model == MODEL_POLICE_c)
            return MODEL_POLICE;
        else if (model == MODEL_AMBULAN || model == MODEL_AMBULAN_a)
            return MODEL_AMBULAN;
        else if (model == MODEL_FBICAR || model == MODEL_FBICAR_a)
            return MODEL_FBICAR;
        else if (model == MODEL_FIRETRUK || model == MODEL_FIRETRUK_a)
            return MODEL_FIRETRUK;
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
            case MODEL_ENFORCER:
            case MODEL_BUS:
            case MODEL_RHINO:
            case MODEL_BARRACKS:
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
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case MODEL_POLICE_a:
        case MODEL_POLICE_b:
        case MODEL_POLICE_c:
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
                vehicle->SetUpDriver();
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
                    vehicle->SetupPassenger(0);
                break;
            case MODEL_POLICE_c:
                vehicle->SetUpDriver();
                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1) {
                    vehicle->SetupPassenger(0);
                    vehicle->SetupPassenger(1);
                    vehicle->SetupPassenger(2);
                }
                break;
            case MODEL_FBICAR:
            case MODEL_FBICAR_a:
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
        int result;

        CPlayerPed *player = FindPlayerPed();
        if (player) {
            
            if (CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_pWanted->m_nWantedLevel > 1 && CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_pWanted->m_nWantedLevel < 4) {
                if (RandomPolice < 2)
                    RandomPolice++;
                else
                    RandomPolice = 0;
                switch (RandomPolice) {
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
                default: result = MODEL_POLICE;  break;
                }
            }

            else if (player->m_pWanted->AreSwatRequired()
                && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == LOADSTATE_LOADED
                && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == LOADSTATE_LOADED)
            {
                int RandomSwat = plugin::Random(0, 1);
                switch (RandomSwat) {
                case 0:
                    if (CStreaming::ms_aInfoForModel[MODEL_POLICE_c].m_nLoadState == LOADSTATE_LOADED)
                        result = MODEL_POLICE_c;
                    else
                        result = MODEL_POLICE;
                    break;
                case 1: result = MODEL_ENFORCER; break;
                default: result = MODEL_POLICE;  break;
                }
            }
            else
            {
                if (player->m_pWanted->AreFbiRequired()
                    && CStreaming::ms_aInfoForModel[MODEL_FBICAR].m_nLoadState == LOADSTATE_LOADED
                    && CStreaming::ms_aInfoForModel[MODEL_FBI].m_nLoadState == LOADSTATE_LOADED)
                {
                    int RandomFbi = plugin::Random(0, 1);
                    if (RandomFbi) {
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
                        int RandomArmy = plugin::Random(0, 5);
                        if (RandomArmy < 4)
                            result = MODEL_BARRACKS;
                        else
                            result = MODEL_RHINO;
                    }
                    else {
                        if (RandomPolice < 2)
                            RandomPolice++;
                        else
                            RandomPolice = 0;
                        switch (RandomPolice) {
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
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case MODEL_POLICE_a:
        case MODEL_POLICE_b:
        case MODEL_POLICE_c:
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
        case MODEL_AMBULAN:
        case MODEL_FBICAR:
        case MODEL_FBICAR_a:
        case MODEL_MRWHOOP:
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_POLICE_a:
        case MODEL_POLICE_b:
        case MODEL_POLICE_c:
        case MODEL_AMBULAN_a:
        case MODEL_FIRETRUK_a:
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
                        case MODEL_AMBULAN:
                        case MODEL_MRWHOOP:
                        case MODEL_PREDATOR:
                        case MODEL_TRAIN:
                        case MODEL_SPEEDER:
                        case MODEL_REEFER:
                        case MODEL_GHOST:
                        case MODEL_AMBULAN_a:
                        case MODEL_FIRETRUK_a:
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
        case MODEL_AMBULAN_a:
        case MODEL_FIRETRUK_a:
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
            if (model == CTheScripts::ScriptParams[1].uParam)
                inModel = true;
            else if ((model == MODEL_POLICE_a || model == MODEL_POLICE_b || model == MODEL_POLICE_c) && CTheScripts::ScriptParams[1].uParam == MODEL_POLICE) // Vigilante
                inModel = true;
            else if (model == MODEL_AMBULAN_a && CTheScripts::ScriptParams[1].uParam == MODEL_AMBULAN) // Paramedic
                inModel = true;
            else if (model == MODEL_FBICAR_a && CTheScripts::ScriptParams[1].uParam == MODEL_FBICAR) // Vigilante
                inModel = true;
            else if (model == MODEL_FIRETRUK_a && CTheScripts::ScriptParams[1].uParam == MODEL_FIRETRUK) // Firefighter
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
        
        static int randomCopTime = 0;
        static unsigned int randomCop = 0;

        Events::gameProcessEvent += [] {
            if (CStreaming::ms_aInfoForModel[MODEL_POLICE_a].m_nLoadState == LOADSTATE_NOT_LOADED) 
                SetVehicleLoadState(MODEL_POLICE_a);
            if (CStreaming::ms_aInfoForModel[MODEL_POLICE_b].m_nLoadState == LOADSTATE_NOT_LOADED) 
                SetVehicleLoadState(MODEL_POLICE_b);
            if (CStreaming::ms_aInfoForModel[MODEL_POLICE_c].m_nLoadState == LOADSTATE_NOT_LOADED) 
                SetVehicleLoadState(MODEL_POLICE_c);
            if (CStreaming::ms_aInfoForModel[MODEL_AMBULAN_a].m_nLoadState == LOADSTATE_NOT_LOADED) 
                SetVehicleLoadState(MODEL_AMBULAN_a);
            if (CStreaming::ms_aInfoForModel[MODEL_FBICAR_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                SetVehicleLoadState(MODEL_FBICAR_a);
            if (CStreaming::ms_aInfoForModel[MODEL_FIRETRUK_a].m_nLoadState == LOADSTATE_NOT_LOADED)
                SetVehicleLoadState(MODEL_FIRETRUK_a);
            if (CStreaming::ms_aInfoForModel[MODEL_COP_a].m_nLoadState == LOADSTATE_NOT_LOADED) 
                SetPedLoadState(MODEL_COP_a);
            if (CStreaming::ms_aInfoForModel[MODEL_COP_b].m_nLoadState == LOADSTATE_NOT_LOADED)
                SetPedLoadState(MODEL_COP_b);
            if (CStreaming::ms_aInfoForModel[MODEL_COP_c].m_nLoadState == LOADSTATE_NOT_LOADED)
                SetPedLoadState(MODEL_COP_c);
            // Random Cop
            if (CTimer::m_snTimeInMilliseconds > (randomCopTime + 30000)) {
                randomCopTime = CTimer::m_snTimeInMilliseconds;
                int modelCop;
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
            }
        };
    }
}test;

unsigned int AddSpecialCars::CurrentSpecialModelForSiren;
unsigned int AddSpecialCars::CurrentSpecialModelForOccupants;
unsigned int AddSpecialCars::RandomPolice = 0;
unsigned int AddSpecialCars::CurrentFiretrukModel;

unsigned int AddSpecialCars::jmp_5373DE;
unsigned int AddSpecialCars::jmp_4F5860;
unsigned int AddSpecialCars::jmp_531FF1;

void __declspec(naked) AddSpecialCars::Patch_5373D7() { // Siren
    __asm {
        movsx eax, word ptr[ebp + 0x5C]
        pushad
        push eax
        call GetSpecialModelForSiren
        mov CurrentSpecialModelForSiren, eax
        popad
        mov eax, CurrentSpecialModelForSiren
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
        mov CurrentSpecialModelForOccupants, eax
        popad
        mov eax, CurrentSpecialModelForOccupants
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
        mov CurrentFiretrukModel, eax
        popad
        mov eax, CurrentFiretrukModel
        fldz
        cmp     eax, 61h
        mov jmp_531FF1, 0x531FF1
        jmp jmp_531FF1
    }
}

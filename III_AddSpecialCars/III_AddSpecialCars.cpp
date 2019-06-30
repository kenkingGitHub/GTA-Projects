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
#include <unordered_set>
#include <string>
#include <fstream>

#define m_MODEL_POLICE 156
#define m_MODEL_AMBULAN 157

using namespace plugin;

class AddSpecialCars {
public:
    static unsigned int CurrentSpecialModel;
    static unsigned int CurrentPoliceModel;

    static std::unordered_set<unsigned int> &GetTaxiModels() {
        static std::unordered_set<unsigned int> taxiIds;
        return taxiIds;
    }

    static int __stdcall GetSpecialModel(unsigned int model) {
        if (model == MODEL_POLICE || model == m_MODEL_POLICE)
            return MODEL_POLICE;
        else if (model == MODEL_TAXI || GetTaxiModels().find(model) != GetTaxiModels().end())
            return MODEL_TAXI;
        return model;
    }

    static int __stdcall GetPoliceModel(unsigned int model) {
        if (model == MODEL_POLICE || model == m_MODEL_POLICE)
            return MODEL_POLICE;
        return model;
    }

    static void Patch_5373D7();
    static void Patch_4F5857();

    static bool __fastcall IsLawEnforcementVehicle(CVehicle *_this) {
        bool result;

        switch (_this->m_nModelIndex) {
        case MODEL_FBICAR:
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case m_MODEL_POLICE:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    static void __cdecl AddPoliceCarOccupants(CVehicle *vehicle) {
        if (!vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated) {
            vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated = 1;
            switch (vehicle->m_nModelIndex) {
            case m_MODEL_POLICE:
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

    static int __cdecl ChoosePoliceCarModel() {
        int result;

        CPlayerPed *player = FindPlayerPed();
        if (player) {
            if (player->m_pWanted->AreSwatRequired()
                && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == LOADSTATE_LOADED
                && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == LOADSTATE_LOADED)
            {
                int _random = plugin::Random(0, 2);
                switch (_random) {
                case 0: result = MODEL_POLICE; break;
                case 1:
                    if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_LOADED)
                        result = m_MODEL_POLICE;
                    else
                        result = MODEL_POLICE;
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
                    result = MODEL_FBICAR;
                }
                else {
                    if (player->m_pWanted->AreArmyRequired()
                        && CStreaming::ms_aInfoForModel[MODEL_RHINO].m_nLoadState == LOADSTATE_LOADED
                        && CStreaming::ms_aInfoForModel[MODEL_BARRACKS].m_nLoadState == LOADSTATE_LOADED
                        && CStreaming::ms_aInfoForModel[MODEL_ARMY].m_nLoadState == LOADSTATE_LOADED)
                    {
                        int __random = plugin::Random(0, 10);
                        if (__random < 7)
                            result = MODEL_BARRACKS;
                        else
                            result = MODEL_RHINO;
                    }
                    else
                        result = MODEL_POLICE;
                }
            }
        }
        return result;
    }

    static bool __fastcall UsesPoliceRadio(cMusicManager *_this, int, CVehicle *vehicle) {
        bool result;

        switch (vehicle->m_nModelIndex) {
        case MODEL_FBICAR:
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case MODEL_RHINO:
        case MODEL_BARRACKS:
        case m_MODEL_POLICE:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    static bool __fastcall UsesSiren(CVehicle *_this, int, int vehicleModel) {
        bool result;

        switch (vehicleModel) {
        case MODEL_FIRETRUK:
        case MODEL_AMBULAN:
        case MODEL_FBICAR:
        case MODEL_MRWHOOP:
        case MODEL_POLICE:
        case MODEL_ENFORCER:
        case MODEL_PREDATOR:
        case m_MODEL_POLICE:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

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
                        case m_MODEL_POLICE:
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

    static bool __fastcall UsesSirenAudio(cAudioManager *_this, int, int index) {
        bool result;

        switch (index) { // eVehicleIndex
        case VEHICLE_FIRETRUK:
        case VEHICLE_AMBULAN:
        case VEHICLE_FBICAR:
        case VEHICLE_POLICE:
        case VEHICLE_ENFORCER:
        case VEHICLE_PREDATOR:
        case VEHICLE_156:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

    static bool __fastcall UsesSirenSwitching(cAudioManager *_this, int, int index) {
        bool result;

        switch (index) { // eVehicleIndex
        case VEHICLE_AMBULAN:
        case VEHICLE_POLICE:
        case VEHICLE_ENFORCER:
        case VEHICLE_PREDATOR:
        case VEHICLE_156:
            result = TRUE;
            break;
        default:
            result = FALSE;
            break;
        }
        return result;
    }

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
        case m_MODEL_POLICE:
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
            else if (model == m_MODEL_POLICE && CTheScripts::ScriptParams[1].uParam == MODEL_POLICE) // Vigilante
                inModel = true;
            //else if (model == m_MODEL_AMBULAN && CTheScripts::ScriptParams[1].uParam == MODEL_AMBULAN) // Paramedic
            //inModel = true;
        }
        script->UpdateCompareFlag(inModel);
    }

    //
    static void __fastcall SetModelIndex(CEntity *_this, int, unsigned int modelIndex) {
        if (modelIndex == MODEL_POLICE) {
            int _random = plugin::Random(0, 3);
            if (_random < 2) {
                if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_LOADED)
                    _this->m_nModelIndex = m_MODEL_POLICE;
                else
                    _this->m_nModelIndex = modelIndex;
            }
            else
                _this->m_nModelIndex = modelIndex;
        }
        else
            _this->m_nModelIndex = modelIndex;
        _this->CreateRwObject();
    }


    AddSpecialCars() {
        std::ifstream stream(PLUGIN_PATH("taxi.dat"));
        if (!stream.is_open())
            return;
        for (std::string line; getline(stream, line); ) {
            if (line.length() > 0 && line[0] != ';' && line[0] != '#')
                GetTaxiModels().insert(std::stoi(line));
        }

        patch::RedirectJump(0x473E70, SetModelIndex);

        patch::RedirectJump(0x552880, IsLawEnforcementVehicle);
        patch::RedirectJump(0x415C60, AddPoliceCarOccupants);
        patch::RedirectJump(0x4181F0, ChoosePoliceCarModel);
        patch::RedirectJump(0x57E6A0, UsesPoliceRadio);
        patch::RedirectJump(0x552200, UsesSiren);
        patch::RedirectJump(0x57E4B0, PlayerInCar);
        patch::RedirectJump(0x56C3C0, UsesSirenAudio);
        patch::RedirectJump(0x56C3F0, UsesSirenSwitching);
        patch::RedirectJump(0x426700, IsCarSprayable);

        patch::RedirectCall(0x446A93, OpcodePlayerDrivingTaxiVehicle);
        patch::Nop(0x446A98, 0x4C); // или сделать jump на 0x446AE4

        patch::RedirectCall(0x43DA19, OpcodeIsPlayerInModel);
        patch::Nop(0x43DA1E, 0x40); // или сделать jump на 0x43DA5E

        patch::RedirectJump(0x5373D7, Patch_5373D7);
        patch::RedirectJump(0x4F5857, Patch_4F5857);

        Events::gameProcessEvent += [] {
            if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_NOT_LOADED) {
                unsigned char oldFlags = CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nFlags;
                CStreaming::RequestModel(m_MODEL_POLICE, GAME_REQUIRED);
                CStreaming::LoadAllRequestedModels(false);
                if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_LOADED) {
                    if (!(oldFlags & GAME_REQUIRED)) {
                        CStreaming::SetModelIsDeletable(m_MODEL_POLICE);
                        CStreaming::SetModelTxdIsDeletable(m_MODEL_POLICE);
                    }
                    CVehicle *vehicle = nullptr;
                    vehicle = new CAutomobile(m_MODEL_POLICE, 1);
                }
            }
        };
    }
}test;

unsigned int AddSpecialCars::CurrentSpecialModel;
unsigned int AddSpecialCars::CurrentPoliceModel;

void __declspec(naked) AddSpecialCars::Patch_5373D7() {
    __asm {
        movsx eax, word ptr[ebp + 0x5C]
        pushad
        push eax
        call GetSpecialModel
        mov CurrentSpecialModel, eax
        popad
        mov eax, CurrentSpecialModel
        lea edx, [eax - 0x61]
        mov edi, 0x5373DE
        jmp edi
    }
}

void __declspec(naked) AddSpecialCars::Patch_4F5857() {
    __asm {
        movsx   eax, word ptr[ebx + 5Ch]
        pushad
        push eax
        call GetPoliceModel
        mov CurrentPoliceModel, eax
        popad
        mov eax, CurrentPoliceModel
        pop     ecx
        pop     ecx
        sub     eax, 61h
        mov edx, 0x4F5860
        jmp edx
    }
}


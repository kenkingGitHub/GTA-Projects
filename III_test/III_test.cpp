#include <plugin.h>
#include "extensions\KeyCheck.h"
#include "CMessages.h"
#include "CTheScripts.h"
#include "CWanted.h"
#include "CWorld.h"
#include "CStreaming.h"
#include "extensions\ScriptCommands.h"
#include "eScriptCommands.h"
#include "CCarAI.h"

//float &m_Distance = *(float *)0x5F07DC;
//bool b_Counter = false;
//int &NumAmbulancesOnDuty = *(int *)0x885BB0;
//bool &bReplayEnabled = *(bool *)0x617CAC;

using namespace plugin;

class Test {
public:
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

    Test() {
        static int blip;
        //Events::gameProcessEvent += [] {
            //KeyCheck::Update();
        Events::drawingEvent += [] {
            
            KeyCheck::Update();

            CPlayerPed *player = FindPlayerPed();
            if (player) {
                CVector posn = FindPlayerCentreOfWorld(CWorld::PlayerInFocus);
                CVector pos = player->m_matrix.pos;
                if (KeyCheck::CheckWithDelay('B', 2000)) {
                    CVector offset = { 0.0f, 10.0f, 0.0f };
                    CVector position = player->m_matrix * offset;
                    //Command<COMMAND_ADD_BLIP_FOR_COORD_OLD>(point.x, point.y, point.z, 1, 3, &blip);
                    int modelIndex = 156;
                    if (LoadModel(modelIndex) && LoadModel(5)) {
                        CVehicle *vehicle = nullptr;
                        vehicle = new CAutomobile(modelIndex, 1);
                        if (vehicle) {
                            // –азмещаем транспорт в игровом мире
                            vehicle->SetPosition(position);
                            //vehicle->SetOrientation(0.0f, 0.0f, orientation);
                            vehicle->m_nState = 4;
                            CWorld::Add(vehicle);
                            CTheScripts::ClearSpaceForMissionEntity(position, vehicle);
                            reinterpret_cast<CAutomobile *>(vehicle)->PlaceOnRoadProperly();
                            //vehicle->SetUpDriver();
                            //Command<COMMAND_SET_CAR_CRUISE_SPEED>(CPools::GetVehicleRef(vehicle), 25.0f);
                            //Command<COMMAND_SET_CAR_MISSION>(CPools::GetVehicleRef(vehicle), 11);
                            CCarAI::AddAmbulanceOccupants(vehicle);
                            Command<COMMAND_CAR_GOTO_COORDINATES>(CPools::GetVehicleRef(vehicle), -1183.0f, 286.7f, 3.8f);
                            if (plugin::Random(0, 1)) {
                                vehicle->m_nSirenOrAlarm = true;
                                vehicle->m_autoPilot.m_nDrivingStyle = DRIVINGSTYLE_AVOID_CARS;
                                vehicle->m_autoPilot.m_nCruiseSpeed = 25;
                            }
                            else {
                                vehicle->m_nSirenOrAlarm = false;
                                vehicle->m_autoPilot.m_nDrivingStyle = DRIVINGSTYLE_STOP_FOR_CARS;
                                vehicle->m_autoPilot.m_nCruiseSpeed = 10;
                            }


                        }
                    }
                }
                //if (KeyCheck::CheckWithDelay('N', 2000)) 
                    //Command<COMMAND_REMOVE_BLIP>(blip);

                gamefont::Print({
                    Format("cop = %d", patch::GetUChar(0x4C11F2)),
                    Format("swat = %d", patch::GetUChar(0x4C1241)),
                    Format("fbi = %d", patch::GetUChar(0x4C12A0)),
                    Format("army = %d", patch::GetUChar(0x4C12FC)),
                    Format("pos = %.2f, %.2f, %.2f", posn.x, posn.y, posn.z),
                    Format("pos = %.2f, %.2f, %.2f", pos.x, pos.y, pos.z)
                }, 10, 400, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);
            }
            
            
            /*if (KeyCheck::CheckWithDelay('N', 2000)) {
                if (CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_pWanted->m_nWantedLevel < 6)
                    CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_pWanted->m_nWantedLevel++;
                else
                    CWorld::Players[CWorld::PlayerInFocus].m_pPed->m_pWanted->m_nWantedLevel = 0;
            }*/
            
            /*if (KeyCheck::CheckWithDelay('O', 1000)) {
                if (b_Counter) {
                    m_Distance = 2.0f; b_Counter = false;
                }
                else {
                    m_Distance = -2.0f; b_Counter = true;
                }
            }*/
            
            if (KeyCheck::CheckWithDelay('M', 1000)) {
                //CMessages::AddMessageJumpQ(L"Yes", 2000, 1);
                static char message[256];
                snprintf(message, 256, "random %d", plugin::Random(123456, 98765));
                CMessages::AddMessageJumpQ(message, 5000, false);
            }
        };
    }
}test;


//#include <plugin.h>
//#include "extensions\KeyCheck.h"
//#include "CStreaming.h"
//#include "eVehicleModel.h"
//#include "ePedModel.h"
//#include "CMessages.h"
//
//bool __cdecl /*CCarCtrl::*/GenerateOneEmergencyServicesCar(int vehicleModelIndex, CVector point) {
//    return ((bool(__cdecl *)(int, CVector))0x41FE50)(vehicleModelIndex, point);
//};
//
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::drawingEvent += [] {
//            gamefont::Print({
//                Format("LANDSTAL LoadState = %d", CStreaming::ms_aInfoForModel[MODEL_LANDSTAL].m_nLoadState),
//                Format("IDAHO LoadState = %d", CStreaming::ms_aInfoForModel[MODEL_IDAHO].m_nLoadState),
//                Format("PEREN LoadState = %d", CStreaming::ms_aInfoForModel[MODEL_PEREN].m_nLoadState),
//                Format("FIRETRUK LoadState = %d", CStreaming::ms_aInfoForModel[MODEL_FIRETRUK].m_nLoadState),
//                Format("BARRACKS LoadState = %d", CStreaming::ms_aInfoForModel[MODEL_BARRACKS].m_nLoadState)
//            }, 10, 200, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange); 
//
//            CPlayerPed *player = FindPlayerPed();
//            if (player) {
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay('M', 1000)) {
//                    CStreaming::RequestModel(MODEL_AMBULAN, MISSION_REQUIRED);
//                    CStreaming::RequestModel(MODEL_MEDIC, 1);
//                    if (CStreaming::ms_aInfoForModel[MODEL_AMBULAN].m_nLoadState == LOADSTATE_LOADED
//                        && CStreaming::ms_aInfoForModel[MODEL_MEDIC].m_nLoadState == LOADSTATE_LOADED)
//                    {
//                        if (/*CCarCtrl::*/GenerateOneEmergencyServicesCar(MODEL_AMBULAN, FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 20.0f, 0.0f))))
//                            CMessages::AddMessageJumpQ(L"AmbulanceCreate", 2000, 0);
//                    }
//                }
//            }
//        };
//    }
//}test;


//#include "plugin.h"
//#include "CWanted.h"
//#include "CStreaming.h"
//#include "CWorld.h"
//#include "eVehicleModel.h"
//#include "ePedModel.h"
//#include "eVehicleIndex.h"
//#include "cMusicManager.h"
//#include "cAudioManager.h"
//#include "CTheScripts.h"
//#include <unordered_set>
//#include <string>
//#include <fstream>
//
//#include "CMessages.h"
//
//#define m_MODEL_POLICE 156
//#define m_MODEL_AMBULAN 157
//#define m_MODEL_FIRETRUK 158
//
//using namespace plugin;
//
//class AddSpecialCars {
//public:
//    static unsigned int CurrentSpecialModelForSiren;
//    static unsigned int CurrentSpecialModelForOccupants;
//    static unsigned int CurrentFiretrukModel;
//
//    static std::unordered_set<unsigned int> &GetTaxiModels() {
//        static std::unordered_set<unsigned int> taxiIds;
//        return taxiIds;
//    }
//
//    static int __stdcall GetSpecialModelForSiren(unsigned int model) {
//        if (model == MODEL_POLICE || model == m_MODEL_POLICE)
//            return MODEL_POLICE;
//        else if (model == MODEL_AMBULAN || model == m_MODEL_AMBULAN)
//            return MODEL_AMBULAN;
//        else if (model == MODEL_FIRETRUK || model == m_MODEL_FIRETRUK)
//            return MODEL_FIRETRUK;
//        else if (model == MODEL_TAXI || GetTaxiModels().find(model) != GetTaxiModels().end())
//            return MODEL_TAXI;
//        return model;
//    }
//
//    static int __stdcall GetSpecialModelForOccupants(unsigned int model) {
//        if (model == MODEL_POLICE || model == m_MODEL_POLICE)
//            return MODEL_POLICE;
//        else if (model == MODEL_AMBULAN || model == m_MODEL_AMBULAN)
//            return MODEL_AMBULAN;
//        else if (model == MODEL_FIRETRUK || model == m_MODEL_FIRETRUK)
//            return MODEL_FIRETRUK;
//        return model;
//    }
//
//    static int __stdcall GetFiretrukModel(unsigned int model) {
//        if (model == MODEL_FIRETRUK || model == m_MODEL_FIRETRUK)
//            return MODEL_FIRETRUK;
//        return model;
//    }
//
//    static void Patch_5373D7(); // Siren
//    static void Patch_4F5857(); // AddPedInCar
//    static void Patch_531FE8(); // FireTruckControl
//
//    static bool __fastcall IsLawEnforcementVehicle(CVehicle *_this) {
//        bool result;
//
//        switch (_this->m_nModelIndex) {
//        case MODEL_FBICAR:
//        case MODEL_POLICE:
//        case MODEL_ENFORCER:
//        case MODEL_PREDATOR:
//        case MODEL_RHINO:
//        case MODEL_BARRACKS:
//        case m_MODEL_POLICE:
//            result = TRUE;
//            break;
//        default:
//            result = FALSE;
//            break;
//        }
//        return result;
//    }
//
//    static void __cdecl AddPoliceCarOccupants(CVehicle *vehicle) {
//        if (!vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated) {
//            vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated = 1;
//            switch (vehicle->m_nModelIndex) {
//            case m_MODEL_POLICE:
//            case MODEL_POLICE:
//            case MODEL_RHINO:
//            case MODEL_BARRACKS:
//                vehicle->SetUpDriver();
//                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
//                    vehicle->SetupPassenger(0);
//                break;
//            case MODEL_FBICAR:
//            case MODEL_ENFORCER:
//                vehicle->SetUpDriver();
//                vehicle->SetupPassenger(0);
//                vehicle->SetupPassenger(1);
//                vehicle->SetupPassenger(2);
//                break;
//            default:
//                return;
//            }
//        }
//    }
//
//    static int __cdecl ChoosePoliceCarModel() {
//        int result;
//
//        CPlayerPed *player = FindPlayerPed();
//        if (player) {
//            if (player->m_pWanted->AreSwatRequired()
//                && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == LOADSTATE_LOADED
//                && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == LOADSTATE_LOADED)
//            {
//                int _random = plugin::Random(0, 2);
//                switch (_random) {
//                case 0: result = MODEL_POLICE; break;
//                case 1:
//                    if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_LOADED)
//                        result = m_MODEL_POLICE;
//                    else
//                        result = MODEL_POLICE;
//                    break;
//                case 2: result = MODEL_ENFORCER; break;
//                default: result = MODEL_POLICE;  break;
//                }
//            }
//            else
//            {
//                if (player->m_pWanted->AreFbiRequired()
//                    && CStreaming::ms_aInfoForModel[MODEL_FBICAR].m_nLoadState == LOADSTATE_LOADED
//                    && CStreaming::ms_aInfoForModel[MODEL_FBI].m_nLoadState == LOADSTATE_LOADED)
//                {
//                    result = MODEL_FBICAR;
//                }
//                else {
//                    if (player->m_pWanted->AreArmyRequired()
//                        && CStreaming::ms_aInfoForModel[MODEL_RHINO].m_nLoadState == LOADSTATE_LOADED
//                        && CStreaming::ms_aInfoForModel[MODEL_BARRACKS].m_nLoadState == LOADSTATE_LOADED
//                        && CStreaming::ms_aInfoForModel[MODEL_ARMY].m_nLoadState == LOADSTATE_LOADED)
//                    {
//                        int __random = plugin::Random(0, 10);
//                        if (__random < 7)
//                            result = MODEL_BARRACKS;
//                        else
//                            result = MODEL_RHINO;
//                    }
//                    else
//                        result = MODEL_POLICE;
//                }
//            }
//        }
//        return result;
//    }
//
//    static bool __fastcall UsesPoliceRadio(cMusicManager *_this, int, CVehicle *vehicle) {
//        bool result;
//
//        switch (vehicle->m_nModelIndex) {
//        case MODEL_FBICAR:
//        case MODEL_POLICE:
//        case MODEL_ENFORCER:
//        case MODEL_PREDATOR:
//        case MODEL_RHINO:
//        case MODEL_BARRACKS:
//        case m_MODEL_POLICE:
//            result = TRUE;
//            break;
//        default:
//            result = FALSE;
//            break;
//        }
//        return result;
//    }
//
//    static bool __fastcall UsesSiren(CVehicle *_this, int, int vehicleModel) {
//        bool result;
//
//        switch (vehicleModel) {
//        case MODEL_FIRETRUK:
//        case MODEL_AMBULAN:
//        case MODEL_FBICAR:
//        case MODEL_MRWHOOP:
//        case MODEL_POLICE:
//        case MODEL_ENFORCER:
//        case MODEL_PREDATOR:
//        case m_MODEL_POLICE:
//        case m_MODEL_AMBULAN:
//        case m_MODEL_FIRETRUK:
//            result = TRUE;
//            break;
//        default:
//            result = FALSE;
//            break;
//        }
//        return result;
//    }
//
//    static bool __fastcall PlayerInCar(cMusicManager *_this) {
//        int action;
//        bool result;
//
//        if (FindPlayerVehicle()) {
//            action = FindPlayerPed()->m_nState;
//            if (action != 51 && action != 54 && action != 56) {
//                CVehicle *vehicle = FindPlayerVehicle();
//                if (vehicle) {
//                    if (vehicle->m_nState == 5)
//                        result = FALSE;
//                    else {
//                        switch (vehicle->m_nModelIndex) {
//                        case MODEL_FIRETRUK:
//                        case MODEL_AMBULAN:
//                        case MODEL_MRWHOOP:
//                        case MODEL_PREDATOR:
//                        case MODEL_TRAIN:
//                        case MODEL_SPEEDER:
//                        case MODEL_REEFER:
//                        case MODEL_GHOST:
//                        case m_MODEL_POLICE:
//                        case m_MODEL_AMBULAN:
//                        case m_MODEL_FIRETRUK:
//                            result = FALSE;
//                            break;
//                        default:
//                            goto LABEL_11;
//                        }
//                    }
//                }
//                else {
//                LABEL_11:
//                    result = TRUE;
//                }
//            }
//            else
//                result = FALSE;
//        }
//        else
//            result = FALSE;
//
//        return result;
//    }
//
//    static bool __fastcall UsesSirenAudio(cAudioManager *_this, int, int index) {
//        bool result;
//
//        switch (index) { // eVehicleIndex
//        case VEHICLE_FIRETRUK:
//        case VEHICLE_AMBULAN:
//        case VEHICLE_FBICAR:
//        case VEHICLE_POLICE:
//        case VEHICLE_ENFORCER:
//        case VEHICLE_PREDATOR:
//        case VEHICLE_156:
//        case VEHICLE_157:
//        case VEHICLE_158:
//            result = TRUE;
//            break;
//        default:
//            result = FALSE;
//            break;
//        }
//        return result;
//    }
//
//    static bool __fastcall UsesSirenSwitching(cAudioManager *_this, int, int index) {
//        bool result;
//
//        switch (index) { // eVehicleIndex
//        case VEHICLE_AMBULAN:
//        case VEHICLE_POLICE:
//        case VEHICLE_ENFORCER:
//        case VEHICLE_PREDATOR:
//        case VEHICLE_156:
//        case VEHICLE_157:
//            result = TRUE;
//            break;
//        default:
//            result = FALSE;
//            break;
//        }
//        return result;
//    }
//
//    static bool __cdecl IsCarSprayable(CAutomobile *car) {
//        bool result;
//
//        switch (car->m_nModelIndex) {
//        case MODEL_FIRETRUK:
//        case MODEL_AMBULAN:
//        case MODEL_POLICE:
//        case MODEL_ENFORCER:
//        case MODEL_BUS:
//        case MODEL_RHINO:
//        case MODEL_BARRACKS:
//        case MODEL_DODO:
//        case MODEL_COACH:
//        case m_MODEL_POLICE:
//        case m_MODEL_AMBULAN:
//        case m_MODEL_FIRETRUK:
//            result = FALSE;
//            break;
//        default:
//            result = TRUE;
//            break;
//        }
//        return result;
//    }
//
//    static void __fastcall OpcodePlayerDrivingTaxiVehicle(CRunningScript *script) {
//        script->CollectParameters(&script->m_nIp, 1);
//        bool isTaxiModel = false;
//
//        CPlayerPed * player = CWorld::Players[CTheScripts::ScriptParams[0].uParam].m_pPed;
//        if (player->m_bInVehicle) {
//            unsigned int model = player->m_pVehicle->m_nModelIndex;
//            if (model == MODEL_TAXI || model == MODEL_CABBIE || model == MODEL_BORGNINE || GetTaxiModels().find(model) != GetTaxiModels().end())
//                isTaxiModel = true;
//        }
//        script->UpdateCompareFlag(isTaxiModel);
//    }
//
//    static void __fastcall OpcodeIsPlayerInModel(CRunningScript *script) {
//        script->CollectParameters(&script->m_nIp, 2);
//        bool inModel = false;
//
//        CPlayerPed * player = CWorld::Players[CTheScripts::ScriptParams[0].uParam].m_pPed;
//        if (player->m_bInVehicle) {
//            unsigned int model = player->m_pVehicle->m_nModelIndex;
//            if (model == CTheScripts::ScriptParams[1].uParam)
//                inModel = true;
//            else if (model == m_MODEL_POLICE && CTheScripts::ScriptParams[1].uParam == MODEL_POLICE) // Vigilante
//                inModel = true;
//            else if (model == m_MODEL_AMBULAN && CTheScripts::ScriptParams[1].uParam == MODEL_AMBULAN) // Paramedic
//                inModel = true;
//            else if (model == m_MODEL_FIRETRUK && CTheScripts::ScriptParams[1].uParam == MODEL_FIRETRUK) // Firefighter
//                inModel = true;
//        }
//        script->UpdateCompareFlag(inModel);
//    }
//
//    static void __fastcall SetModelIndex(CEntity *_this, int, unsigned int modelIndex) {
//        int _random = plugin::Random(0, 3);
//        switch (modelIndex) {
//        case MODEL_POLICE:
//            if (_random == 0 || _random == 1) {
//                if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_LOADED)
//                    _this->m_nModelIndex = m_MODEL_POLICE;
//                else
//                    _this->m_nModelIndex = modelIndex;
//            }
//            break;
//        case MODEL_AMBULAN:
//            if (_random == 1 || _random == 2) {
//                if (CStreaming::ms_aInfoForModel[m_MODEL_AMBULAN].m_nLoadState == LOADSTATE_LOADED)
//                    _this->m_nModelIndex = m_MODEL_AMBULAN;
//                else
//                    _this->m_nModelIndex = modelIndex;
//            }
//            break;
//        default:
//            _this->m_nModelIndex = modelIndex;
//            break;
//        }
//        _this->CreateRwObject();
//    }
//
//    static bool LoadModel(int model) {
//        unsigned char oldFlags = CStreaming::ms_aInfoForModel[model].m_nFlags;
//        CStreaming::RequestModel(model, GAME_REQUIRED);
//        CStreaming::LoadAllRequestedModels(false);
//        if (CStreaming::ms_aInfoForModel[model].m_nLoadState == LOADSTATE_LOADED) {
//            if (!(oldFlags & GAME_REQUIRED)) {
//                CStreaming::SetModelIsDeletable(model);
//                CStreaming::SetModelTxdIsDeletable(model);
//            }
//            return true;
//        }
//        return false;
//    }
//
//    AddSpecialCars() {
//        std::ifstream stream(PLUGIN_PATH("taxi.dat"));
//        if (!stream.is_open())
//            return;
//        for (std::string line; getline(stream, line); ) {
//            if (line.length() > 0 && line[0] != ';' && line[0] != '#')
//                GetTaxiModels().insert(std::stoi(line));
//        }
//
//        patch::RedirectJump(0x473E70, SetModelIndex);
//
//        patch::RedirectJump(0x552880, IsLawEnforcementVehicle);
//        patch::RedirectJump(0x415C60, AddPoliceCarOccupants);
//        patch::RedirectJump(0x4181F0, ChoosePoliceCarModel);
//        patch::RedirectJump(0x57E6A0, UsesPoliceRadio);
//        patch::RedirectJump(0x552200, UsesSiren);
//        patch::RedirectJump(0x57E4B0, PlayerInCar);
//        patch::RedirectJump(0x56C3C0, UsesSirenAudio);
//        patch::RedirectJump(0x56C3F0, UsesSirenSwitching);
//        patch::RedirectJump(0x426700, IsCarSprayable);
//
//        patch::RedirectCall(0x446A93, OpcodePlayerDrivingTaxiVehicle);
//        patch::Nop(0x446A98, 0x4C); // или сделать jump на 0x446AE4
//
//        patch::RedirectCall(0x43DA19, OpcodeIsPlayerInModel);
//        patch::Nop(0x43DA1E, 0x40); // или сделать jump на 0x43DA5E
//
//        patch::RedirectJump(0x5373D7, Patch_5373D7);
//        patch::RedirectJump(0x4F5857, Patch_4F5857);
//        patch::RedirectJump(0x531FE8, Patch_531FE8);
//
//        Events::gameProcessEvent += [] {
//            if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_NOT_LOADED) {
//                if (LoadModel(m_MODEL_POLICE)) {
//                    CVehicle *police = nullptr;
//                    police = new CAutomobile(m_MODEL_POLICE, 1);
//                }
//            }
//            if (CStreaming::ms_aInfoForModel[m_MODEL_AMBULAN].m_nLoadState == LOADSTATE_NOT_LOADED) {
//                if (LoadModel(m_MODEL_AMBULAN)) {
//                    CVehicle *ambulan = nullptr;
//                    ambulan = new CAutomobile(m_MODEL_AMBULAN, 1);
//                }
//            }
//        };
//    }
//}test;
//
//unsigned int AddSpecialCars::CurrentSpecialModelForSiren;
//unsigned int AddSpecialCars::CurrentSpecialModelForOccupants;
//unsigned int AddSpecialCars::CurrentFiretrukModel;
//
//void __declspec(naked) AddSpecialCars::Patch_5373D7() {
//    __asm {
//        movsx eax, word ptr[ebp + 0x5C]
//        pushad
//        push eax
//        call GetSpecialModelForSiren
//        mov CurrentSpecialModelForSiren, eax
//        popad
//        mov eax, CurrentSpecialModelForSiren
//        lea edx, [eax - 0x61]
//        mov edi, 0x5373DE
//        jmp edi
//    }
//}
//
//void __declspec(naked) AddSpecialCars::Patch_4F5857() {
//    __asm {
//        movsx   eax, word ptr[ebx + 5Ch]
//        pushad
//        push eax
//        call GetSpecialModelForOccupants
//        mov CurrentSpecialModelForOccupants, eax
//        popad
//        mov eax, CurrentSpecialModelForOccupants
//        pop     ecx
//        pop     ecx
//        sub     eax, 61h
//        mov edx, 0x4F5860
//        jmp edx
//    }
//}
//
//void __declspec(naked) AddSpecialCars::Patch_531FE8() {
//    __asm {
//        movsx eax, word ptr[ebp + 0x5C]
//        pushad
//        push eax
//        call GetFiretrukModel
//        mov CurrentFiretrukModel, eax
//        popad
//        mov eax, CurrentFiretrukModel
//        fldz
//        cmp     eax, 61h
//        mov ecx, 0x531FF1
//        jmp ecx
//    }
//}


//#include "plugin.h"
//#include "CWanted.h"
//#include "CStreaming.h"
//#include "CWorld.h"
//#include "eVehicleModel.h"
//#include "ePedModel.h"
//#include "eVehicleIndex.h"
//#include "cMusicManager.h"
//#include "cAudioManager.h"
//#include "CTheScripts.h"
//#include "CTheCarGenerators.h"
//#include <unordered_set>
//#include <string>
//#include <fstream>
//
//#include "CMessages.h"
////#include "extensions\KeyCheck.h"
//
//#define m_MODEL_POLICE 156
//#define m_MODEL_AMBULAN 157
//
//using namespace plugin;
//
////class MyCarGenerator {
////public:
////    MyCarGenerator() {
////        static CdeclEvent<AddressList<0x582E6C, H_CALL, 0x48C7CC, H_CALL>, PRIORITY_AFTER, ArgPickNone, void()> myOnInitGame;
////        
////        static float fCarGenAngleOne, fCarGenAngleTwo, fCarGenAngleThree;
////        fCarGenAngleOne = fCarGenAngleTwo = fCarGenAngleThree = 0.0f;
////        static CVector vCarGenPositionOne = { 1148.76f, -690.0f, 14.0f };
////        static CVector vCarGenPositionTwo = { 347.0f, -1170.25f, 22.0f };
////        static CVector vCarGenPositionThree = { -1261.84f, - 21.49f, 58.5f };
////
////        myOnInitGame += [] {
////            if (CTheCarGenerators::NumOfCarGenerators < 160) {
////                bool alreadyRegisteredOne, alreadyRegisteredTwo, alreadyRegisteredThree;
////                alreadyRegisteredOne = alreadyRegisteredTwo = alreadyRegisteredThree = false;
////                for (int i = 0; i < CTheCarGenerators::NumOfCarGenerators; i++) {
////                    CCarGenerator &carGenOne = CTheCarGenerators::CarGeneratorArray[i];
////                    if (carGenOne.m_nEnabled && DistanceBetweenPoints(vCarGenPositionOne, carGenOne.m_vecPos) < 1.0f && carGenOne.m_fAngle == fCarGenAngleOne && carGenOne.m_nModelId == m_MODEL_POLICE)
////                        alreadyRegisteredOne = true;
////                    CCarGenerator &carGenTwo = CTheCarGenerators::CarGeneratorArray[i];
////                    if (carGenTwo.m_nEnabled && DistanceBetweenPoints(vCarGenPositionTwo, carGenTwo.m_vecPos) < 1.0f && carGenTwo.m_fAngle == fCarGenAngleTwo && carGenTwo.m_nModelId == m_MODEL_POLICE)
////                        alreadyRegisteredTwo = true;
////                    CCarGenerator &carGenThree = CTheCarGenerators::CarGeneratorArray[i];
////                    if (carGenThree.m_nEnabled && DistanceBetweenPoints(vCarGenPositionThree, carGenThree.m_vecPos) < 1.0f && carGenThree.m_fAngle == fCarGenAngleThree && carGenThree.m_nModelId == m_MODEL_POLICE)
////                        alreadyRegisteredThree = true;
////                }
////                if (!alreadyRegisteredOne) {
////                    unsigned int carGenOneId = CTheCarGenerators::CreateCarGenerator(vCarGenPositionOne.x, vCarGenPositionOne.y, vCarGenPositionOne.z, 0.0f, m_MODEL_POLICE, -1, -1, 0, 0, 0, 0, 10000);
////                    CTheCarGenerators::CarGeneratorArray[carGenOneId].SwitchOn();
////                }
////                if (!alreadyRegisteredTwo) {
////                    unsigned int carGenTwoId = CTheCarGenerators::CreateCarGenerator(vCarGenPositionTwo.x, vCarGenPositionTwo.y, vCarGenPositionTwo.z, 0.0f, m_MODEL_POLICE, -1, -1, 0, 0, 0, 0, 10000);
////                    CTheCarGenerators::CarGeneratorArray[carGenTwoId].SwitchOn();
////                }
////                if (!alreadyRegisteredThree) {
////                    unsigned int carGenThreeId = CTheCarGenerators::CreateCarGenerator(vCarGenPositionThree.x, vCarGenPositionThree.y, vCarGenPositionThree.z, 173.23f, m_MODEL_POLICE, -1, -1, 0, 0, 0, 0, 10000);
////                    CTheCarGenerators::CarGeneratorArray[carGenThreeId].SwitchOn();
////                }
////            }
////        };
////    }
////} instance;
//
//class Test {
//public:
//    static unsigned int CurrentSpecialModel;
//    static unsigned int CurrentPoliceModel;
//
//    static std::unordered_set<unsigned int> &GetTaxiModels() {
//        static std::unordered_set<unsigned int> taxiIds;
//        return taxiIds;
//    }
//
//    static int __stdcall GetSpecialModel(unsigned int model) {
//        if (model == MODEL_POLICE || model == m_MODEL_POLICE)
//            return MODEL_POLICE;
//        else if (model == MODEL_TAXI || GetTaxiModels().find(model) != GetTaxiModels().end())
//            return MODEL_TAXI;
//        return model;
//    }
//
//    static int __stdcall GetPoliceModel(unsigned int model) {
//        if (model == MODEL_POLICE || model == m_MODEL_POLICE)
//            return MODEL_POLICE;
//        return model;
//    }
//
//    static void Patch_5373D7();
//    static void Patch_4F5857();
//
//    static bool __fastcall IsLawEnforcementVehicle(CVehicle *_this) {
//        bool result;
//
//        switch (_this->m_nModelIndex) {
//        case MODEL_FBICAR:
//        case MODEL_POLICE:
//        case MODEL_ENFORCER:
//        case MODEL_PREDATOR:
//        case MODEL_RHINO:
//        case MODEL_BARRACKS:
//        case m_MODEL_POLICE:
//            result = TRUE;
//            break;
//        default:
//            result = FALSE;
//            break;
//        }
//        return result;
//    }
//
//    static void __cdecl AddPoliceCarOccupants(CVehicle *vehicle) {
//        if (!vehicle->m_nVehicleFlags.b25) {
//            vehicle->m_nVehicleFlags.b25 = 1;// vehicle->m_nVehicleFlags.bOccupantsHaveBeenGenerated = 1;
//            int _random = plugin::Random(0, 2);
//            switch (vehicle->m_nModelIndex) {
//            case m_MODEL_POLICE:
//                vehicle->SetUpDriver();
//                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
//                    vehicle->SetupPassenger(_random);
//                break;
//            case MODEL_POLICE:
//            case MODEL_RHINO:
//            case MODEL_BARRACKS:
//                vehicle->SetUpDriver();
//                if (FindPlayerPed()->m_pWanted->m_nWantedLevel > 1)
//                    vehicle->SetupPassenger(0);
//                break;
//            case MODEL_FBICAR:
//            case MODEL_ENFORCER:
//                vehicle->SetUpDriver();
//                vehicle->SetupPassenger(0);
//                vehicle->SetupPassenger(1);
//                vehicle->SetupPassenger(2);
//                break;
//            default:
//                return;
//            }
//        }
//    }
//
//    static int __cdecl ChoosePoliceCarModel() {
//        int result;
//
//        CPlayerPed *player = FindPlayerPed();
//        if (player) {
//            if (player->m_pWanted->AreSwatRequired()
//                && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == LOADSTATE_LOADED
//                && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == LOADSTATE_LOADED)
//            {
//                int _random = plugin::Random(0, 2);
//                switch (_random) { 
//                case 0: result = MODEL_POLICE; break;
//                case 1:
//                    if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_LOADED)
//                        result = m_MODEL_POLICE;
//                    else
//                        result = MODEL_POLICE;
//                    break;
//                case 2: result = MODEL_ENFORCER; break;
//                default: result = MODEL_POLICE;  break;
//                }
//            }
//            else
//            {
//                if (player->m_pWanted->AreFbiRequired()
//                    && CStreaming::ms_aInfoForModel[MODEL_FBICAR].m_nLoadState == LOADSTATE_LOADED
//                    && CStreaming::ms_aInfoForModel[MODEL_FBI].m_nLoadState == LOADSTATE_LOADED)
//                {
//                    result = MODEL_FBICAR;
//                }
//                else {
//                    if (player->m_pWanted->AreArmyRequired()
//                        && CStreaming::ms_aInfoForModel[MODEL_RHINO].m_nLoadState == LOADSTATE_LOADED
//                        && CStreaming::ms_aInfoForModel[MODEL_BARRACKS].m_nLoadState == LOADSTATE_LOADED
//                        && CStreaming::ms_aInfoForModel[MODEL_ARMY].m_nLoadState == LOADSTATE_LOADED)
//                    {
//                        int __random = plugin::Random(0, 10);
//                        if (__random < 7)
//                            result = MODEL_BARRACKS;
//                        else
//                            result = MODEL_RHINO;
//                    }
//                    else
//                        result = MODEL_POLICE;
//                }
//            }
//        }
//        return result;
//    }
//
//    static bool __fastcall UsesPoliceRadio(cMusicManager *_this, int, CVehicle *vehicle) {
//        bool result;
//
//        switch (vehicle->m_nModelIndex) {
//        case MODEL_FBICAR:
//        case MODEL_POLICE:
//        case MODEL_ENFORCER:
//        case MODEL_PREDATOR:
//        case MODEL_RHINO:
//        case MODEL_BARRACKS:
//        case m_MODEL_POLICE:
//            result = TRUE;
//            break;
//        default:
//            result = FALSE;
//            break;
//        }
//        return result;
//    }
//
//    static bool __fastcall UsesSiren(CVehicle *_this, int, int vehicleModel) {
//        bool result;
//
//        switch (vehicleModel) {
//        case MODEL_FIRETRUK:
//        case MODEL_AMBULAN:
//        case MODEL_FBICAR:
//        case MODEL_MRWHOOP:
//        case MODEL_POLICE:
//        case MODEL_ENFORCER:
//        case MODEL_PREDATOR:
//        case m_MODEL_POLICE:
//            result = TRUE;
//            break;
//        default:
//            result = FALSE;
//            break;
//        }
//        return result;
//    }
//
//    static bool __fastcall PlayerInCar(cMusicManager *_this) {
//        int action;
//        bool result;
//
//        if (FindPlayerVehicle()) {
//            action = FindPlayerPed()->m_nState;
//            if (action != 51 && action != 54 && action != 56) {
//                CVehicle *vehicle = FindPlayerVehicle();
//                if (vehicle) {
//                    if (vehicle->m_nState == 5)
//                        result = FALSE;
//                    else {
//                        switch (vehicle->m_nModelIndex) {
//                        case MODEL_FIRETRUK:
//                        case MODEL_AMBULAN:
//                        case MODEL_MRWHOOP:
//                        case MODEL_PREDATOR:
//                        case MODEL_TRAIN:
//                        case MODEL_SPEEDER:
//                        case MODEL_REEFER:
//                        case MODEL_GHOST:
//                        case m_MODEL_POLICE:
//                            result = FALSE;
//                            break;
//                        default:
//                            goto LABEL_11;
//                        }
//                    }
//                }
//                else {
//                LABEL_11:
//                    result = TRUE;
//                }
//            }
//            else
//                result = FALSE;
//        }
//        else
//            result = FALSE;
//
//        return result;
//    }
//
//    static bool __fastcall UsesSirenAudio(cAudioManager *_this, int, int index) {
//        bool result;
//
//        switch (index) { // eVehicleIndex
//        case VEHICLE_FIRETRUK:
//        case VEHICLE_AMBULAN:
//        case VEHICLE_FBICAR:
//        case VEHICLE_POLICE:
//        case VEHICLE_ENFORCER:
//        case VEHICLE_PREDATOR:
//        case VEHICLE_156:
//            result = TRUE;
//            break;
//        default:
//            result = FALSE;
//            break;
//        }
//        return result;
//    }
//
//    static bool __fastcall UsesSirenSwitching(cAudioManager *_this, int, int index) {
//        bool result;
//
//        switch (index) { // eVehicleIndex
//        case VEHICLE_AMBULAN:
//        case VEHICLE_POLICE:
//        case VEHICLE_ENFORCER:
//        case VEHICLE_PREDATOR:
//        case VEHICLE_156:
//            result = TRUE;
//            break;
//        default:
//            result = FALSE;
//            break;
//        }
//        return result;
//    }
//
//    static bool __cdecl IsCarSprayable(CAutomobile *car) {
//        bool result;
//
//        switch (car->m_nModelIndex) {
//        case MODEL_FIRETRUK:
//        case MODEL_AMBULAN:
//        case MODEL_POLICE:
//        case MODEL_ENFORCER:
//        case MODEL_BUS:
//        case MODEL_RHINO:
//        case MODEL_BARRACKS:
//        case MODEL_DODO:
//        case MODEL_COACH:
//        case m_MODEL_POLICE:
//            result = FALSE;
//            break;
//        default:
//            result = TRUE;
//            break;
//        }
//        return result;
//    }
//
//    static void __fastcall OpcodePlayerDrivingTaxiVehicle(CRunningScript *script) {
//        script->CollectParameters(&script->m_nIp, 1);
//        bool isTaxiModel = false;
//
//        CPlayerPed * player = CWorld::Players[CTheScripts::ScriptParams[0].uParam].m_pPed;
//        if (player->m_bInVehicle) {
//            unsigned int model = player->m_pVehicle->m_nModelIndex;
//            if (model == MODEL_TAXI || model == MODEL_CABBIE || model == MODEL_BORGNINE || GetTaxiModels().find(model) != GetTaxiModels().end())
//                isTaxiModel = true;
//        }
//        script->UpdateCompareFlag(isTaxiModel);
//    }
//    
//    static void __fastcall OpcodeIsPlayerInModel(CRunningScript *script) {
//        script->CollectParameters(&script->m_nIp, 2);
//        bool inModel = false;
//
//        CPlayerPed * player = CWorld::Players[CTheScripts::ScriptParams[0].uParam].m_pPed;
//        if (player->m_bInVehicle) {
//            unsigned int model = player->m_pVehicle->m_nModelIndex;
//            if (model == CTheScripts::ScriptParams[1].uParam)
//                inModel = true;
//            else if (model == m_MODEL_POLICE && CTheScripts::ScriptParams[1].uParam == MODEL_POLICE) // Vigilante
//                inModel = true;
//            //else if (model == m_MODEL_AMBULAN && CTheScripts::ScriptParams[1].uParam == MODEL_AMBULAN) // Paramedic
//            //inModel = true;
//        }
//        script->UpdateCompareFlag(inModel);
//    }
//
//    //
//    static void __fastcall SetModelIndex(CEntity *_this, int, unsigned int modelIndex) {
//        if (modelIndex == MODEL_POLICE) {
//            int _random = plugin::Random(0, 3);
//            if (_random < 2) {
//                if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_LOADED) 
//                    _this->m_nModelIndex = m_MODEL_POLICE;
//                else
//                    _this->m_nModelIndex = modelIndex;
//            }
//            else
//                _this->m_nModelIndex = modelIndex;
//        }
//        else
//            _this->m_nModelIndex = modelIndex;
//        _this->CreateRwObject();
//    }
//
//
//    Test() {
//        std::ifstream stream(PLUGIN_PATH("taxi.dat"));
//        if (!stream.is_open())
//            return;
//        for (std::string line; getline(stream, line); ) {
//            if (line.length() > 0 && line[0] != ';' && line[0] != '#')
//                GetTaxiModels().insert(std::stoi(line));
//        }
//
//        patch::RedirectJump(0x473E70, SetModelIndex);//
//
//        patch::RedirectJump(0x552880, IsLawEnforcementVehicle);
//        patch::RedirectJump(0x415C60, AddPoliceCarOccupants);
//        patch::RedirectJump(0x4181F0, ChoosePoliceCarModel);
//        patch::RedirectJump(0x57E6A0, UsesPoliceRadio);
//        patch::RedirectJump(0x552200, UsesSiren);
//        patch::RedirectJump(0x57E4B0, PlayerInCar);
//        patch::RedirectJump(0x56C3C0, UsesSirenAudio);
//        patch::RedirectJump(0x56C3F0, UsesSirenSwitching);
//        patch::RedirectJump(0x426700, IsCarSprayable);
//
//        patch::RedirectCall(0x446A93, OpcodePlayerDrivingTaxiVehicle);
//        patch::Nop(0x446A98, 0x4C); // или сделать jump на 0x446AE4
//
//        patch::RedirectCall(0x43DA19, OpcodeIsPlayerInModel);
//        patch::Nop(0x43DA1E, 0x40); // или сделать jump на 0x43DA5E
//
//        patch::RedirectJump(0x5373D7, Patch_5373D7);
//        patch::RedirectJump(0x4F5857, Patch_4F5857);
//
//        Events::gameProcessEvent += [] {
//            if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_NOT_LOADED) {
//                unsigned char oldFlags = CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nFlags;
//                CStreaming::RequestModel(m_MODEL_POLICE, GAME_REQUIRED);
//                CStreaming::LoadAllRequestedModels(false);
//                if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_LOADED) {
//                    if (!(oldFlags & GAME_REQUIRED)) {
//                        CStreaming::SetModelIsDeletable(m_MODEL_POLICE);
//                        CStreaming::SetModelTxdIsDeletable(m_MODEL_POLICE);
//                    }
//                    CVehicle *vehicle = nullptr;
//                    vehicle = new CAutomobile(m_MODEL_POLICE, 1);
//                }
//            }
//            static char message[256];
//            snprintf(message, 256, "LoadState ID 156 = %d", CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState);
//            CMessages::AddMessageJumpQ(message, 1000, false);
//        };
//    }
//}test;
//
//unsigned int Test::CurrentSpecialModel;
//unsigned int Test::CurrentPoliceModel;
//
//void __declspec(naked) Test::Patch_5373D7() {
//    __asm {
//        movsx eax, word ptr[ebp + 0x5C]
//        pushad
//        push eax
//        call GetSpecialModel
//        mov CurrentSpecialModel, eax
//        popad
//        mov eax, CurrentSpecialModel
//        lea edx, [eax - 0x61]
//        mov edi, 0x5373DE
//        jmp edi
//    }
//}
//
//void __declspec(naked) Test::Patch_4F5857() {
//    __asm {
//        movsx   eax, word ptr[ebx + 5Ch]
//        pushad
//        push eax
//        call GetPoliceModel
//        mov CurrentPoliceModel, eax
//        popad
//        mov eax, CurrentPoliceModel
//        pop     ecx
//        pop     ecx
//        sub     eax, 61h
//        mov edx, 0x4F5860
//        jmp edx
//    }
//}
//
// end

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//#include "TxdDef.h"
//#include "CFileLoader.h"
//#include "CTxdStore.h"
//#include "CModelInfo.h"
//#include "CGameLogic.h"
//#include "CDirectory.h"
//#include "CStreaming.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static TxdDef *m_txd;
//    static RwTexDictionary *m_dictionary;
//
//    Test() {
//        Events::drawingEvent += [] {
//            if (CGameLogic::ActivePlayers) {
//                if (CTxdStore::ms_pTxdPool->m_byteMap[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex].bEmpty)
//                    m_txd = nullptr;
//                else 
//                    m_txd = &CTxdStore::ms_pTxdPool->m_pObjects[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex];
//                    
//                gamefont::Print({
//                    Format("116 ID %d", CTxdStore::ms_pTxdPool->m_byteMap[CModelInfo::ms_modelInfoPtrs[116]->m_nType]),
//                    Format("m_MODEL_POLICE ID %d", CTxdStore::ms_pTxdPool->m_pObjects[CModelInfo::ms_modelInfoPtrs[m_MODEL_POLICE]->m_nObjectDataIndex])
//                }, 10, 250, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);
//
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay('B', 1000)) {
//                    if (CTxdStore::ms_pTxdPool->m_byteMap[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex].bEmpty)
//                        m_txd = nullptr;
//                    else {
//                        m_txd = &CTxdStore::ms_pTxdPool->m_pObjects[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex];
//                        
//                        //CModelInfo::ms_modelInfoPtrs[116]->ClearTexDictionary();
//                        //CModelInfo::ms_modelInfoPtrs[116]->SetTexDictionary("police4");
//                        //CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                        
//                        //CMessages::AddMessageJumpQ(L"OK", 2000, 0);
//
//                        //m_dictionary = CFileLoader::LoadTexDictionary(GAME_PATH("txd\\mpolice.txd"));
//                        //if (m_dictionary) {
//                        //    CModelInfo::ms_modelInfoPtrs[116]->ClearTexDictionary();
//                        //    CModelInfo::ms_modelInfoPtrs[116]->SetTexDictionary("police4");
//                        //    CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                        //    //RwTexDictionarySetCurrent(m_txd->m_pRwDictionary);
//                        //    //CFileLoader::AddTexDictionaries(m_txd->m_pRwDictionary, m_dictionary);
//                        //    //RwTexDictionaryDestroy(m_dictionary);
//                        //    //m_txd->m_pRwDictionary = m_dictionary;
//                        //    CMessages::AddMessageJumpQ(L"OK", 2000, 0);
//                        //}
//                    }
//                }
//            }
//        };
//    }
//} test;
//
//TxdDef *Test::m_txd = nullptr;
//RwTexDictionary *Test::m_dictionary = nullptr;

//#include <plugin.h>
//#include "common.h"
//#include "CModelInfo.h"
//#include "CTxdStore.h"
//#include "eVehicleModel.h"
//#include "CSprite2d.h"
//#include "extensions\KeyCheck.h"
//#include "CFileLoader.h"
//
//char *gString = (char *)0x711B40;
//
//using namespace plugin;
//
//class VehicleTexture {
//public:
//    //static RwTexture *pMyTexture;
//    static RwTexture *myTexture;
//    static RwTexture *texture;
//
//    static RwTexDictionary *m_txd;
//    static RwTexture *m_textureLogo;
//    static RwTexture *m_textureTest;
//
//    static float ScreenCoord(float a) {
//        return static_cast<int>(a * (static_cast<float>(RsGlobal.maximumHeight) / 900.0f));
//    }
//
//    //static void Draw() {
//
//    //    static TxdDef *myTxd;
//    //    
//    //    KeyCheck::Update();
//    //    //if (KeyCheck::CheckWithDelay('J', 1000)) {
//    //    //    if (CTxdStore::ms_pTxdPool->m_byteMap[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex].bEmpty)
//    //    //        myTxd = nullptr;
//    //    //    else {
//    //    //        myTxd = &CTxdStore::ms_pTxdPool->m_pObjects[CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex];
//    //    //        RwTexture *texture = RwTexDictionaryFindNamedTexture(myTxd->m_pRwDictionary, "remap");
//    //    //        if (texture) {
//    //    //            if (texture->dict) {
//    //    //                texture->dict = 0;
//    //    //                texture->lInDictionary.prev = texture->lInDictionary.next;
//    //    //                texture->lInDictionary.next = texture->lInDictionary.prev;
//    //    //            }
//    //    //            //texture = RwTextureRead("test", 0);
//    //    //            //RwTextureDestroy(texture);
//    //    //            //RwTexDictionaryAddTexture(myTxd->m_pRwDictionary, myTexture);
//    //    //        }
//    //    //    }
//    //    //}
//
//    //    if (KeyCheck::CheckWithDelay('N', 1000)) {
//    //        //CVehicle *vehicle = FindPlayerVehicle();
//    //        //if (vehicle) {
//    //            CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[91]);
//    //            if (myTexture) {
//    //                RpMaterial **materialPrim;
//    //                materialPrim = &vehModel->m_apMaterialsPrimary[13];
//    //                RpMaterial *material;
//    //                //int i = 13;
//    //                while (true) {
//    //                    material = *materialPrim;
//    //                    if (!*materialPrim)
//    //                        break;
//    //                    if (myTexture)
//    //                        RpMaterialSetTexture(material, myTexture);
//    //                    ++materialPrim; //++i;
//    //                }
//    //            }
//    //        //}
//    //    }
//    //    
//    //    if (KeyCheck::CheckWithDelay('B', 1000)) {
//    //        //CVehicle *vehicle = FindPlayerVehicle();
//    //        //if (vehicle) {
//    //            CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[91]);
//    //            RpMaterial **materialPrim;
//    //            materialPrim = &vehModel->m_apMaterialsPrimary[0];
//    //            RpMaterial *material;
//    //            TxdDef *txd = CTxdStore::ms_pTxdPool->GetAt(CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex);
//    //            if (txd) {
//    //                RwTexture *remaptexture = RwTexDictionaryFindNamedTexture(txd->m_pRwDictionary, "remap");
//    //                if (remaptexture) {
//    //                    int i = 0;
//    //                    while (i < 13) {
//    //                        material = *materialPrim;
//    //                        if (!*materialPrim)
//    //                            break;
//    //                        RpMaterialSetTexture(material, remaptexture);
//    //                        ++materialPrim; ++i;
//    //                    }
//    //                }
//    //            }
//    //        //}
//    //    }
//
//
//    //    CPed *player = FindPlayerPed();
//    //    //CVehicle *veh = FindPlayerVehicle();
//    //    //if (veh && veh->m_nModelIndex == MODEL_IDAHO) {
//    //    if (player) {
//    //        //if (!myTexture) {
//    //        //    //sprintf(gString, "image\\%s.bmp", "mytest");
//    //        //    //RwImage *image = RtBMPImageRead(gString);
//    //        //    RwImage *image = RtBMPImageRead("image\\mytest.bmp");
//    //        //    if (image) {
//    //        //        RwInt32 width, height, depth, flags;
//    //        //        RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
//    //        //        RwRaster *raster = RwRasterCreate(width, height, depth, flags);
//    //        //        RwRasterSetFromImage(raster, image);
//    //        //        myTexture = RwTextureCreate(raster);
//    //        //        RwTextureSetName(myTexture, "mytest");
//    //        //        RwImageDestroy(image);
//    //        //    }
//    //        //}
//
//    //        //TxdDef *txd = CTxdStore::ms_pTxdPool->GetAt(CModelInfo::ms_modelInfoPtrs[veh->m_nModelIndex]->m_nTxdIndex);
//    //        if (!texture) {
//    //            TxdDef *txd = CTxdStore::ms_pTxdPool->GetAt(CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex);
//    //            if (txd) {
//    //                texture = RwTexDictionaryFindNamedTexture(txd->m_pRwDictionary, "test");
//    //            }
//    //        }
//    //        
//    //        if (texture) {
//    //            RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, texture->raster);
//    //            //CSprite2d::DrawRect(CRect(RsGlobal.maximumWidth / 2 - 128, 200.0f, RsGlobal.maximumWidth / 2 + 128, 264.0f), CRGBA(255, 255, 255, 255));
//    //            //CSprite2d::SetVertices(CRect(RsGlobal.maximumWidth - (RsGlobal.maximumWidth - 130), RsGlobal.maximumHeight - 122, RsGlobal.maximumWidth - (RsGlobal.maximumWidth - 210), RsGlobal.maximumHeight - 42), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//    //            CSprite2d::SetVertices(CRect(ScreenCoord(120.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(200.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//    //            RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//    //        }
//    //        
//    //        /*if (myTexture) {
//    //            RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, myTexture->raster);
//    //            CSprite2d::SetVertices(CRect(ScreenCoord(220.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(300.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//    //            RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//    //        }*/
//
//    //    }
//    //}
//
//    /*static void DrawLogo() {
//        CPed *player = FindPlayerPed();
//        if (player) {
//            if (m_textureTest) {
//                RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, m_textureTest->raster);
//                CSprite2d::SetVertices(CRect(ScreenCoord(120.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(200.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//                RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//            }
//            if (m_textureLogo) {
//                RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, m_textureLogo->raster);
//                CSprite2d::SetVertices(CRect(ScreenCoord(220.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(640.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//                RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//            }
//        }
//    }*/
//
//    VehicleTexture() {
//        Events::initRwEvent += [] {
//            m_txd = CFileLoader::LoadTexDictionary(GAME_PATH("txd\\mpolice.txd"));
//            m_textureLogo = GetFirstTexture(m_txd);
//            m_textureTest = RwTexDictionaryFindNamedTexture(m_txd, "test");
//        };
//
//        //Events::drawingEvent += Draw;
//        //Events::menuDrawingEvent += Draw;
//        //Events::menuDrawingEvent += DrawLogo;
//        //Events::drawHudEvent += DrawLogo;
//
//        Events::drawingEvent += [] {
//            if (m_textureTest) {
//                RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, m_textureTest->raster);
//                CSprite2d::SetVertices(CRect(ScreenCoord(120.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(200.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//                RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//            }
//            if (m_textureLogo) {
//                RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, m_textureLogo->raster);
//                CSprite2d::SetVertices(CRect(ScreenCoord(220.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(640.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), 0);
//                RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//            }
//        };
//
//        Events::shutdownRwEvent += [] {
//            RwTexDictionaryDestroy(m_txd);
//        };
//    }
//} vehTexture;
//
////RwTexture *VehicleTexture::pMyTexture;
//RwTexture *VehicleTexture::myTexture = nullptr;
//RwTexture *VehicleTexture::texture = nullptr;
//
//RwTexDictionary *VehicleTexture::m_txd;
//RwTexture *VehicleTexture::m_textureLogo;
//RwTexture *VehicleTexture::m_textureTest;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "common.h"
//#include "CWorld.h"
//#include "CObject.h"
//#include "CFont.h"
//#include "CTimer.h"
//
////RwFrame* nRwFrameForAllObjects(RwFrame* frame, RwObjectCallBack callBack, int data) {
////    return ((RwFrame*(__cdecl *)(RwFrame*, RwObjectCallBack, int))0x5A2340)(frame, callBack, data);
////}
////RwObject* GetCurrentAtomicObjectCB(RwObject *rwObject, void *data) {
////    return ((RwObject*(__cdecl *)(RwObject*, void*))0x52C690)(rwObject, data);
////}
//
//using namespace plugin;
//
//class Test {
//public:
//    /*static CVehicle* GetVehicle(CPed *player) {
//        CVehicle* result = nullptr;
//        CEntity *outEntity[10] = { nullptr };
//        short outCount = 0;
//        int index = 0;
//        float radius = 20.0f;
//        float distance = 0.0f;
//        CWorld::FindObjectsInRange(player->GetPosition(), 20.0, 1, &outCount, 10, outEntity, 0, 1, 0, 0, 0);
//        if (outCount > 0) {
//            do
//            {
//                CVehicle *vehicle = (CVehicle *)outEntity[index];
//                if (vehicle && player->m_pVehicle) {
//                    distance = DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition());
//                    if (distance < radius) {
//                        radius = distance; result = vehicle;
//                    }
//                }
//                ++index;
//            } while (index < outCount);
//        }
//        return result;
//    }*/
//
//    Test() {
//        static float &UserPause = *((float *)0x5F76B8 + 2);
//        static bool &Mode = *(bool *)0x95CD5B;
//
//        //Events::gameProcessEvent += [] {
//        Events::drawingEvent += [] {
//            CFont::SetScale(0.5f, 1.0f);
//            CFont::SetColor(CRGBA(255, 255, 255, 255));
//            CFont::SetJustifyOn();
//            CFont::SetFontStyle(0);
//            CFont::SetPropOn();
//            CFont::SetWrapx(600.0f);
//            wchar_t text[64];
//            swprintf(text, L"Mode %d", Mode);
//            CFont::PrintString(10.0f, 10.0f, text);
//
//            CPed *player = FindPlayerPed();
//            if (player) {
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay('H', 1000)) {
//                    //UserPause = CTimer::m_UserPause;
//                    //CTimer::m_CodePause = TRUE;
//                    //UserPause = 0.0f;
//                    Mode = TRUE;
//                }
//                    
//                if (KeyCheck::CheckWithDelay('J', 1000))
//                    Mode = FALSE;
//                    //UserPause = 0.001f;
//                    //CTimer::m_CodePause = FALSE;
//            }
//            
//            //CVehicle *vehicle = FindPlayerVehicle();
//            //if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
//            //    CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
//            //    KeyCheck::Update();
//            //    if (KeyCheck::CheckWithDelay('H', 1000)) {
//            //        //CPed *player = FindPlayerPed();
//            //        //if (player) {
//            //            //CVehicle* car = GetVehicle(player);
//            //            //if (car) {
//            //                RpAtomic *rwObject = nullptr;
//            //                RwFrameForAllObjects(automobile->m_aCarNodes[CAR_BOOT], GetCurrentAtomicObjectCB, &rwObject);
//            //                if (rwObject) {
//            //                    CObject *object = nullptr;
//            //                    object = new CObject();
//            //                    if (object) {
//            //                        object->SetModelIndexNoCreate(193);
//            //                        object->RefModelInfo(automobile->m_nModelIndex);
//            //                        RwMatrix *matrix = RwFrameGetLTM(automobile->m_aCarNodes[CAR_BOOT]);
//            //                        RwFrame *frame = RwFrameCreate();
//            //                        rwObject = RpAtomicClone(rwObject);
//            //                        frame->modelling = *matrix;
//            //                        RpAtomicSetFrame(rwObject, frame);
//
//            //                        object->AttachToRwObject(&rwObject->object.object);
//            //                        object->m_fMass = 0x41200000;
//            //                        object->m_fTurnMass = 0x41C80000;
//            //                        object->m_fAirResistance = 0x3F7851EC;
//            //                        object->m_fElasticity = 0x3DCCCCCD;
//            //                        object->m_vecMoveSpeed.x = automobile->m_vecMoveSpeed.x;
//            //                        object->m_vecMoveSpeed.y = automobile->m_vecMoveSpeed.y;
//            //                        object->m_vecMoveSpeed.z = automobile->m_vecMoveSpeed.z;
//
//            //                        object->m_matrix.pos.x = automobile->m_matrix.pos.x + 2.0f;
//            //                        object->m_matrix.pos.y = automobile->m_matrix.pos.y + 2.0f;
//            //                        object->m_matrix.pos.z = automobile->m_matrix.pos.z;
//
//            //                        CWorld::Add(object);
//            //                    }
//            //                    //car->AttachToRwObject(&rwObject->object.object);
//            //                }
//            //            //}
//            //        //}
//            //    }
//            //}
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CModelInfo.h"
//#include "RenderWare.h"
//#include "common.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static void __stdcall AddWeapon(int modelIndex)
//    {
//        CPed *ped = FindPlayerPed();
//        RpAtomic *atomic; 
//        CBaseModelInfo *baseInfo;
//
//        if (modelIndex != -1)
//        {
//            baseInfo = CModelInfo::ms_modelInfoPtrs[modelIndex];
//            atomic = (RpAtomic *)baseInfo->CreateInstance();
//            RwFrameDestroy((RwFrame *)atomic->object.object.parent);
//            RpAtomicSetFrame(atomic, ped->m_apFrames[HEAD]->m_pIFrame);
//            RpClumpAddAtomic(ped->m_pRwClump, atomic);
//            ped->m_nWepModelID = modelIndex;
//        }
//    }
//    
//    Test() {
//        patch::RedirectJump(0x4CF8F0, AddWeapon);
//    }
//} test;

//#include <plugin.h>
//#include "CSprite.h"
//#include "CFont.h"
//#include "CModelInfo.h"
//#include "CZoneInfo.h"
//#include "extensions\KeyCheck.h"
//#include "CStreaming.h"
//#include "eVehicleModel.h"
//#include "ePedModel.h"
//#include "CMessages.h"
//#include "CWorld.h"
//#include "CCarAI.h"
//#include "CPathFind.h"
//
//#define m_MODEL_POLICE 156
//
//void __cdecl GetZoneInfoForTimeOfDay(CVector *point, CZoneInfo *info) {
//    ((void(__cdecl *)(CVector*, CZoneInfo*))0x4B6FB0)(point, info);
//};
//
//int __cdecl ChooseModel(CZoneInfo *info, CVector *point, int *vehicleClass) {
//    return ((int(__cdecl *)(CZoneInfo*, CVector*, int *))0x417EC0)(info, point, vehicleClass);
//};
//
//bool __cdecl /*CCarCtrl::*/GenerateOneEmergencyServicesCar(int vehicleModelIndex, CVector point) {
//    return ((bool(__cdecl *)(int, CVector))0x41FE50)(vehicleModelIndex, point);
//};
//
//
//using namespace plugin;
//
//class DistanceExample {
//public:
//    class DistanceInfo {
//    public:
//        // данные нашего класса
//        float distance; // пройденна€ дистанци€
//        bool startedPositionRecording; // если уже начали отслеживать позицию
//        CVector lastPosition; // последн€€ позици€
//
//        DistanceInfo(CVehicle *vehicle) {
//            //  онтсруктор нашего класса. ƒолжен быть об€зательно обь€влен. ѕринимает один параметр -
//            // транспорт, к которому "прицепл€етс€" этот класс.
//            // Ётот конструктор будет вызван на этапе конструировани€ CVehicle (CVehicle::CVehicle).
//            // ѕри этом, обращатьс€ к каким-либо членам CVehicle запрещено (обьект CVehicle ещЄ не построен до конца).
//            distance = 0.0f; // устанавливаем начальное значение
//            startedPositionRecording = false;
//        }
//    };
//
//    DistanceExample() {
//        static VehicleExtendedData<DistanceInfo> VehDistance; // Ќаше расширение
//
//        Events::vehicleRenderEvent += [](CVehicle *vehicle) {
//            DistanceInfo &info = VehDistance.Get(vehicle); // ѕолучаем наши данные дл€ этого транспорта
//            if (info.startedPositionRecording) // если info.lastPosition не "пустой"
//                info.distance += DistanceBetweenPoints(info.lastPosition, vehicle->GetPosition()); // добавл€ем рассто€ние
//            else
//                info.startedPositionRecording = true;
//            info.lastPosition = vehicle->GetPosition();
//        };
//
//        Events::drawingEvent += [] {
//            CPlayerPed *player = FindPlayerPed();
//            if (player) {
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay('M', 1000)) {
//                    //unsigned char oldFlags = CStreaming::ms_aInfoForModel[MODEL_AMBULAN].m_nFlags;
//                    CStreaming::RequestModel(MODEL_AMBULAN, MISSION_REQUIRED);
//                    CStreaming::RequestModel(MODEL_MEDIC, 1);
//                    if (CStreaming::ms_aInfoForModel[MODEL_AMBULAN].m_nLoadState == LOADSTATE_LOADED
//                        && CStreaming::ms_aInfoForModel[MODEL_MEDIC].m_nLoadState == LOADSTATE_LOADED)
//                    {
//                        /*if (!(oldFlags & GAME_REQUIRED)) {
//                        CStreaming::SetModelIsDeletable(MODEL_AMBULAN);
//                        CStreaming::SetModelTxdIsDeletable(MODEL_AMBULAN);
//                        }*/
//
//                        if (/*CCarCtrl::*/GenerateOneEmergencyServicesCar(MODEL_AMBULAN, FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 50.0f, 0.0f))))
//                            //CCarCtrl::LastTimeAmbulanceCreated = CTimer::m_snTimeInMilliseconds;
//                            CMessages::AddMessageJumpQ(L"AmbulanceCreate", 2000, 0);
//                    }
//                }
//
//                if (KeyCheck::CheckWithDelay('N', 1000)) {
//                    unsigned char oldFlags = CStreaming::ms_aInfoForModel[MODEL_AMBULAN].m_nFlags;
//                    CStreaming::RequestModel(m_MODEL_POLICE, MISSION_REQUIRED);
//                    CStreaming::RequestModel(MODEL_MEDIC, 1);
//                    if (CStreaming::ms_aInfoForModel[m_MODEL_POLICE].m_nLoadState == LOADSTATE_LOADED
//                        && CStreaming::ms_aInfoForModel[MODEL_MEDIC].m_nLoadState == LOADSTATE_LOADED)
//                    {
//                        if (!(oldFlags & GAME_REQUIRED)) {
//                            CStreaming::SetModelIsDeletable(m_MODEL_POLICE);
//                            CStreaming::SetModelTxdIsDeletable(m_MODEL_POLICE);
//                        }
//
//                        if (/*CCarCtrl::*/GenerateOneEmergencyServicesCar(m_MODEL_POLICE, FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 50.0f, 0.0f))))
//                            CMessages::AddMessageJumpQ(L"Create", 2000, 0);
//                    }
//                }
//
//            }
//
//
//            //for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//            //    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//            //    if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE && vehicle->GetIsOnScreen()) {
//            //        CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
//            //        CVector &posn = vehicle->GetPosition();
//            //        CZoneInfo info; int vehicleClass = 17; int model;
//            //        GetZoneInfoForTimeOfDay(&posn, &info);
//            //        model = ChooseModel(&info, &posn, &vehicleClass);
//            //        
//            //        RwV3d rwp = { posn.x, posn.y, posn.z + 1.0f };
//            //        RwV3d screenCoors; float w, h;
//            //        if (CSprite::CalcScreenCoors(rwp, &screenCoors, &w, &h, true) && w > 10.0f) {
//            //            CFont::SetScale(w * 0.015f, h * 0.03f);
//            //            CFont::SetColor(CRGBA(255, 255, 255, 255));
//            //            CFont::SetJustifyOn();
//            //            CFont::SetFontStyle(0);
//            //            CFont::SetPropOn();
//            //            //CFont::SetWrapx(600.0f);
//            //            wchar_t text[64];
//            //            //swprintf(text, L"%.2f", VehDistance.Get(vehicle).distance);
//            //            if (vehicleClass == 17)
//            //                swprintf(text, L"%d, %d", vehicleClass, model);
//            //            //swprintf(text, L"%d", info.carCops);
//            //            CFont::PrintString(screenCoors.x, screenCoors.y, text);
//            //        }
//            //    }
//            //}
//        };
//    }
//} example;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CModelInfo.h"
//#include "CFont.h"
//#include "CStreaming.h"
//#include "CTxdStore.h"
//
//#include "CCamera.h"
//
//#include "common.h"
//#include <map>
//
//
//RpMaterial* SetTextureCB(RpMaterial *material, void *data) {
//    return ((RpMaterial*(__cdecl *)(RpMaterial*, void*))0x528B10)(material, data);
//}
//
//using namespace plugin;
//
//class VehicleTextures {
//public:
//    VehicleTextures() {
//        static CdeclEvent<AddressList<0x4A7A7B, H_CALL, 0x4A7ABE, H_CALL, 0x4A7AFE, H_CALL, 0x4A7B71, H_CALL, 0x5290E4, H_CALL>,
//            PRIORITY_BEFORE, ArgPickN<CEntity*, 0>, void(CEntity*)> myOnRenderOneNonRoad;
//
//        static std::map<RpMaterial*, RwTexture *> originalTextures;
//
//        myOnRenderOneNonRoad.before += [](CEntity *entity) {
//            if (KeyPressed('M') && entity == FindPlayerVehicle()) {
//                CVehicle *vehicle = reinterpret_cast<CVehicle *>(entity);
//                RpClumpForAllAtomics(vehicle->m_pRwClump, [](RpAtomic *atomic, void *data) {
//                    RpGeometryForAllMaterials(atomic->geometry, [](RpMaterial *material, void *data) {
//                        //if (originalTextures.find(material) == originalTextures.end())
//                        if (originalTextures.find(material) == originalTextures.end()) 
//                            originalTextures[material] = material->texture;
//                                                    
//                        //if (!strcmp(material->texture->name, "remap"))
//                        //if (!strcmp(originalTextures[material]->name, "remap"))
//                            //RpMaterialSetTexture(material, *reinterpret_cast<RwTexture **>(0x648F2C));
//                            material->texture = *reinterpret_cast<RwTexture **>(0x648F2C);
//                        return material;
//                    }, nullptr);
//                    return atomic;
//                }, nullptr);
//            }
//        };
//
//        myOnRenderOneNonRoad.after += [](CEntity *) {
//            if (originalTextures.size() > 0) {
//                for (auto &i : originalTextures)
//                    i.first->texture = i.second;
//                originalTextures.clear();
//            }
//        };
//    }
//} vehicleTextures;

//class MyPlugin {
//public:
//    MyPlugin() {
//
//        Events::drawingEvent += [] {
//            CFont::SetScale(0.5f, 1.0f);
//            CFont::SetColor(CRGBA(255, 255, 255, 255));
//            CFont::SetJustifyOn();
//            CFont::SetFontStyle(0);
//            CFont::SetPropOn();
//            CFont::SetWrapx(600.0f);
//            wchar_t text[64];
//            swprintf(text, L"TxdIndex %d : %d : %d", CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex, CModelInfo::ms_modelInfoPtrs[4001]->m_nTxdIndex, CModelInfo::ms_modelInfoPtrs[90]->m_nTxdIndex);
//            CFont::PrintString(10.0f, 80.0f, text);
//
//            unsigned char oldFlags = CStreaming::ms_aInfoForModel[116].m_nFlags;
//            swprintf(text, L"Flag %d : %d : %d", oldFlags, CStreaming::ms_aInfoForModel[4001].m_nFlags, CStreaming::ms_aInfoForModel[90].m_nFlags);
//            CFont::PrintString(10.0f, 110.0f, text);
//
//            swprintf(text, L"LoadState %d : %d : %d", CStreaming::ms_aInfoForModel[116].m_nLoadState, CStreaming::ms_aInfoForModel[4001].m_nLoadState, CStreaming::ms_aInfoForModel[90].m_nLoadState);
//            CFont::PrintString(10.0f, 140.0f, text);
//
//            swprintf(text, L"NumRefs %d : %d : %d", CTxdStore::GetNumRefs(35), CTxdStore::GetNumRefs(78), CTxdStore::GetNumRefs(736));
//            CFont::PrintString(10.0f, 170.0f, text);
//
//            swprintf(text, L"RefCount %d : %d : %d", CModelInfo::ms_modelInfoPtrs[116]->m_nRefCount, CModelInfo::ms_modelInfoPtrs[4001]->m_nRefCount, CModelInfo::ms_modelInfoPtrs[90]->m_nRefCount);
//            CFont::PrintString(10.0f, 210.0f, text);
//
//            /*swprintf(text, L"ValueScript %.2f", TheCamera.m_fCarZoomValueScript);
//            CFont::PrintString(10.0f, 250.0f, text);
//            swprintf(text, L"ZoomIndicator %.2f", TheCamera.m_fCarZoomIndicator);
//            CFont::PrintString(10.0f, 280.0f, text);
//            swprintf(text, L"ZoomValue %.2f", TheCamera.m_fCarZoomValue);
//            CFont::PrintString(10.0f, 310.0f, text);
//            swprintf(text, L"ValueSmooth %.2f", TheCamera.m_fCarZoomValueSmooth);
//            CFont::PrintString(10.0f, 340.0f, text);*/
//
//            //swprintf(text, L"numModelsReq %d", CStreaming::ms_numModelsRequested);
//            /*swprintf(text, L"bUse %d", TheCamera.m_bUseScriptZoomValueCar);
//            CFont::PrintString(10.0f, 380.0f, text);*/
//        };
//
//        Events::gameProcessEvent += [] {
//            //int index;
//
//            /*CPed *player = FindPlayerPed();
//            if (player)
//                player->m_nFlags.bBulletProof = 1;*/
//
//            KeyCheck::Update();
//            //if (KeyCheck::CheckWithDelay('Y', 500)) {
//            //    //TheCamera.m_asCams[TheCamera.m_nActiveCam].
//            //    TheCamera.m_fCarZoomValueScript = 5.0f;
//            //    TheCamera.m_fCarZoomIndicator = 1.0f;
//            //    TheCamera.m_fCarZoomValue = 1.0f;
//            //    //TheCamera.m_fCarZoomValueSmooth = 7.0f;
//            //    TheCamera.m_bUseScriptZoomValueCar = 1;
//            //}
//            //if (KeyCheck::CheckWithDelay('U', 500)) {
//            //    TheCamera.m_fCarZoomValueScript = 0.0f;
//            //    TheCamera.m_bUseScriptZoomValueCar = 0;
//            //    TheCamera.m_fCarZoomIndicator = 1.0f;
//            //    TheCamera.m_fCarZoomValue = 0.5f;
//            //    TheCamera.m_fCarZoomValueSmooth = 0.5f;
//            //    //TheCamera.m_bUseScriptZoomValueCar = 1;
//            //}
//            //if (KeyCheck::CheckWithDelay('I', 500)) {
//            //    TheCamera.m_bInATunnelAndABigVehicle = 1;
//            //}
//            //if (KeyCheck::CheckWithDelay('P', 500)) {
//            //    TheCamera.m_bInATunnelAndABigVehicle = 0;
//            //}
//            
//            //if (KeyCheck::CheckWithDelay('N', 200)) {
//            //    CVehicle* vehicle = FindPlayerVehicle();
//            //    if (vehicle) {
//            //        CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
//            //        RpMaterial **materialPrim;
//            //        materialPrim = vehModel->m_apMaterialsPrimary;
//            //        RpMaterial *material;
//            //        material = *materialPrim;
//            //        RpMaterialSetTexture(material, *reinterpret_cast<RwTexture **>(0x648F2C));
//            //        //vehModel->m_nTxdIndex = CTxdStore::FindTxdSlot("police2");
//            //        //vehModel->SetTexDictionary("police2");
//            //        //CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//            //        //vehicle->SetModelIndex(vehicle->m_nModelIndex);
//            //    }
//            //}
//            
//            //unsigned char oldLoadState;
//            if (KeyCheck::CheckWithDelay(99, 1000)) {
//                /*index = CTxdStore::FindTxdSlot("police4");
//                if (index != -1) {
//                    CTxdStore::AddTxdSlot("police4");
//                    CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex = index;
//                    CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                }*/
//                
//                
//                //oldLoadState = CStreaming::ms_aInfoForModel[116].m_nLoadState;
//
//                //CModelInfo::ms_modelInfoPtrs[116]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[116]->SetTexDictionary("police4");
//                CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                //CStreaming::ms_aInfoForModel[116].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[116].m_nLoadState = oldLoadState;
//            }
//            if (KeyCheck::CheckWithDelay(98, 1000)) {
//                /*index = CTxdStore::FindTxdSlot("police3");
//                if (index != -1) {
//                    CTxdStore::AddTxdSlot("police3");
//                    CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex = index;
//                    CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                    
//                }*/
//
//                //CModelInfo::ms_modelInfoPtrs[116]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[116]->SetTexDictionary("police3");
//                CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                //CStreaming::ms_aInfoForModel[116].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[116].m_nLoadState = oldLoadState;
//            }
//            if (KeyCheck::CheckWithDelay(97, 1000)) {
//                /*index = CTxdStore::FindTxdSlot("police2");
//                if (index != -1) {
//                    CTxdStore::AddTxdSlot("police2");
//                    CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex = index;
//                    CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                    
//                }*/
//
//                //CModelInfo::ms_modelInfoPtrs[116]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[116]->SetTexDictionary("police2");
//                CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                //CStreaming::ms_aInfoForModel[116].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[116].m_nLoadState = oldLoadState;
//            }
//            if (KeyCheck::CheckWithDelay(96, 1000)) {
//                /*index = CTxdStore::FindTxdSlot("police");
//                if (index != -1) {
//                    CTxdStore::AddTxdSlot("police");
//                    CModelInfo::ms_modelInfoPtrs[116]->m_nTxdIndex = index;
//                    CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                }*/
//
//                //CModelInfo::ms_modelInfoPtrs[116]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[116]->SetTexDictionary("police");
//                CStreaming::ms_aInfoForModel[116].m_nLoadState = 0;
//                //CStreaming::ms_aInfoForModel[116].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[116].m_nLoadState = oldLoadState;
//            }
//        };
//
//
//
//        /*Events::shutdownRwEvent += [] {
//            CTxdStore::RemoveTxdSlot(CTxdStore::FindTxdSlot("police2"));
//            CTxdStore::RemoveTxdSlot(CTxdStore::FindTxdSlot("police3"));
//            CTxdStore::RemoveTxdSlot(CTxdStore::FindTxdSlot("police4"));
//            CModelInfo::ms_modelInfoPtrs[116]->RemoveTexDictionaryRef();
//        };*/
//
//    }
//} myPlugin;

//#include "plugin.h"
//#include "CModelInfo.h"
//#include "extensions\KeyCheck.h"
//#include "CWorld.h"
//#include "CVehicleModelInfo.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static CVehicle* GetVehicle(CPed *player) {
//        CVehicle* result = nullptr;
//        CEntity *outEntity[10] = { nullptr };
//        short outCount = 0;
//        int index = 0;
//        float radius = 20.0f;
//        float distance = 0.0f;
//        CWorld::FindObjectsInRange(player->GetPosition(), 20.0, 1, &outCount, 10, outEntity, 0, 1, 0, 0, 0);
//        if (outCount > 0) {
//            do
//            {
//                CVehicle *vehicle = (CVehicle *)outEntity[index];
//                if (vehicle) {
//                    if (player->m_pVehicle && player->m_bInVehicle) {
//                        result = player->m_pVehicle; break;
//                    }
//                    else {
//                        distance = DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition());
//                        if (distance < radius) {
//                            radius = distance; result = vehicle;
//                        }
//                    }
//                }
//                ++index;
//            } while (index < outCount);
//        }
//        return result;
//    }
//
//    static void Color() {
//CPed *player = FindPlayerPed();
//if (player) {
//    KeyCheck::Update();
//    if (KeyCheck::CheckWithDelay('N', 200)) {
//        CVehicle* vehicle = GetVehicle(player);
//        if (vehicle) {
//            CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
//            unsigned char pPrim, pSec;
//            vehModel->ChooseVehicleColour(pPrim, pSec);
//            for (int i = 0; i < 8 && pPrim == vehicle->m_nPrimaryColor && pSec == vehicle->m_nSecondaryColor; ++i) {
//                vehModel->ChooseVehicleColour(pPrim, pSec);
//            }
//            vehicle->m_nPrimaryColor = pPrim;
//            vehicle->m_nSecondaryColor = pSec;
//        }
//    }
//}
//    }
//
//    Test() {
//        Events::gameProcessEvent += Color;
//    }
//
//}test;

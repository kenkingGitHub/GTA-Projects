/*
Plugin-SDK (Grand Theft Auto) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#include "plugin.h"
#include "game_vc\common.h"
#include "game_vc\CAutomobile.h"
#include "game_vc\CTimer.h"

using namespace plugin;

const float ACTION_TIME_STEP = 0.05f;
const int TIME_FOR_KEYPRESS = 500;

class DoorsExample {
public:
    static int componentByDoorId[6]; // ������� �������� eDoors � Id ����������

    static int m_nLastTimeWhenAnyActionWasEnabled; // ��������� ����� ������� �������

    enum eDoorEventType { // ��� �������
        DOOR_EVENT_OPEN,
        DOOR_EVENT_CLOSE
    };

    class DoorEvent { // ����� �������
    public:
        bool m_active;
        eDoorEventType m_type;
        float m_openingState;

        DoorEvent() {
            m_active = false;
            m_type = DOOR_EVENT_CLOSE;
        }
    };

    class VehicleDoors {
    public:
        DoorEvent events[6]; // ������� ��� ���� 6 ������

        VehicleDoors(CVehicle *) {}
    };

    static VehicleExtendedData<VehicleDoors> VehDoors; // ���� ����������

    static void EnableDoorEvent(CAutomobile *automobile, eDoors doorId) { // �������� ������� �����
        if (automobile->IsComponentPresent(componentByDoorId[doorId])) {
            if (automobile->m_carDamage.GetDoorStatus(doorId) != DAMSTATE_NOTPRESENT) {
                DoorEvent &event = VehDoors.Get(automobile).events[doorId];
                if (event.m_type == DOOR_EVENT_OPEN)
                    event.m_type = DOOR_EVENT_CLOSE; // ���� ��������� ������� - ��������, �� ���������
                else
                    event.m_type = DOOR_EVENT_OPEN; // ���� ��������� ������� �������� - �� ���������
                event.m_active = true; // �������� ���������
                m_nLastTimeWhenAnyActionWasEnabled = CTimer::m_snTimeInMilliseconds;
            }
        }
    }

    static void ProcessDoors(CVehicle *vehicle) { // ��������� ������� ��� ����������� ����
        if (vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
            CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
            for (unsigned int i = 0; i < 6; i++) { // ������������ ��� �������
                eDoors doorId = static_cast<eDoors>(i);
                DoorEvent &event = VehDoors.Get(automobile).events[doorId];
                if (event.m_active) { // ���� ������� �������
                    if (event.m_type == DOOR_EVENT_OPEN) {
                        event.m_openingState += ACTION_TIME_STEP;
                        if (event.m_openingState > 1.0f) { // ���� ��������� �������
                            event.m_active = false; // ��������� ���������
                            automobile->OpenDoor(componentByDoorId[doorId], doorId, 1.0f); // ��������� ���������
                            event.m_openingState = 1.0f;
                        }
                        else
                            automobile->OpenDoor(componentByDoorId[doorId], doorId, event.m_openingState);
                    }
                    else {
                        event.m_openingState -= ACTION_TIME_STEP;
                        if (event.m_openingState < 0.0f) { // ���� ��������� �������
                            event.m_active = false; // ��������� ���������
                            automobile->OpenDoor(componentByDoorId[doorId], doorId, 0.0f); // ��������� ���������
                            event.m_openingState = 0.0f;
                        }
                        else
                            automobile->OpenDoor(componentByDoorId[doorId], doorId, event.m_openingState);
                    }
                }
            }
        }
    }

    static void MainProcess() { // ��������� ������� ������ � ������ �������
        if (CTimer::m_snTimeInMilliseconds > (m_nLastTimeWhenAnyActionWasEnabled + TIME_FOR_KEYPRESS)) { // ���� ������ 500 �� � ���� �������, ��� �� ������ ���������/��������� ���-��
            CVehicle *vehicle = FindPlayerVehicle();
            if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle); // ����� ��, ���������� �����. �.�. �� ����� ����� damageManager, ��� ����� ���������, ��� ��������� - ��� ���������� (CAutomobile)
                if (KeyPressed(219)) // [
                    EnableDoorEvent(automobile, BONNET); // �����
                else if (KeyPressed(221)) // ]
                    EnableDoorEvent(automobile, BOOT); // ��������
                else if (KeyPressed(186) && KeyPressed(187)) // ; =
                    EnableDoorEvent(automobile, DOOR_FRONT_LEFT); // ����� �������� �����
                else if (KeyPressed(222) && KeyPressed(187)) // ' =
                    EnableDoorEvent(automobile, DOOR_FRONT_RIGHT); // ������ �������� �����
                else if (KeyPressed(186) && KeyPressed(189)) // ; -
                    EnableDoorEvent(automobile, DOOR_REAR_LEFT); // ����� ������ �����
                else if (KeyPressed(222) && KeyPressed(189)) // ' -
                    EnableDoorEvent(automobile, DOOR_REAR_RIGHT); // ������ ������ �����
                else if (KeyPressed(VK_F12)) {
                    EnableDoorEvent(automobile, BONNET);
                    EnableDoorEvent(automobile, BOOT);
                    EnableDoorEvent(automobile, DOOR_FRONT_LEFT);
                    EnableDoorEvent(automobile, DOOR_FRONT_RIGHT);
                    EnableDoorEvent(automobile, DOOR_REAR_LEFT);
                    EnableDoorEvent(automobile, DOOR_REAR_RIGHT);
                }
            }
        }
    }

    DoorsExample() {
        Events::gameProcessEvent += MainProcess; // ��� ������������ ������� � ��������� �������
        Events::vehicleRenderEvent += ProcessDoors; // ��� ������������ �������, � ����� ��������� ��
    }
} example;

int DoorsExample::componentByDoorId[6] = { CAR_BONNET, CAR_BOOT, CAR_DOOR_LF, CAR_DOOR_RF, CAR_DOOR_LR, CAR_DOOR_RR };
int DoorsExample::m_nLastTimeWhenAnyActionWasEnabled = 0;
VehicleExtendedData<DoorsExample::VehicleDoors> DoorsExample::VehDoors;



//#include "plugin.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//                    if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 7.0f)) {
//                        vehicle->m_autoPilot.m_nAnimationId = TEMPACT_REVERSE;
//                        vehicle->m_autoPilot.m_nAnimationTime = 20000;
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static void Patch_41065D();
//    static void Patch_580FD4();
//    static void Patch_61DCFA();
//    
//    Test() {
//        patch::RedirectJump(0x41065D, Patch_41065D);
//        patch::RedirectJump(0x580FD4, Patch_580FD4);
//        patch::RedirectJump(0x61DCFA, Patch_61DCFA);
//        
//        //patch::SetInt(0x41065D + 2, 2000, true);
//        //patch::SetInt(0x580FD4 + 2, 2000, true);
//        patch::SetInt(0x58102C + 1, 2000, true);
//        patch::SetInt(0x61D98B + 1, 2000, true);
//        patch::SetInt(0x61D9E5 + 1, 2000, true);
//        patch::SetInt(0x61DAA5 + 1, 2000, true);
//        patch::SetInt(0x61DBA5 + 1, 2000, true);
//        //patch::SetInt(0x61DCFA + 2, 2000, true);
//        patch::SetInt(0x61DD39 + 1, 2000, true);
//    }
//} test;
//
//void __declspec(naked) Test::Patch_41065D() {
//    __asm {
//        cmp ebx, 2000
//        mov ebp, 0x410663
//        jmp ebp
//    }
//}
//
//void __declspec(naked) Test::Patch_580FD4() {
//    __asm {
//        cmp ebx, 2000
//        mov eax, 0x580FDA
//        jmp eax
//    }
//}
//
//void __declspec(naked) Test::Patch_61DCFA() {
//    __asm {
//        cmp ebx, 2000
//        mov ebp, 0x61DD00
//        jmp ebp
//    }
//}


//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//#include "CStreaming.h"
//#include "CWorld.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('N', 2000)) {
//                CPed *player = FindPlayerPed();
//                if (player) {
//                    //Command<COMMAND_UNDRESS_CHAR>(CPools::GetPedRef(player), "STRIPA");
//                    int modelIndex = player->m_nModelIndex;
//                    player->DeleteRwObject();
//                    if (player->IsPlayer())
//                        modelIndex = 0;
//                    CStreaming::RequestSpecialModel(modelIndex, "STRIPA", 6);
//                    CWorld::Remove(player);
//                    //Command<COMMAND_LOAD_ALL_MODELS_NOW>();
//                    CTimer::Stop();
//                    CStreaming::LoadAllRequestedModels(false);
//                    CTimer::Update();
//                    //Command<COMMAND_DRESS_CHAR>(CPools::GetPedRef(player));
//                    player->m_nModelIndex = -1;
//                    player->SetModelIndex(modelIndex);
//                    CWorld::Add(player);
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('N', 2000)) {
//                CVehicle *vehicle = FindPlayerVehicle();
//                if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
//                    CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
//                    if (!automobile->m_carDamage.GetWheelStatus(0)) {
//                        automobile->m_carDamage.SetWheelStatus(0, 1);
//                        CMessages::AddMessageJumpQ(L"damage", 2000, false);
//                    }
//                    else {
//                        automobile->m_carDamage.SetWheelStatus(0, 0);
//                        CMessages::AddMessageJumpQ(L"fix", 2000, false);
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CStreaming.h"
//#include "CCivilianPed.h"
//#include "CWorld.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('M', 2000)) {
//                CStreaming::RequestSpecialChar(20, "SAM", 6); // 023C: load_special_actor 21 'SAM'
//                if (CStreaming::HasSpecialCharLoaded(20)) {   // 023D: special_actor 21 loaded 
//                    CPed *ped = new CCivilianPed(PEDTYPE_CIVMALE, 129);
//                    if (ped) {
//                        ped->SetPosition(FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 2.0f, 0.0f)));
//                        CWorld::Add(ped);
//                    }
//                    CStreaming::SetMissionDoesntRequireSpecialChar(20); // 0296: unload_special_actor 21 
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CClumpModelInfo.h"
//#include "CMessages.h"
//
//RwObject* m_GetCurrentAtomicObjectCB(RwObject* object, void* data) {
//    return ((RwObject* (__cdecl *)(RwObject*, void*))0x59F1E0)(object, data);
//}
//
//using namespace plugin;
//
//class Test {
//public:
//    static void SetComponentAlpha(CVehicle *vehicle, RwFrame *frame, int alpha) {
//        RpAtomic *atomic = nullptr;
//        if (frame) {
//            RwFrameForAllObjects(frame, m_GetCurrentAtomicObjectCB, &atomic);
//            if (atomic) {
//                vehicle->SetComponentAtomicAlpha(atomic, alpha);
//                CMessages::AddMessageJumpQ(L"atomic yes", 2000, 0);
//            }
//            else
//                CMessages::AddMessageJumpQ(L"atomic no", 2000, 0);
//        }
//    }
//    
//    Test() {
//        Events::gameProcessEvent += [] {
//            CVehicle* vehicle = FindPlayerVehicle();
//            if (vehicle) {
//                CAutomobile *car = reinterpret_cast<CAutomobile *>(vehicle);
//                KeyCheck::Update();
//                RwFrame *frame = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "wiper_or");
//                if (KeyCheck::CheckWithDelay('M', 2000)) 
//                    SetComponentAlpha(vehicle, frame, 50);
//                    //SetComponentAlpha(vehicle, car->m_aCarNodes[CAR_BONNET], 200);
//                if (KeyCheck::CheckWithDelay('N', 2000)) 
//                    SetComponentAlpha(vehicle, frame, 255);
//                    //SetComponentAlpha(vehicle, car->m_aCarNodes[CAR_BONNET], 255);
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "eScriptCommands.h"
//#include "extensions\ScriptCommands.h"
//#include "extensions\KeyCheck.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('M', 2000)) {
//                Command<COMMAND_LOAD_MISSION_TEXT>("GENERA1");
//                Command<COMMAND_PRINT_NOW>("GEN1_A", 2000, 1);
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CPed.h"
//#include "extensions\KeyCheck.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static CPed *GetRandomPed(CVector const &posn, float radius) {
//        std::vector<CPed *> peds;
//        for (auto ped : CPools::ms_pPedPool) {
//            if (DistanceBetweenPoints(ped->GetPosition(), posn) <= radius)
//                peds.push_back(ped);
//        }
//        return peds.empty() ? nullptr : peds[plugin::Random(0, peds.size() - 1)];
//    }
//    
//    Test() {
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('M', 1000)) {
//                CPed *player = FindPlayerPed();
//                if (player) {
//                    CPed *ped = GetRandomPed(player->GetPosition(), 15.0f);
//                    if (ped) {
//                        ped->SetObjective(OBJECTIVE_ENTER_CAR_AS_DRIVER, player->m_pVehicle);
//                        ped->WarpPedIntoCar(player->m_pVehicle);
//                    }
//                }
//            }
//        };
//    }
//} test;

//CPed *ped; CVehicle *vehicle;
//ped->SetObjective(OBJECTIVE_ENTER_CAR_AS_DRIVER, vehicle);
//ped->WarpPedIntoCar(vehicle);

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CUserDisplay.h"
//
//int &var_$3402 = *(int *)0x8247A8;
//
//using namespace plugin;
//
//class Test {
//public:
//    static int timer;
//    enum eTimerState { STATE_ADD, STATE_CLEAR };
//    static eTimerState m_currentState;
//
//    Test() {
//        Events::drawingEvent += [] {
//            gamefont::Print({
//                Format("id %d", CUserDisplay::OnscnTimer.m_aClocks[0].m_nVarId),
//                Format("direction %d", CUserDisplay::OnscnTimer.m_aClocks[0].m_nTimerDirection),
//                Format("enabled %d", CUserDisplay::OnscnTimer.m_aClocks[0].m_bEnabled),
//                Format("text %s", CUserDisplay::OnscnTimer.m_aClocks[0].m_acDisplayedText),
//                Format("key %s", &CUserDisplay::OnscnTimer.m_aClocks[0].m_acDescriptionTextKey),
//                Format("var $3402 %d", var_$3402)
//            }, 10, 10, 1, FONT_DEFAULT, 0.75f, 0.75f, color::White);
//
//            KeyCheck::Update();
//            switch (m_currentState) {
//            case STATE_ADD:
//                if (!CUserDisplay::OnscnTimer.m_aClocks[0].m_bEnabled) {
//                    if (KeyCheck::CheckWithDelay('M', 1000)) {
//                        var_$3402 = 120000; //in CLEO $3402 = 120000
//                        CUserDisplay::OnscnTimer.AddClock(timer, "R_TIME", 1); //03C3: set_timer_with_text_to $3402 type 1 text 'R_TIME'
//                        m_currentState = STATE_CLEAR;
//                    }
//                    if (KeyCheck::CheckWithDelay('U', 1000)) {
//                        CUserDisplay::OnscnTimer.AddClock(timer, "R_TIME", 0); //03C3: set_timer_with_text_to $3402 type 0 text 'R_TIME'
//                        m_currentState = STATE_CLEAR;
//                    }
//                }
//                break;
//            case STATE_CLEAR:
//                if (KeyCheck::CheckWithDelay('N', 1000) || var_$3402 > 121000 || var_$3402 < 1) {
//                    CUserDisplay::OnscnTimer.ClearClock(timer); //014F: stop_timer $3402
//                    var_$3402 = 0; //$3402 = 0
//                    m_currentState = STATE_ADD;
//                }
//                break;
//            }
//        };
//    }
//} test;
//
//int Test::timer = 13608; 
//Test::eTimerState Test::m_currentState = STATE_ADD;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CTimer.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    enum eTimerState { STATE_START, STATE_PLAY };
//    static eTimerState m_currentState;
//    static int m_min, m_sec, m_millisec;
//
//    Test() {
//        Events::drawingEvent += [] {
//            switch (m_currentState) {
//            case STATE_START:
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay('M', 2000)) {
//                    m_min = 0; m_sec = 0; m_millisec = CTimer::m_snTimeInMilliseconds;
//                    m_currentState = STATE_PLAY;
//                }
//                break;
//            case STATE_PLAY:
//                if (CTimer::m_snTimeInMilliseconds > (m_millisec + 1000)) {
//                    m_millisec = CTimer::m_snTimeInMilliseconds;
//                    if (m_sec < 59)
//                        m_sec++;
//                    else {
//                        m_sec = 0;
//                        if (m_min < 2)
//                            m_min++;
//                        else
//                            m_currentState = STATE_START;
//                    }
//                }
//                gamefont::Print({
//                    Format("min %02d : sec %02d", m_min, m_sec)
//                }, 10, 10, 1, FONT_DEFAULT, 0.75f, 0.75f, color::White);
//                break;
//            }
//        };
//    }
//} test;
//
//Test::eTimerState Test::m_currentState = STATE_START;
//int Test::m_min = 0;
//int Test::m_sec = 0;
//int Test::m_millisec = 0;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//#include "CModelInfo.h"
//
//using namespace plugin;
//
//class Test {
//public:
//
//    static void EntityType(CEntity *entity) {
//        if (CModelInfo::ms_modelInfoPtrs[entity->m_nModelIndex]->m_nType == MODEL_INFO_PED)
//            CMessages::AddMessageJumpQ(L"ped", 2000, 0);
//        if (CModelInfo::ms_modelInfoPtrs[entity->m_nModelIndex]->m_nType == MODEL_INFO_VEHICLE)
//            CMessages::AddMessageJumpQ(L"vehicle", 2000, 0);
//    }
//
//    Test() {
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('M', 2000)) {
//                CPed *player = FindPlayerPed();
//                if (player) {
//                    if (player->m_pVehicle && player->m_bInVehicle)
//                        EntityType(player->m_pVehicle);
//                    else
//                        EntityType(player);
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CWorld.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::drawingEvent += [] {
//            gamefont::Print({
//                Format("test = %d", CWorld::Players[CWorld::PlayerInFocus].m_nPlayerState)
//            }, 10, 200, 1, FONT_DEFAULT, 0.75f, 0.75f, color::White);
//        };
//    }
//} test;

//#include "plugin.h"
//#include <vector>
//#include <fstream>
//#include <string>
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//
//using namespace plugin;
//using namespace std;
//
////#pragma pack(push, 4)
//struct MyVehicleSampleData {
//    char name[16];
//    int m_nModelId;
//    int m_nEngineFarSample;
//    int m_nEngineNearSample;
//    int m_nHornSample;
//    int m_nHornFreq;
//    int m_nSirenSample;
//    int m_nSirenFreq;
//    int m_nDoorSounds;
//};
////#pragma pack(pop)
//
//    MyVehicleSampleData dataSound;
//    vector<MyVehicleSampleData> carSound;
//
//class Test {
//public:
//    static int currentValue;
//
//    // cAudioManager::ProcessVehicleSirenOrAlarm
//    static void Patch_5F0420();    static void Patch_5F0448();
//
//    // cAudioManager::ProcessVehicleEngine
//    static void Patch_5F2799();    static void Patch_5F27F2();
//    static void Patch_5F281D();    static void Patch_5F2873();
//
//    // cAudioManager::ProcessPlayersVehicleEngine
//    static void Patch_5F1427();
//
//    static int __stdcall GetVehicleSettingsIndex(int id) {
//        for (unsigned int i = 0; i < carSound.size(); i++) {
//            if (carSound[i].m_nModelId == id)
//                return i;
//        }
//        return 0;
//    }
//
//    static int __stdcall GetEngineFarSample(int id_130) { return carSound[GetVehicleSettingsIndex(id_130 + 130)].m_nEngineFarSample; }
//    static int __stdcall GetEngineNearSample(int id_130) { return carSound[GetVehicleSettingsIndex(id_130 + 130)].m_nEngineNearSample; }
//    static int __stdcall GetHornSample(int id_130) { return carSound[GetVehicleSettingsIndex(id_130 + 130)].m_nHornSample; }
//    static int __stdcall GetHornFreq(int id_130) { return carSound[GetVehicleSettingsIndex(id_130 + 130)].m_nHornFreq; }
//    static int __stdcall GetSirenSample(int id_130) { return carSound[GetVehicleSettingsIndex(id_130 + 130)].m_nSirenSample; }
//    static int __stdcall GetSirenFreq(int id_130) { return carSound[GetVehicleSettingsIndex(id_130 + 130)].m_nSirenFreq; }
//
//    Test() {
//        ifstream stream(GAME_PATH("data\\carsound.cfg"));
//        if (!stream.is_open())
//            return;
//        for (string line; getline(stream, line); ) {
//            if (line.length() > 0 && line[0] != ';' && line[0] != '#')
//                if (sscanf(line.c_str(), "%s %d %d %d %d %d %d %d %d", &dataSound.name, &dataSound.m_nModelId, &dataSound.m_nEngineFarSample, &dataSound.m_nEngineNearSample, &dataSound.m_nHornSample, &dataSound.m_nHornFreq, &dataSound.m_nSirenSample, &dataSound.m_nSirenFreq, &dataSound.m_nDoorSounds) == 9)
//                    carSound.push_back(dataSound);
//        }
//
//        patch::RedirectJump(0x5F0420, Patch_5F0420);
//        patch::RedirectJump(0x5F0448, Patch_5F0448);
//        
//        patch::RedirectJump(0x5F2799, Patch_5F2799);
//        patch::RedirectJump(0x5F27F2, Patch_5F27F2);
//        patch::RedirectJump(0x5F281D, Patch_5F281D);
//        patch::RedirectJump(0x5F2873, Patch_5F2873);
//
//        patch::RedirectJump(0x5F1427, Patch_5F1427);
//
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('M', 1000)) {
//                int i = 106;
//                static char message[256];
//                snprintf(message, 256, "name %s far %d near %d;", carSound[i].name, carSound[i].m_nEngineFarSample, carSound[i].m_nEngineNearSample);
//                CMessages::AddMessageJumpQ(message, 10000, false);
//            }
//        };
//
//        /*Events::drawingEvent += [] {
//            gamefont::Print({
//                Format("test = %d", index)
//            }, 10, 100, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);
//        };*/
//
//    }
//} test;
//
//int Test::currentValue;
//
//void __declspec(naked) Test::Patch_5F0420() {
//    __asm {
//        mov ecx, [ebp + 20]
//        pushad
//        push ecx
//        call GetSirenSample
//        mov currentValue, eax
//        popad
//        mov eax, currentValue
//        mov[esi + 1Ch], eax
//        mov ecx, [ebp + 20]
//        pushad
//        push ecx
//        call GetSirenFreq
//        mov currentValue, eax
//        popad
//        mov eax, currentValue
//        mov edx, 0x5F0446
//        jmp edx
//    }
//}
//
//void __declspec(naked) Test::Patch_5F0448() {
//    __asm {
//        mov  ecx, [ebp + 20]
//        pushad
//        push ecx
//        call GetHornSample
//        mov currentValue, eax
//        popad
//        mov eax, currentValue
//        mov[esi + 1Ch], eax
//        mov  ecx, [ebp + 20]
//        pushad
//        push ecx
//        call GetHornFreq
//        mov currentValue, eax
//        popad
//        mov eax, currentValue
//        mov edx, 0x5F0471
//        jmp edx
//    }
//}
//
//void __declspec(naked) Test::Patch_5F2799() {
//    __asm {
//        mov  esi, [edi + 20]
//        pushad
//        push esi
//        call GetEngineNearSample
//        mov currentValue, eax
//        popad
//        mov eax, currentValue
//        add eax, 284
//        mov[ebx + 1Ch], eax
//        mov edx, 0x5F27B5
//        jmp edx
//    }
//}
//
//void __declspec(naked) Test::Patch_5F27F2() {
//    __asm {
//        mov  ecx, [edi + 20]
//        pushad
//        push ecx
//        call GetEngineFarSample
//        mov currentValue, eax
//        popad
//        mov eax, currentValue
//        mov edx, 0x5F2805
//        jmp edx
//    }
//}
//
//void __declspec(naked) Test::Patch_5F281D() {
//    __asm {
//        mov  esi, [edi + 20]
//        pushad
//        push esi
//        call GetEngineNearSample
//        mov currentValue, eax
//        popad
//        mov eax, currentValue
//        add eax, 284
//        mov[ebx + 1Ch], eax
//        mov edx, 0x5F2839
//        jmp edx
//    }
//}
//
//void __declspec(naked) Test::Patch_5F2873() {
//    __asm {
//        mov  ecx, [edi + 20]
//        pushad
//        push ecx
//        call GetEngineFarSample
//        mov currentValue, eax
//        popad
//        mov eax, currentValue
//        mov edx, 0x5F2886
//        jmp edx
//    }
//}
//
//void __declspec(naked) Test::Patch_5F1427() {
//    __asm {
//        mov  eax, [esi + 20]
//        pushad
//        push eax
//        call GetEngineNearSample
//        mov currentValue, eax
//        popad
//        mov eax, currentValue
//        mov[esp + 20], eax
//        mov edx, 0x5F1449
//        jmp edx
//    }
//}

//#include "plugin.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//#include "extensions\KeyCheck.h"
//
//#include "CModelInfo.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    enum eAudioState { STATE_LOAD, STATE_PLAY, STATE_CLEAR };
//    static eAudioState m_currentState;
//    
//    Test() {
//        //CModelInfo::AddVehicleModel(237);
//
//        Events::gameProcessEvent += [] {
//            switch (m_currentState) {
//            case STATE_LOAD:
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay('M', 2000)) {
//                    Command<COMMAND_LOAD_MISSION_AUDIO>(1, "FIN_1a");
//                    m_currentState = STATE_PLAY;
//                }
//                break;
//            case STATE_PLAY:
//                if (Command<COMMAND_HAS_MISSION_AUDIO_LOADED>(1)) {
//                    Command<COMMAND_PLAY_MISSION_AUDIO>(1);
//                    m_currentState = STATE_CLEAR;
//                }
//                break;
//            case STATE_CLEAR:
//                if (Command<COMMAND_HAS_MISSION_AUDIO_FINISHED>(1)) {
//                    Command<COMMAND_CLEAR_MISSION_AUDIO>(1);
//                    m_currentState = STATE_LOAD;
//                }
//                break;
//            }
//        };
//    }
//} test;
//
//Test::eAudioState Test::m_currentState = STATE_LOAD;

//#include "plugin.h"
//#include "CTheScripts.h"
//#include "extensions\KeyCheck.h"
//
//unsigned int &OnAMissionFlag = *(unsigned int *)0x978748;
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::drawingEvent += [] {
//            gamefont::Print({
//                Format("OnAMissionFlag = %d", CTheScripts::ScriptSpace[OnAMissionFlag])
//            }, 10, 200, 1, FONT_DEFAULT, 0.75f, 0.75f, color::Orange);
//           
//            CPed *player = FindPlayerPed();
//            if (player) {
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay('M', 1000)) {
//                    if (CTheScripts::ScriptSpace[OnAMissionFlag])
//                        CTheScripts::ScriptSpace[OnAMissionFlag] = 0;
//                    else
//                        CTheScripts::ScriptSpace[OnAMissionFlag] = 1;
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CMessages.h"
//#include "CWorld.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//#include "extensions\KeyCheck.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                CVector point = { 241.6f, -1283.0f, 10.9f };
//                if (Command<COMMAND_LOCATE_PLAYER_ANY_MEANS_3D>(CWorld::PlayerInFocus, point.x, point.y, point.z, 2.0, 2.0, 2.0))
//                    CMessages::AddMessageJumpQ(L"Yes", 1000, 1);
//                //
//                static int sphere;
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay('M', 2000)) {
//                    CVector pos = FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 5.0f, 0.0f));
//                    Command<COMMAND_ADD_SPHERE>(pos.x, pos.y, pos.z, 2.0, &sphere);
//                    CMessages::AddMessageJumpQ(L"Create", 2000, 1);
//                }
//                if (KeyCheck::CheckWithDelay('N', 2000)) {
//                    Command<COMMAND_REMOVE_SPHERE>(sphere);
//                    CMessages::AddMessageJumpQ(L"Delete", 2000, 1);
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CMessages.h"
//#include "CWorld.h"
//#include "CStreaming.h"
//#include "CTimer.h"
//#include "ePedModel.h"
//#include "ePedType.h"
//#include "CCivilianPed.h"
//
//using namespace plugin;
//
//class Test {
//public:
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
//    static CPed *CreatePed(ePedType pedType, unsigned int modelIndex) {
//        CPed *ped = nullptr;
//        if (LoadModel(modelIndex)) {
//            ped = new CCivilianPed(pedType, modelIndex);
//            if (ped) {
//                ped->SetPosition(FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 2.0f, 0.0f)));
//                CWorld::Add(ped);
//            }
//        }
//        return ped;
//    }
//
//    Test() {
//        static int keyPressTime = 0;
//
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                if (KeyPressed('M') && CTimer::m_snTimeInMilliseconds > (keyPressTime + 5000)) {
//                    keyPressTime = CTimer::m_snTimeInMilliseconds;
//                    CPed *ped = CreatePed(PEDTYPE_CIVFEMALE, MODEL_HFYST);
//                    if (ped)
//                        CMessages::AddMessageJumpQ(L"CreatePed", 1000, 0);;
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//#include "CWorld.h"
//#include "eWeaponModel.h"
//#include "eWeaponType.h"
//#include "CStreaming.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static int m_test;
//
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('B', 1000)) {
//                static char message[256];
//                snprintf(message, 256, "test = %d", m_test);
//                CMessages::AddMessageJumpQ(message, 1000, false);
//            }
//            if (player) {
//                if (KeyCheck::CheckWithDelay('N', 1000)) {
//                    //if (player->m_bInVehicle)
//                    if (Command<COMMAND_IS_PLAYER_IN_ANY_CAR>(CWorld::PlayerInFocus))
//                        CMessages::AddMessageJumpQ(L"in car", 1000, 0);
//                    else
//                        CMessages::AddMessageJumpQ(L"not in car", 1000, 0);
//                }
//                if (KeyCheck::CheckWithDelay('M', 1000)) {
//                    //CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 1000;
//                    Command <COMMAND_ADD_SCORE>(CWorld::PlayerInFocus, 1000);
//
//                    /*CStreaming::RequestModel(MODEL_COLT45, 2);
//                    CStreaming::LoadAllRequestedModels(false);
//                    player->GiveWeapon(WEAPONTYPE_PISTOL, 100, true);
//                    player->SetCurrentWeapon(WEAPONTYPE_PISTOL);
//                    CStreaming::SetModelIsDeletable(MODEL_COLT45);*/
//                    Command<COMMAND_REQUEST_MODEL>(MODEL_COLT45, 2);
//                    Command<COMMAND_LOAD_ALL_MODELS_NOW>(false);
//                    if (Command<COMMAND_HAS_MODEL_LOADED>(MODEL_COLT45)) {
//                        Command<COMMAND_GIVE_WEAPON_TO_PLAYER>(CWorld::PlayerInFocus, WEAPONTYPE_PISTOL, 100, true);
//                        Command<COMMAND_SET_CURRENT_PLAYER_WEAPON>(CWorld::PlayerInFocus, WEAPONTYPE_PISTOL);
//                        Command<COMMAND_MARK_MODEL_AS_NO_LONGER_NEEDED>(MODEL_COLT45);
//                    }
//                }
//            }
//        };
//    }
//} test;
//
//int Test::m_test = 3;

//#include "plugin.h"
//#include "CSprite2d.h"
//#include "CFileLoader.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static RwTexture *textureTest;
//    static RwTexDictionary *m_txd;
//
//    static float ScreenCoord(float a) {
//        return static_cast<int>(a * (static_cast<float>(RsGlobal.maximumHeight) / 900.0f));
//    }
//
//    Test() {
//        Events::initRwEvent += [] {
//            m_txd = CFileLoader::LoadTexDictionary(GAME_PATH("models\\MYTEST.TXD"));
//            textureTest = GetFirstTexture(m_txd);
//        };
//
//        Events::drawingEvent += [] {
//            if (textureTest) {
//                RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, textureTest->raster);
//                CSprite2d::SetVertices(CRect(ScreenCoord(120.0f), RsGlobal.maximumHeight - ScreenCoord(115.0f), ScreenCoord(200.0f), RsGlobal.maximumHeight - ScreenCoord(35.0f)), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255), CRGBA(255, 255, 255, 255));
//                RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
//            }
//        };
//
//        Events::shutdownRwEvent += [] {
//            RwTexDictionaryDestroy(m_txd);
//        };
//    }
//} test;
//
//RwTexDictionary  *Test::m_txd;
//RwTexture *Test::textureTest;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static int blip;
//    
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            KeyCheck::Update();
//            if (player && KeyCheck::CheckWithDelay('M', 1000)) 
//                Command<COMMAND_ADD_BLIP_FOR_CHAR_OLD>(CPools::GetPedRef(player), 4, 2, &blip);
//                //Command<COMMAND_ADD_BLIP_FOR_CHAR>(CPools::GetPedRef(player), &blip);
//                //CVector offset = { 0.0f, 10.0f, 0.0f };
//                //CVector point = player->m_placement * offset;
//                //Command<COMMAND_ADD_BLIP_FOR_COORD_OLD>(point.x, point.y, point.z, 1, 3, &blip);
//            if (player && KeyCheck::CheckWithDelay('N', 1000)) 
//                Command<COMMAND_REMOVE_BLIP>(blip);
//        };
//    }
//} test;
//
//int Test::blip;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "CModelInfo.h"
//#include "CFont.h"
//#include "CStreaming.h"
//
//using namespace plugin;
//
//class MyPlugin {
//public:
//    MyPlugin() {
//
//        Events::drawingEvent += [] {
//            CFont::SetScale(0.5f, 1.0f);
//            CFont::SetColor(CRGBA(255, 255, 255, 255));
//            CFont::SetJustifyOn();
//            CFont::SetFontStyle(2);
//            CFont::SetPropOn();
//            CFont::SetWrapx(600.0f);
//            wchar_t text[32];
//            swprintf(text, L"TxdIndex %d", CModelInfo::ms_modelInfoPtrs[162]->m_nTxdIndex);
//            CFont::PrintString(10.0f, 30.0f, text);
//
//            unsigned char oldFlags = CStreaming::ms_aInfoForModel[162].m_nFlags;
//            swprintf(text, L"Flag %d", oldFlags);
//            CFont::PrintString(10.0f, 50.0f, text);
//
//            swprintf(text, L"LoadState %d", CStreaming::ms_aInfoForModel[162].m_nLoadState);
//            CFont::PrintString(10.0f, 70.0f, text);
//        };
//
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            //unsigned char oldLoadState;
//            if (KeyCheck::CheckWithDelay(99, 1000)) {
//                //oldLoadState = CStreaming::ms_aInfoForModel[162].m_nLoadState;
//                
//                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino4");
//                CStreaming::ms_aInfoForModel[162].m_nLoadState = 0;
//                CStreaming::ms_aInfoForModel[162].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[162].m_nLoadState = oldLoadState;
//            }
//            if (KeyCheck::CheckWithDelay(98, 1000)) {
//                //oldLoadState = CStreaming::ms_aInfoForModel[162].m_nLoadState;
//                
//                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino3");
//                CStreaming::ms_aInfoForModel[162].m_nLoadState = 0;
//                CStreaming::ms_aInfoForModel[162].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[162].m_nLoadState = oldLoadState;
//            }
//            if (KeyCheck::CheckWithDelay(97, 1000)) {
//                //oldLoadState = CStreaming::ms_aInfoForModel[162].m_nLoadState;
//                
//                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino2");
//                CStreaming::ms_aInfoForModel[162].m_nLoadState = 0;
//                CStreaming::ms_aInfoForModel[162].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[162].m_nLoadState = oldLoadState;
//            }
//            if (KeyCheck::CheckWithDelay(96, 1000)) {
//                //oldLoadState = CStreaming::ms_aInfoForModel[162].m_nLoadState;
//                
//                CModelInfo::ms_modelInfoPtrs[162]->ClearTexDictionary();
//                CModelInfo::ms_modelInfoPtrs[162]->SetTexDictionary("rhino");
//                CStreaming::ms_aInfoForModel[162].m_nLoadState = 0;
//                CStreaming::ms_aInfoForModel[162].m_nFlags = 0;
//                //CStreaming::ms_aInfoForModel[162].m_nLoadState = oldLoadState;
//            }
//        };
//    }
//} myPlugin;

//#include "plugin.h"
//#include "CPed.h"
//#include "extensions\KeyCheck.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    static CPed *GetRandomPed(CVector const &posn, float radius) {
//        std::vector<CPed *> peds;
//        for (auto ped : CPools::ms_pPedPool) {
//            if (DistanceBetweenPoints(ped->GetPosition(), posn) <= radius)
//                peds.push_back(ped);
//        }
//        return peds.empty() ? nullptr : peds[plugin::Random(0, peds.size() - 1)];
//    }
//    
//    Test() {
//        Events::gameProcessEvent += [] {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('M', 500)) {
//                CPed *player = FindPlayerPed();
//                if (player) {
//                    /*CVector offset = { 0.0f, 20.0f, 0.0f };
//                    CVector point = player->m_placement * offset;
//                    player->m_nPedFlags.bHasObjectiveCompleted = 0;
//                    player->SetObjective(OBJECTIVE_RUN_TO_AREA, point);*/
//                    CPed *ped = GetRandomPed(player->GetPosition(), 15.0f);
//                    if (ped) {
//                        //CVector offset = { 0.0f, 2.0f, 0.0f };
//                        //CVector point = player->m_placement * offset;
//                        ped->m_nPedFlags.bHasObjectiveCompleted = 0;
//                        //ped->SetObjective(OBJECTIVE_RUN_TO_AREA, point);
//                        //ped->SetObjective(OBJECTIVE_SOLICIT_VEHICLE, player->m_pVehicle);
//                        ped->SetObjective(OBJECTIVE_ENTER_CAR_AS_PASSENGER, player->m_pVehicle);
//                        ped->m_dwObjectiveTimer = 60000;
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CPed.h"
//#include "CWorld.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pObjectPool->m_nSize; i++) {
//                    CObject *object = CPools::ms_pObjectPool->GetAt(i);
//                    if (object && /*object->m_nModelIndex == 370 &&*/ DistanceBetweenPoints(object->GetPosition(), player->GetPosition()) < 5.0f) {
//                        static char message[256];
//                        snprintf(message, 256, "ID = %d", object->m_nModelIndex);
//                        CMessages::AddMessageJumpQ(message, 1000, false);
//                        //object->m_placement.pos.x += 2.0f;
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CPed.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (auto car : CPools::ms_pVehiclePool) {
//                    if (DistanceBetweenPoints(car->GetPosition(), player->GetPosition()) < 20.0f && car->m_nVehicleClass == VEHICLE_AUTOMOBILE && car->m_fHealth > 50.0f) {
//                        if (car->m_pDriver && car->m_pDriver != player)
//                            car->m_pDriver->SetExitCar(car, 15);
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle();
//            KeyCheck::Update();
//            if (vehicle && KeyCheck::CheckWithDelay('M', 200)) {
//                CVector pos = { 0.0f, 0.0f, 0.0f };
//                Command<COMMAND_GET_CAR_COORDINATES>(CPools::GetVehicleRef(vehicle), &pos.x, &pos.y, &pos.z);
//                static char message[256];
//                snprintf(message, 256, "x = %.2f; y = %.2f; z = %.2f; ", pos.x, pos.y, pos.z);
//                CMessages::AddMessageJumpQ(message, 3000, false);
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//                    if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
//                        static char message[256];
//                        snprintf(message, 256, "MaxSpeed = %.2f", vehicle->m_pHandlingData->m_transmissionData.m_fMaxSpeed);
//                        CMessages::AddMessageJumpQ(message, 3000, false);
//                        vehicle->m_autopilot.m_nCruiseSpeed = 30.0f;
//                        float maxSpeed = 60.0f * vehicle->m_pHandlingData->m_transmissionData.m_fMaxSpeed;
//                        if (vehicle->m_autopilot.m_nCruiseSpeed >= maxSpeed)
//                            maxSpeed = vehicle->m_autopilot.m_nCruiseSpeed;
//                        vehicle->m_autopilot.m_nCruiseSpeed = maxSpeed;
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "extensions\KeyCheck.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle();
//            static int blip;
//            KeyCheck::Update();
//            if (vehicle && KeyCheck::CheckWithDelay('M', 200)) {
//                Command<COMMAND_ADD_BLIP_FOR_CAR>(CPools::GetVehicleRef(vehicle), &blip);
//            }
//            if (vehicle && KeyCheck::CheckWithDelay('N', 200)) {
//                Command<COMMAND_REMOVE_BLIP>(&blip);
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CWorld.h"
//#include "CTimer.h"
//#include "CVehicle.h"
//#include "CCarCtrl.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//                    if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
//                        CVector offset = { 0.0f, 10.0f, 0.0f };
//                        CVector point = vehicle->m_placement * offset;
//                        
//                        if (point.z <= -100.0f)
//                            point.z = CWorld::FindGroundZForCoord(point.x, point.y);
//                        point.z = vehicle->GetDistanceFromCentreOfMassToBaseOfModel() + point.z;
//                        if (CCarCtrl::JoinCarWithRoadSystemGotoCoors(vehicle, point))
//                            vehicle->m_autopilot.m_nCarMission = 9;
//                        else
//                            vehicle->m_autopilot.m_nCarMission = 8;
//                        vehicle->m_nType |= 0x18;
//                        vehicle->m_nVehicleFlags.bIsEngineOn = 1;
//                        if (vehicle->m_autopilot.m_nCruiseSpeed <= 6)
//                            vehicle->m_autopilot.m_nCruiseSpeed = 6;
//                        else
//                            vehicle->m_autopilot.m_nCruiseSpeed = vehicle->m_autopilot.m_nCruiseSpeed;
//                        vehicle->m_autopilot.m_nTimeToStartMission = CTimer::m_snTimeInMilliseconds;
//
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "extensions\ScriptCommands.h"
//#include "eScriptCommands.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//                    if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
//                        CVector offset = {0.0f, 10.0f, 0.0f};
//                        CVector point = vehicle->m_placement * offset;
//                        Command<COMMAND_CAR_GOTO_COORDINATES>(CPools::GetVehicleRef(vehicle), point.x, point.y, point.z);
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CFont.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::drawingEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (player) {
//                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//                    if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
//                        CFont::SetScale(0.5f, 1.0f);
//                        CFont::SetColor(CRGBA(255, 255, 255, 255));
//                        CFont::SetJustifyOn();
//                        CFont::SetFontStyle(1);
//                        CFont::SetPropOn();
//                        CFont::SetWrapx(600.0f);
//                        wchar_t text[64];
//                        swprintf(text, L"ID vehicle %.d", vehicle->m_nModelIndex);
//                        CFont::PrintString(10.0f, 10.0f, text);
//                    }
//                }
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CWorld.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            KeyCheck::Update();
//            if (player && player->m_bInVehicle && player->m_pVehicle && KeyCheck::CheckWithDelay('M', 200)) {
//                player->m_pVehicle->m_fHealth = 1000.0f;
//                player->m_pVehicle->m_nPrimaryColor = 0;
//                player->m_pVehicle->m_nSecondaryColor = 0;
//                static char message[256];
//                snprintf(message, 256, "x = %.2f; y = %.2f; z = %.2f;", player->m_pVehicle->GetPosition().x, player->m_pVehicle->GetPosition().y, player->m_pVehicle->GetPosition().z);
//                CMessages::AddMessageJumpQ(message, 3000, false);
//                CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 5000;
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CWorld.h"
//#include "extensions\KeyCheck.h"
//#include "CMessages.h"
//
//using namespace plugin;
//
//class Test {
//public:
//    Test() {
//        Events::gameProcessEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle();
//            KeyCheck::Update();
//            if (vehicle && KeyCheck::CheckWithDelay('M', 200)) {
//                vehicle->m_fHealth = 1000.0f;
//                vehicle->m_nPrimaryColor = 0;
//                vehicle->m_nSecondaryColor = 0;
//                static char message[256];
//                snprintf(message, 256, "x = %.2f; y = %.2f; z = %.2f;", vehicle->GetPosition().x, vehicle->GetPosition().y, vehicle->GetPosition().z);
//                CMessages::AddMessageJumpQ(message, 3000, false);
//                CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 5000;
//            }
//        };
//    }
//} test;

//#include "plugin.h"
//#include "CClumpModelInfo.h"
//#include "CParticle.h"
//#include "CWorld.h"
//#include "extensions\KeyCheck.h"
//
//using namespace plugin;
//
//class ParticleTest {
//public:
//    static void CreateParticleOnVehicleNode(CVehicle *vehicle, char const *nodeName) {
//        RwFrame *node = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, nodeName); // �������� ��������� �� ��������
//        if (node) {
//            RwMatrix *m = RwFrameGetLTM(node); // �������� ������� ���������� � ���������� ������������
//            CParticle::AddParticle(PARTICLE_FLYERS, CVector(m->pos.x, m->pos.y, m->pos.z), CVector(0, 0, 0), nullptr, 0.4f, 0, 0, 0, 0);
//        }
//    }
//
//    ParticleTest() {
//        Events::gameProcessEvent += [] {
//            CPed *player = FindPlayerPed();
//            if (KeyPressed('I'))
//                CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 5000;
//            if (player && player->m_bInVehicle && player->m_pVehicle && player->m_pVehicle->m_fGasPedal > 0) {
//                CreateParticleOnVehicleNode(player->m_pVehicle, "wheel_lb_dummy");
//                CreateParticleOnVehicleNode(player->m_pVehicle, "wheel_rb_dummy");
//            }
//        };
//    }
//} particleTest;

//#include "plugin.h"
//#include "game_vc\common.h"
//#include <map>
//
//using namespace plugin;
//
//class VehicleTextures {
//public:
//    VehicleTextures() {
//        static CdeclEvent<AddressList<0x4C9ED1, H_CALL, 0x4C9F23, H_CALL, 0x4CA10D, H_CALL, 0x5813D1, H_CALL>,
//            PRIORITY_BEFORE, ArgPickN<CEntity*, 0>, void(CEntity*)> myOnRenderOneNonRoad;
//
//        static std::map<RpMaterial*, RwTexture *> originalTextures;
//
//        myOnRenderOneNonRoad.before += [](CEntity *entity) {
//            if (KeyPressed('Z') && entity == FindPlayerVehicle()) {
//                CVehicle *vehicle = reinterpret_cast<CVehicle *>(entity);
//                RpClumpForAllAtomics(vehicle->m_pRwClump, [](RpAtomic *atomic, void *data) {
//                    RpGeometryForAllMaterials(atomic->geometry, [](RpMaterial *material, void *data) {
//                        if (originalTextures.find(material) == originalTextures.end())
//                            originalTextures[material] = material->texture;
//                        material->texture = *reinterpret_cast<RwTexture **>(0x77FA58);
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


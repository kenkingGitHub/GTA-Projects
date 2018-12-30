#include "plugin.h"
#include "CMessages.h"

using namespace plugin;

class Test {
public:
    Test() {
        Events::gameProcessEvent += [] {
            CPed *player = FindPlayerPed();
            if (player) {
                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
                    if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
                        static char message[256];
                        snprintf(message, 256, "MaxSpeed = %.2f", vehicle->m_pHandlingData->m_transmissionData.m_fMaxSpeed);
                        CMessages::AddMessageJumpQ(message, 3000, false);
                        vehicle->m_autopilot.m_nCruiseSpeed = 30.0f;
                        float maxSpeed = 60.0f * vehicle->m_pHandlingData->m_transmissionData.m_fMaxSpeed;
                        if (vehicle->m_autopilot.m_nCruiseSpeed >= maxSpeed)
                            maxSpeed = vehicle->m_autopilot.m_nCruiseSpeed;
                        vehicle->m_autopilot.m_nCruiseSpeed = maxSpeed;
                    }
                }
            }
        };
    }
} test;

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
//        RwFrame *node = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, nodeName); // получить компонент по названию
//        if (node) {
//            RwMatrix *m = RwFrameGetLTM(node); // получить матрицу компонента в глобальном пространстве
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

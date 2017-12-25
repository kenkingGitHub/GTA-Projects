#include "plugin_III.h"
#include "game_III\common.h"
#include "extensions\KeyCheck.h"
#include "game_III\CMessages.h"
#include "game_III\CVector.h"
#include "game_III\CTheScripts.h"

using namespace plugin;

class MyPlugin {
public:
    MyPlugin() {
        static float dist;
        static float speed;
        static CVector start;
        static CVector finish;
        static bool enable = false;

        Events::drawingEvent += [] {
            CVehicle *vehicle = FindPlayerVehicle();
            if (vehicle) {
                if ((vehicle->m_fBreakPedal > 0.0f) && (enable == false)) {
                    start = vehicle->m_matrix.pos;
                    speed = vehicle->m_vecMoveSpeed.Magnitude() * 50.0f;
                    enable = true;
                }
                if (CTheScripts::IsVehicleStopped(vehicle) && enable) {
                    enable = false;
                    finish = vehicle->m_matrix.pos;
                    dist = sqrt( ((start.x - finish.x)*(start.x - finish.x)) + ((start.y - finish.y)*(start.y - finish.y)) );
                    static char message[256];
                    snprintf(message, 256, "speed = %.2f; dist = %.2f", speed, dist);
                    CMessages::AddMessageJumpQ(message, 5000, false);
                }
            }
        };
    }
} myPlugin;


//#include "plugin_III.h"
//#include "extensions\ScriptCommands.h"
//#include "game_III\common.h"
//#include "game_III\CMessages.h"
//
//using namespace plugin;
//using namespace plugin::test;
//
//class Gta3CarAngle {
//public:
//    Gta3CarAngle() {
//        Events::gameProcessEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle();
//
//            if (!vehicle)
//                return;
//
//            float angle = atan2(-vehicle->m_matrix.up.x, vehicle->m_matrix.up.y) * 57.295776f;
//            if (angle < 0.0f)
//                angle += 360.0f;
//            if (angle > 360.0f)
//                angle -= 360.0f;
//
//            float angleCommand = 0.0f;
//            ScriptCommand<GET_CAR_HEADING>(CPools::GetVehicleRef(vehicle), &angleCommand);
//
//            static char message[256];
//            snprintf(message, 256, "Angle: %g Angle (command): %g", angle, angleCommand);
//
//            CMessages::AddMessageJumpQ(message, 200, false);
//        };
//    }
//} gta3CarAngle;


//#include "plugin_III.h"
//#include "game_III\common.h"
//#include "game_III\CFont.h"
//#include "extensions\KeyCheck.h"
//#include "game_III\CGeneral.h"
//#include "game_III\CModelInfo.h"
//#include "game_III\CMessages.h"
//#include "game_III\CTheScripts.h"
//#include "game_III\CTrafficLights.h"
//
//using namespace plugin;
//
//class MyPlugin {
//public:
//
//    MyPlugin() {
//
//        Events::drawingEvent += [] {
//            //CVector &fCamPosX = *(CVector *)0x6FAD2C;
//            static unsigned int randomInt = 1;
//            static unsigned int randomInt2 = 1;
//            static float randomFloat;
//            
//            CVehicle *vehicle = FindPlayerVehicle();
//            if (vehicle) {
//                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
//
//                CFont::SetScale(0.5f, 1.0f);
//                CFont::SetColor(CRGBA(255, 255, 255, 255));
//                CFont::SetJustifyOn();
//                CFont::SetFontStyle(0);
//                CFont::SetPropOn();
//                CFont::SetWrapx(600.0f);
//                wchar_t text[32];
//                swprintf(text, L"MisFlag %d", *&CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag]);
//                CFont::PrintString(10.0f, 10.0f, text);
//                swprintf(text, L"randomInt2 %d", randomInt2);
//                CFont::PrintString(10.0f, 30.0f, text);
//                
//                swprintf(text, L"car %d", CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]->m_nType);
//                CFont::PrintString(10.0f, 50.0f, text);
//                swprintf(text, L"randomInt %d", randomInt);
//                CFont::PrintString(10.0f, 70.0f, text);
//                
//                swprintf(text, L"stopBridge %d", CTrafficLights::ShouldCarStopForBridge(vehicle));
//                CFont::PrintString(10.0f, 90.0f, text);
//                swprintf(text, L"svetCar1 %d", CTrafficLights::LightForCars1());
//                CFont::PrintString(10.0f, 110.0f, text);
//                swprintf(text, L"svetCar2 %d", CTrafficLights::LightForCars2());
//                CFont::PrintString(10.0f, 130.0f, text);
//                swprintf(text, L"svetofor %d", CTrafficLights::FindTrafficLightType(vehicle));
//                CFont::PrintString(10.0f, 150.0f, text);
//
//                swprintf(text, L"stopForL %d", CTrafficLights::ShouldCarStopForLight(vehicle, false));
//                CFont::PrintString(10.0f, 170.0f, text);
//                swprintf(text, L"Flags %d", automobile->m_nAutomobileFlags);
//                CFont::PrintString(10.0f, 190.0f, text);
//
//                swprintf(text, L"vehFlag %d", vehicle->m_nVehicleFlags);
//                CFont::PrintString(10.0f, 210.0f, text);
//
//                swprintf(text, L"field_4DB %d", automobile->field_4DB);
//                CFont::PrintString(10.0f, 230.0f, text);
//
//                // inline ModelInfo
//                //static inline bool IsCarModel(int index) {
//                //    return (ms_modelInfoPtrs[index] && ms_modelInfoPtrs[index]->m_nType == 5 && reinterpret_cast<CVehicleModelInfo *>(ms_modelInfoPtrs[index])->m_nVehicleType == 0);
//                //}
//
//                //// return -1 if model is not a vehicle model otherwise returns vehicle model type
//                //// 0 - car, 1 - boat, 2 - train, 3 - heli, 4 - plane
//                //static inline int IsVehicleModelType(int index) {
//                //    int result;
//                //    if (index < 5500 && ms_modelInfoPtrs[index]) {
//                //        if (ms_modelInfoPtrs[index]->m_nType == 5)
//                //            result = reinterpret_cast<CVehicleModelInfo *>(ms_modelInfoPtrs[index])->m_nVehicleType;
//                //        else
//                //            result = -1;
//                //    }
//                //    else
//                //        result = -1;
//                //    return result;
//                //}
//
//
//                /*swprintf(text, L"x %.2f", fCamPosX.x);
//                CFont::PrintString(10.0f, 50.0f, text);
//                swprintf(text, L"y %.2f", fCamPosX.y);
//                CFont::PrintString(10.0f, 70.0f, text);
//                swprintf(text, L"z %.2f", fCamPosX.z);
//                CFont::PrintString(10.0f, 90.0f, text);*/
//
//                /*float angLF = CGeneral::GetATanOfXY(automobile->m_aCarNodes[CAR_WHEEL_LF]->modelling.right.x, automobile->m_aCarNodes[CAR_WHEEL_LF]->modelling.right.y) - 3.141593f;
//                float angLB = CGeneral::GetATanOfXY(automobile->m_aCarNodes[CAR_WHEEL_LB]->modelling.right.x, automobile->m_aCarNodes[CAR_WHEEL_LB]->modelling.right.y) - 3.141593f;
//                float angRF = CGeneral::GetATanOfXY(-automobile->m_aCarNodes[CAR_WHEEL_RF]->modelling.right.x, -automobile->m_aCarNodes[CAR_WHEEL_RF]->modelling.right.y) - 3.141593f;
//                float angRB = CGeneral::GetATanOfXY(-automobile->m_aCarNodes[CAR_WHEEL_RB]->modelling.right.x, -automobile->m_aCarNodes[CAR_WHEEL_RB]->modelling.right.y) - 3.141593f;
//
//                swprintf(text, L"LF %.2f", angLF);
//                CFont::PrintString(10.0f, 30.0f, text);
//                swprintf(text, L"LB %.2f", angLB);
//                CFont::PrintString(10.0f, 50.0f, text);
//                swprintf(text, L"RF %.2f", angRF);
//                CFont::PrintString(10.0f, 70.0f, text);
//                swprintf(text, L"RB %.2f", angRB);
//                CFont::PrintString(10.0f, 90.0f, text);*/
//
//                KeyCheck::Update();
//                if (KeyCheck::CheckWithDelay(48, 1000))
//                    randomInt2 = CGeneral::GetRandomNumber();
//                    //*&CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag] = 1;
//                if (KeyCheck::CheckWithDelay(49, 1000))
//                    vehicle->BurstTyre(1);
//                    //*&CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag] = 0;
//                if (KeyCheck::CheckWithDelay(51, 1000)) {
//                    randomFloat = CGeneral::GetRandomNumberInRange(5.0f, 10.0f);
//                    static char message[256];
//                    snprintf(message, 256, "float %.2f", randomFloat);
//                    CMessages::AddMessageJumpQ(message, 2000, false);
//                }
//                    
//                if (KeyCheck::CheckWithDelay(50, 1000)) {
//                    randomInt = CGeneral::GetRandomNumberInRange(4, 12);
//                }
//                    
//
//                if (KeyCheck::CheckWithDelay(52, 1000)) {
//                    //CVector offset = { 0.0f, 8.0f, 0.0f };
//                    vehicle->SetPosition(vehicle->TransformFromObjectSpace(CVector(0.0f, 8.0f, 0.0f)));
//                }
//                    
//
//                //if (KeyCheck::CheckWithDelay(52, 1000)) {
//                //    CVehicle *car = nullptr;
//                //    car = new CAutomobile(vehicle->m_nModelIndex, 0);
//                //    if (car) {
//                //        CMessages::AddMessageJumpQ(L"yes", 1000, 0);
//                //        CVector offset = { 0.0f, 8.0f, 0.0f };
//                //        CVector posn = automobile->m_matrix * offset;
//                //        //float Z = CWorld::FindGroundZForCoord(posn.x, posn.y);
//                //        car->m_matrix.pos = posn;
//                //        //car->m_matrix.pos.y = posn.y;
//                //        //car->m_matrix.pos.z = Z;
//                //        //player->GetPosition();
//                //        //vehicle->SetHeading(angleZ);
//                //        //vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
//                //        CWorld::Add(car);
//                //        //CTheScripts::ClearSpaceForMissionEntity(position, vehicle);
//                //        reinterpret_cast<CAutomobile *>(car)->PlaceOnRoadProperly();
//                //    }
//                //}
//            }
//        };
//    }
//} myPlugin;


//#include "plugin_III.h"
//#include "game_III\common.h"
//#include "extensions\KeyCheck.h"
//#include "game_III\CClumpModelInfo.h"
//#include "game_III\CFont.h"
//#include "extensions\ScriptCommands.h"
//
//using namespace plugin;
//using namespace plugin::test;
//
//class MyPlugin {
//public:
//
//    MyPlugin() {
//        static float angleG, angleS, ang, ang2, angleZ;
//
//
//        Events::processScriptsEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle();
//            if (vehicle) {
//                angleZ = 0.0f;
//                ScriptCommand<GET_CAR_HEADING>(CPools::GetVehicleRef(vehicle), &angleZ);
//            }
//        };
//
//
//        Events::drawingEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle();
//            if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
//                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
//                
//                float ang = atan2(vehicle->m_matrix.up.y, -vehicle->m_matrix.up.x);
//                float ang2 = 180.0f * ang * 0.31830987f;
//                if (ang2 >= 0.0f)
//                    ang2 = ang2 + 360.0f;
//                if (ang2 > 360.0f)
//                    ang2 = ang2 - 360.0f;
//
////                
//                CFont::SetScale(0.5f, 1.0f);
//                CFont::SetColor(CRGBA(255, 255, 255, 255));
//                CFont::SetJustifyOn();
//                CFont::SetFontStyle(0);
//                CFont::SetPropOn();
//                CFont::SetWrapx(600.0f);
//                wchar_t text[32];
//                swprintf(text, L"speed %.2f", vehicle->m_vecMoveSpeed.Magnitude() * 50.0f);
//                CFont::PrintString(10.0f, 10.0f, text);
//                swprintf(text, L"angleZ %.0f", angleZ);
//                CFont::PrintString(10.0f, 30.0f, text);
//                swprintf(text, L"GetRotation %.2f", ang2);
//                CFont::PrintString(10.0f, 50.0f, text);
//
//                //swprintf(text, L"GetHeading %.2f", vehicle->GetHeading());
//                //CFont::PrintString(10.0f, 80.0f, text);
//                
//                KeyCheck::Update();
//                //if (KeyCheck::CheckWithDelay(50, 1000)) 
//                    //ScriptCommand<SET_CAR_HEADING>(CPools::GetVehicleRef(vehicle), 85.0f);
//                    //automobile->SetModelIndex(automobile->m_nModelIndex);
//
//                if (KeyCheck::CheckWithDelay(51, 1000)) {
//                    //CVector offset = { 0.0f, 8.0f, 0.0f };
//                    //CVector posn = automobile->m_matrix * offset;
//                    //automobile->SetPosition(posn);
//                    //automobile->SetOrientation(0.0f, 0.0f, (angleS * 0.017453292f));
//                    
//                    ang2 *= 0.0055555557f * 3.1415927f;
//                    automobile->SetHeading(ang2);
//                }
//                    
//
//                if (KeyCheck::CheckWithDelay(55, 1000)) {
//                    RwFrame *m_pCement = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "cement");
//                    if (m_pCement)
//                        automobile->SetComponentVisibility(m_pCement, 1);
//                    if (automobile->m_aCarNodes[CAR_BONNET])
//                        automobile->SetComponentVisibility(automobile->m_aCarNodes[CAR_BONNET], 0);
//                }
//
//                if (KeyCheck::CheckWithDelay(56, 1000)) {
//                    RwFrame *m_pCement = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "cement");
//                    if (m_pCement)
//                        automobile->SetComponentVisibility(m_pCement, 0);
//                    if (automobile->m_aCarNodes[CAR_BONNET])
//                        automobile->SetComponentVisibility(automobile->m_aCarNodes[CAR_BONNET], 1);
//                }
//
//                if (KeyCheck::CheckWithDelay(57, 1000)) {
//                    RwFrame *m_pCement = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "cement");
//                    if (m_pCement)
//                        automobile->SetComponentVisibility(m_pCement, 2);
//                    if (automobile->m_aCarNodes[CAR_BONNET])
//                        automobile->SetComponentVisibility(automobile->m_aCarNodes[CAR_BONNET], 2);
//                }
//
//                //if (KeyCheck::CheckWithDelay(52, 1000)) {
//                //    for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                //        CVehicle *trailer = CPools::ms_pVehiclePool->GetAt(i);
//                //        if (trailer && !trailer->m_pDriver && (trailer->m_nVehicleFlags & 0x10) && trailer->m_fHealth > 0.1f && trailer->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
//                //            CVector offset = { 0.0f, -8.0f, 0.0f };
//                //            CVector posn = automobile->m_matrix * offset;
//                //            trailer->SetPosition(posn);
//                //            static float angleGet, angleSet;
//                //            //angleGet = automobile->GetRotation() * 57.295776f;
//                //            //angleGet = automobile->GetHeading();
//                //            angleGet = automobile->GetRotation();
//                //            angleSet = angleGet;
//                //            /*if (angleGet < 0.0f)
//                //                angleSet = angleSet + 360.0f;
//                //            if (angleSet > 360.0f)
//                //                angleSet = angleSet - 360.0f;*/
//
//                //            //angleSet *= 0.017453292f;
//
//
//                //            //trailer->SetOrientation(0.0f, 0.0f, angleSet);
//                //            //trailer->SetHeading(angleSet);
//                //            
//                //            //trailer->Teleport(posn);
//                //            //trailer->SetHeading(angle);
//                //            //angle += 5.0f;
//                //            //trailer->m_matrix.SetTranslate(posn.x, posn.y, posn.z);
//                //            //trailer->AttachToRwObject(&vehicle->m_pRwClump->object);
//                //            
//                //            break;
//                //        }
//                //    }
//                //}
//
//            }
//        };
//    }
//} myPlugin;


//#include "plugin_III.h"
//#include "game_III\common.h"
//#include "extensions\KeyCheck.h"
//#include "game_III\CFont.h"
//#include "game_III\CTimer.h"
//
//using namespace plugin;
//
//class MyPlugin {
//public:
//    
//    MyPlugin() {
//                    
//                //CPed *player = FindPlayerPed();
//                //if (player) 
//               
//                Events::drawingEvent += [] {
//                    CVehicle *veh = FindPlayerVehicle();
//                    if (veh && veh->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
//                        CAutomobile *automobile = reinterpret_cast<CAutomobile *>(veh);
//                        CFont::SetScale(0.5f, 1.0f);
//                        CFont::SetColor(CRGBA(255, 255, 255, 255));
//                        CFont::SetJustifyOn();
//                        CFont::SetFontStyle(0);
//                        CFont::SetPropOn();
//                        CFont::SetWrapx(600.0f);
//                        wchar_t text[32];
//                        swprintf(text, L"Extra[0] %d", veh->m_nExtra[0]);
//                        CFont::PrintString(10.0f, 10.0f, text);
//                        swprintf(text, L"Extra[1] %d", veh->m_nExtra[1]);
//                        CFont::PrintString(10.0f, 30.0f, text);
//                        
//                        KeyCheck::Update();
//                        if (KeyCheck::CheckWithDelay(52, 1000)) {
//                            automobile->SetModelIndex(automobile->m_nModelIndex);
//                        }
//                    }
//                };
//                
//                
//                /*int currentWather = wather->NewWeatherType;
//                switch (currentWather) {
//                case 0: swprintf(text, L"weather %hs", "SUNNY");  break;
//                case 1: swprintf(text, L"weather %hs", "CLOUDY"); break;
//                case 2: swprintf(text, L"weather %hs", "RAINY");  break;
//                case 3: swprintf(text, L"weather %hs", "FOGGY");  break;
//                }*/
//                //CFont::PrintString(10.0f, 10.0f, text);
//                
//
//                /*swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[0]);
//                CFont::PrintString(10.0f, 110.0f, text);
//                swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[1]);
//                CFont::PrintString(10.0f, 140.0f, text);
//                swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[2]);
//                CFont::PrintString(10.0f, 170.0f, text);
//                swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[3]);
//                CFont::PrintString(10.0f, 200.0f, text);*/
//
//    }
//} myPlugin;



//if (car->vehicle.m_nSirenOrAlarm)
//{
//    v429.x = 1053609165;
//    v429.y = 1058642330;
//    v429.z = 1050253722;
//    operator*((int)&v428, (int)&car->vehicle.physical.entity.m_placement.matrix, (int)&v429);
//    CVector::Copy(&v261, &v428);
//    if (CTimer::m_snTimeInMilliseconds & 0x100
//        && (v154 = *(float *)&car->vehicle.physical.entity.m_placement.matrix.matrix.pos.z - *(float *)&fCamPosX.z,
//            v155 = *(float *)&car->vehicle.physical.entity.m_placement.matrix.matrix.pos.y - *(float *)&fCamPosX.y,
//            v156 = *(float *)&car->vehicle.physical.entity.m_placement.matrix.matrix.pos.x - *(float *)&fCamPosX.x,
//            CVector::Set(&v430, v156, v155, v154),  
//              
//            *(float *)(v243 + 4) * *(float *)&v430.y 
//            + *(float *)v243 * *(float *)&v430.x
//            + *(float *)(v243 + 8) * *(float *)&v430.z >= flt_6004F8))
//    {
//        CCoronas::RegisterCorona(
//            (int)&car->vehicle.physical.entity.m_placement.matrix.matrix.top.x + 1,
//            0,
//            0,
//            -1,
//            -1,
//            (int)&v261,
//            flt_600520,
//            flt_6005B4,
//            0,
//            0,
//            0,
//            0,
//            0,
//            flt_6004F8);
//    }
//}
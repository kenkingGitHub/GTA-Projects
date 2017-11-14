#include "plugin_III.h"
#include "game_III\common.h"
#include "extensions\KeyCheck.h"
#include "game_III\CClumpModelInfo.h"
#include "game_III\CFont.h"

using namespace plugin;

class MyPlugin {
public:

    MyPlugin() {
        Events::drawingEvent += [] {
            CVehicle *vehicle = FindPlayerVehicle();
            if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
                
                CFont::SetScale(0.5f, 1.0f);
                CFont::SetColor(CRGBA(255, 255, 255, 255));
                CFont::SetJustifyOn();
                CFont::SetFontStyle(0);
                CFont::SetPropOn();
                CFont::SetWrapx(600.0f);
                wchar_t text[32];
                swprintf(text, L"speed %.2f", vehicle->m_vecMoveSpeed.Magnitude() * 50.0f);
                CFont::PrintString(10.0f, 10.0f, text);
                //swprintf(text, L"Extra[1] %d", veh->m_nExtra[1]);
                //CFont::PrintString(10.0f, 30.0f, text);
                
                KeyCheck::Update();
                if (KeyCheck::CheckWithDelay(53, 1000)) 
                    automobile->SetModelIndex(automobile->m_nModelIndex);
                   
                if (KeyCheck::CheckWithDelay(55, 1000)) {
                    RwFrame *m_pCement = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "cement");
                    if (m_pCement)
                        automobile->SetComponentVisibility(m_pCement, 1);
                    if (automobile->m_aCarNodes[CAR_BONNET])
                        automobile->SetComponentVisibility(automobile->m_aCarNodes[CAR_BONNET], 0);
                }

                if (KeyCheck::CheckWithDelay(56, 1000)) {
                    RwFrame *m_pCement = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "cement");
                    if (m_pCement)
                        automobile->SetComponentVisibility(m_pCement, 0);
                    if (automobile->m_aCarNodes[CAR_BONNET])
                        automobile->SetComponentVisibility(automobile->m_aCarNodes[CAR_BONNET], 1);
                }

                if (KeyCheck::CheckWithDelay(57, 1000)) {
                    RwFrame *m_pCement = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "cement");
                    if (m_pCement)
                        automobile->SetComponentVisibility(m_pCement, 2);
                    if (automobile->m_aCarNodes[CAR_BONNET])
                        automobile->SetComponentVisibility(automobile->m_aCarNodes[CAR_BONNET], 2);
                }

                if (KeyCheck::CheckWithDelay(52, 1000)) {
                    for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                        CVehicle *trailer = CPools::ms_pVehiclePool->GetAt(i);
                        if (trailer && !trailer->m_pDriver && (trailer->m_nVehicleFlags & 0x10) && trailer->m_fHealth > 0.1f && trailer->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                            CVector offset = { 0.0f, 8.0f, 0.0f };
                            CVector posn = automobile->m_matrix * offset;
                            //static float angle;
                            //trailer->Teleport(posn);
                            //trailer->SetHeading(angle);
                            //angle += 5.0f;
                            //trailer->m_matrix.SetTranslate(posn.x, posn.y, posn.z);
                            //trailer->AttachToRwObject(&vehicle->m_pRwClump->object);
                            
                            break;
                        }
                    }
                }

            }
        };
    }
} myPlugin;


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
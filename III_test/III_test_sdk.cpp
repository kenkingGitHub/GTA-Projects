#include "plugin_III.h"
#include "game_III\common.h"
#include "extensions\KeyCheck.h"
#include "game_III\CMessages.h"
#include "game_III\CFont.h"
#include "game_III\CSprite2d.h"
#include "game_III\CTxdStore.h"
#include "game_III\CModelInfo.h"
#include "game_III\CCoronas.h"
#include "game_III\CPointLights.h"
#include "game_III\CTimer.h"

using namespace plugin;

class MyPlugin {
public:
    MyPlugin() {

        //CModelInfo::AddVehicleModel(4000);
        // 0x546B99;
        //static CSprite2d sprite;

        //Events::initRwEvent += [] {
        //    int txd = CTxdStore::AddTxdSlot("mytxd");
        //    CTxdStore::LoadTxd(txd, "MODELS\\MYTXD.TXD");
        //    CTxdStore::AddRef(txd);
        //    CTxdStore::PushCurrentTxd();
        //    CTxdStore::SetCurrentTxd(txd);
        //    sprite.SetTexture("up", "upa");
        //    CTxdStore::PopCurrentTxd();
        //    //pMyTextureArrow = sprite.m_pTexture;
        //};

        //Events::onMenuDrawingEvent += [] {
        //    //if (!sprite.m_pTexture) // load texture only once
        //        //sprite.m_pTexture = RwD3D9DDSTextureRead(PLUGIN_PATH("image"), 0); 
        //    //sprite.Draw(CRect(100.0f, 100.0f, 200.0f, 200.0f), CRGBA(255, 255, 255, 150)); 
        //    sprite.SetRenderState();
        //    sprite.Draw(20.0, 20.0, 50.0, 50.0, CRGBA(255, 255, 255, 255));
        //};
        //
        //Events::shutdownRwEvent += [] {
        //    sprite.Delete();
        //    CTxdStore::RemoveTxdSlot(CTxdStore::FindTxdSlot("mytxd"));
        //};

        //Events::onMenuDrawingEvent += [] {
        //    // вывод текста
        //    CFont::SetScale(0.5f, 1.0f);
        //    CFont::SetColor(CRGBA(238, 173, 53, 255));
        //    CFont::SetJustifyOn();
        //    CFont::SetFontStyle(0);
        //    CFont::SetPropOn();
        //    CFont::SetWrapx(600.0f);
        //    wchar_t text[64];
        //    swprintf(text, L"plugin Additional Components by kenking version %.1f", 0.5f);
        //    CFont::PrintString(25.0f, 25.0f, text);
        //};

        //Events::vehicleRenderEvent.before += [](CVehicle *vehicle) {
            //if (*(bool *)0x8F5AE9) {
            //    // вывод текста
            //    CFont::SetScale(0.5f, 1.0f);
            //    CFont::SetColor(CRGBA(255, 255, 255, 255));
            //    CFont::SetJustifyOn();
            //    CFont::SetFontStyle(0);
            //    CFont::SetPropOn();
            //    CFont::SetWrapx(600.0f);
            //    wchar_t text[32];
            //    swprintf(text, L"test %hs", "777");
            //    CFont::PrintString(100.0f, 100.0f, text);
            //}

            //sprite.Draw(CRect(50.0f, 50.0f, 100.0f, 100.0f), CRGBA(255, 255, 255, 150));
            //sprite.Draw(20.0, 20.0, 50.0, 50.0, CRGBA(255, 255, 255, 255));

           
                //CVector posn = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[veh->m_nModelIndex])->m_avDummyPos[9];
                //RwFrame *m_pTest = CClumpModelInfo::GetFrameFromName(veh->m_pRwClump, "indicators_front");
                //if (m_pTest) {
                //    RwMatrix* matr = RwFrameGetLTM(m_pTest);
                //    //CMessages::AddMessageJumpQ(L"yes", 1000, 0);
                //    CVector posnOut, posnOut2;
                //    posnOut.x = matr->pos.x;
                //    posnOut2.x = matr->pos.x + 0.5f;
                //    posnOut.y = posnOut2.y = matr->pos.y;
                //    posnOut.z = posnOut2.z = matr->pos.z;
                //    if (CTimer::m_snTimeInMilliseconds & 0x200) {
                //        CCoronas::RegisterCorona(reinterpret_cast<unsigned int>(veh) + 17, 128, 128, 0, 255, posnOut, 0.3f, 50.0f, 1, 0, 1, 0, 0, 0.0f);
                //        CCoronas::RegisterCorona(reinterpret_cast<unsigned int>(veh) + 18, 128, 128, 0, 255, posnOut2, 0.3f, 50.0f, 1, 0, 1, 0, 0, 0.0f);
                //        CVector two = { 0.0f, 0.0f, 0.0f };
                //        CPointLights::AddLight(0, posnOut, two, 3.0f, 1.0f, 1.0f, 0.5f, 0, 1);
                //        CPointLights::AddLight(0, posnOut2, two, 3.0f, 1.0f, 1.0f, 0.5f, 0, 1);
                //    }
                //    else {
                //        CCoronas::UpdateCoronaCoors(reinterpret_cast<unsigned int>(veh) + 17, posnOut, 50.0f, 0.0f);
                //        CCoronas::UpdateCoronaCoors(reinterpret_cast<unsigned int>(veh) + 18, posnOut2, 50.0f, 0.0f);
                //    }
                //}

                /*if (veh->IsComponentPresent(15)) {
                    CVector posnOut;
                    veh->GetComponentWorldPosition(15, posnOut);
                    if (CTimer::m_snTimeInMilliseconds & 0x200) {
                        CCoronas::RegisterCorona(reinterpret_cast<unsigned int>(veh) + 18, 128, 128, 0, 255, posnOut, 0.3f, 50.0f, 1, 0, 1, 0, 0, 0.0f);
                        CVector twoR = { 0.0f, 0.0f, 0.0f };
                        CPointLights::AddLight(0, posnOut, twoR, 3.0f, 1.0f, 1.0f, 0.5f, 0, 1);
                    }
                    else {
                        CCoronas::UpdateCoronaCoors(reinterpret_cast<unsigned int>(veh) + 18, posnOut, 50.0f, 0.0f);
                    }
                }*/
                ///*if(veh->IsComponentPresent(11)) {
                //    CVector posnOut;
                //    veh->GetComponentWorldPosition(11, posnOut);
                //    if (CTimer::m_snTimeInMilliseconds & 0x200) {
                //        CCoronas::RegisterCorona(reinterpret_cast<unsigned int>(veh) + 19, 128, 128, 0, 255, posnOut, 0.3f, 50.0f, 1, 0, 1, 0, 0, 0.0f);
                //        CVector twoR = { 0.0f, 0.0f, 0.0f };
                //        CPointLights::AddLight(0, posnOut, twoR, 3.0f, 1.0f, 1.0f, 0.5f, 0, 1);
                //    }
                //    else {
                //        CCoronas::UpdateCoronaCoors(reinterpret_cast<unsigned int>(veh) + 19, posnOut, 50.0f, 0.0f);
                //    }
                //}*/


                    //CVector posR = veh->m_matrix.pos;
                    //posR.x += 2.0f;
                    //CVector posL = veh->m_matrix.pos;
                    //posL.x -= 2.0f;
                    //RwV3d offset;
                    //offset.x = veh->m_matrix.pos.x +2.0f;
                    //offset.y = veh->m_matrix.pos.y;
                    //offset.z = veh->m_matrix.pos.z;
                    
                    //RwV3dTransformPoints(&offset, &offset, 1, (*CPools::VehiclePoolGetStruct(CPools::ms_pVehiclePool, CPools::GetVehicleRef(veh)) +4));

                    //posR.x = offset.x;
                    //posR.y = offset.y;
                    //posR.z = offset.z;

                    
                    
                    
                        
                

               /* for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                    CVehicle *veh = CPools::ms_pVehiclePool->GetAt(i);
                    if (veh && !veh->m_pDriver && veh->m_fHealth > 0.1f && veh->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                        if ((veh->m_matrix.pos.x - veh->m_matrix.pos.x) < 6.0f) {
                            float posX = veh->m_matrix.pos.x;
                            veh->m_matrix.pos.x = posX - 3.0f;
                            veh->m_matrix.pos.y = veh->m_matrix.pos.y - 3.0f;
                            veh->m_matrix.pos.z = veh->m_matrix.pos.z;
                            veh->m_matrix.at.z = veh->m_matrix.at.z;
                        }
                    }
                }*/
                    
                //CPed *player = FindPlayerPed();
                //if (player) 
                // вывод текста
               
                Events::drawingEvent += [] {
                    CVehicle *veh = FindPlayerVehicle();
                    if (veh && veh->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                        CAutomobile *playerAutomobile = reinterpret_cast<CAutomobile *>(veh);
                        CFont::SetScale(0.5f, 1.0f);
                        CFont::SetColor(CRGBA(255, 255, 255, 255));
                        CFont::SetJustifyOn();
                        CFont::SetFontStyle(0);
                        CFont::SetPropOn();
                        CFont::SetWrapx(600.0f);
                        wchar_t text[32];
                        swprintf(text, L"Extra 0 %d", veh->m_nExtra[0]);
                        CFont::PrintString(10.0f, 10.0f, text);
                        swprintf(text, L"Extra 1 %d", veh->m_nExtra[1]);
                        CFont::PrintString(10.0f, 30.0f, text);

                        /*int lf = playerAutomobile->m_carDamage.GetPanelStatus(WING_FRONT_LEFT);
                        int lr = playerAutomobile->m_carDamage.GetPanelStatus(WING_REAR_LEFT);
                        int rf = playerAutomobile->m_carDamage.GetPanelStatus(WING_FRONT_RIGHT);
                        int rr = playerAutomobile->m_carDamage.GetPanelStatus(WING_REAR_RIGHT);
                        swprintf(text, L"lf %d", lf);
                        CFont::PrintString(10.0f, 30.0f, text);
                        swprintf(text, L"lr %d", lr);
                        CFont::PrintString(10.0f, 50.0f, text);
                        swprintf(text, L"rf %d", rf);
                        CFont::PrintString(10.0f, 70.0f, text);
                        swprintf(text, L"rr %d", rr);
                        CFont::PrintString(10.0f, 90.0f, text);

                        if (veh->IsComponentPresent(13))
                            swprintf(text, L"WING_LF yes");
                        else
                            swprintf(text, L"WING_LF no");
                        CFont::PrintString(100.0f, 30.0f, text);
                        if (veh->IsComponentPresent(14))
                            swprintf(text, L"WING_LR yes");
                        else
                            swprintf(text, L"WING_LR no");
                        CFont::PrintString(100.0f, 50.0f, text);
                        if (veh->IsComponentPresent(9))
                            swprintf(text, L"WING_RF yes");
                        else
                            swprintf(text, L"WING_RF no");
                        CFont::PrintString(100.0f, 70.0f, text);
                        if (veh->IsComponentPresent(10))
                            swprintf(text, L"WING_RR yes");
                        else
                            swprintf(text, L"WING_RR no");
                        CFont::PrintString(100.0f, 90.0f, text);*/
                            
                    }
                };
                
                
                /*int currentWather = wather->NewWeatherType;
                switch (currentWather) {
                case 0: swprintf(text, L"weather %hs", "SUNNY");  break;
                case 1: swprintf(text, L"weather %hs", "CLOUDY"); break;
                case 2: swprintf(text, L"weather %hs", "RAINY");  break;
                case 3: swprintf(text, L"weather %hs", "FOGGY");  break;
                }*/
                //CFont::PrintString(10.0f, 10.0f, text);
                
                
                
                //swprintf(text, L"steer angle %.2f", veh->m_fSteerAngle);
                /*swprintf(text, L"dumperAngle %.2f", vehComps.Get(veh).dumperAngle);
                CFont::PrintString(10.0f, 50.0f, text);
                swprintf(text, L"manholePos %.2f", vehComps.Get(veh).manholePos);
                CFont::PrintString(10.0f, 80.0f, text);*/
                //KeyCheck::Update();
                //if (KeyCheck::CheckWithDelay(0x6E, 1000))
                //    //veh->BurstTyre(0);
                //    veh->m_matrix.SetRotateZOnly(1.57f);

                /*swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[0]);
                CFont::PrintString(10.0f, 110.0f, text);
                swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[1]);
                CFont::PrintString(10.0f, 140.0f, text);
                swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[2]);
                CFont::PrintString(10.0f, 170.0f, text);
                swprintf(text, L"WheelRot %d", playerAutomobile->m_nTireFriction[3]);
                CFont::PrintString(10.0f, 200.0f, text);*/

                /*swprintf(text, L"LightStatus %d", playerAutomobile->m_carDamage.GetLightStatus(LIGHT_FRONT_LEFT));
                CFont::PrintString(10.0f, 140.0f, text);
                swprintf(text, L"LightStatus %d", playerAutomobile->m_carDamage.GetLightStatus(LIGHT_FRONT_RIGHT));
                CFont::PrintString(10.0f, 170.0f, text);
                swprintf(text, L"LightStatus %d", playerAutomobile->m_carDamage.GetLightStatus(LIGHT_REAR_RIGHT));
                CFont::PrintString(10.0f, 200.0f, text);
                swprintf(text, L"LightStatus %d", playerAutomobile->m_carDamage.GetLightStatus(LIGHT_REAR_LEFT));
                CFont::PrintString(10.0f, 230.0f, text);*/
            

    }
} myPlugin;



//#include <plugin.h>
//#include "game_sa\common.h"
//#include "game_sa\CTimer.h"
//#include "game_sa\CModelInfo.h"
//#include "game_sa\CVehicle.h"
//#include "game_sa\tHandlingData.h"
//#include "game_sa\CCoronas.h"
//
//using namespace plugin;
//
//class Diesel {
//public:
//    Diesel() {
//        static bool m_currentState = true;
//        static unsigned int m_nLastTimeWhenAnyActionWasEnabled = 0;
//        static tHandlingData *hanlData;
//
//        Events::gameProcessEvent += [] {
//            CVehicle *vehicle = FindPlayerVehicle(-1, false);
//            if (vehicle && vehicle->m_dwVehicleClass == VEHICLE_AUTOMOBILE && vehicle->IsStopped() && KeyPressed(87) && m_currentState) {
//                CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[vehicle->m_wModelIndex]);
//                hanlData = vehicle->m_pHandlingData;
//                if (vehModel->m_nClass == 4 && hanlData->m_transmissionData.m_nEngineType == 68) {
//                    m_currentState = false;
//                    m_nLastTimeWhenAnyActionWasEnabled = CTimer::m_snTimeInMilliseconds;
//                }
//            }
//            else if (!m_currentState) {
//                if (CTimer::m_snTimeInMilliseconds < (m_nLastTimeWhenAnyActionWasEnabled + 2000)) {
//                    CVector posn = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[vehicle->m_wModelIndex])->m_pVehicleStruct->m_avDummyPosn[6];
//                    CCoronas::RegisterCorona(reinterpret_cast<unsigned int>(vehicle) + 50 + 6 + 0, vehicle, 255, 128, 0, 255, posn, 0.3f, 150.0f, CORONATYPE_SHINYSTAR, 0, false, false, 0, 0.0f, false, 0.5f, 0, 50.0f, false, true);
//                    if (hanlData->m_bDoubleExhaust) {
//                        posn.x *= -1.0f;
//                        CCoronas::RegisterCorona(reinterpret_cast<unsigned int>(vehicle) + 50 + 6 + 1, vehicle, 255, 128, 0, 255, posn, 0.3f, 150.0f, CORONATYPE_SHINYSTAR, 0, false, false, 0, 0.0f, false, 0.5f, 0, 50.0f, false, true);
//                    }
//                }
//                else
//                    m_currentState = true;
//            }
//        };
//    }
//} example;


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
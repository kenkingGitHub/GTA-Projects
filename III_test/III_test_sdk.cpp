#include "plugin_III.h"
#include "game_III\common.h"
#include "extensions\KeyCheck.h"
#include "game_III\CMessages.h"
#include "game_III\CFont.h"
#include "game_III\CSprite2d.h"
#include "game_III\CTxdStore.h"
//#include "game_III\CModelInfo.h"
#include "game_III\CCoronas.h"
#include "game_III\CPointLights.h"
#include "game_III\CTimer.h"

#define TURN_ON_OFF_DELAY 500

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

        Events::vehicleRenderEvent.before += [](CVehicle *vehicle) {
        //Events::drawingEvent += [] {
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

            CVehicle *playerVehicle = FindPlayerVehicle();
            if (playerVehicle && playerVehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                CAutomobile *playerAutomobile = reinterpret_cast<CAutomobile *>(playerVehicle);
                
                    CVector pos = playerVehicle->m_matrix.pos;
                    pos.z += 1.0f;
                    if (CTimer::m_snTimeInMilliseconds & 0x200)
                        CCoronas::RegisterCorona(reinterpret_cast<unsigned int>(playerVehicle) + 17, 128, 128, 0, 255, pos, 0.3f, 50.0f, 1, 0, 1, 0, 0, 0.0f);
                    else
                        CCoronas::UpdateCoronaCoors(reinterpret_cast<unsigned int>(playerVehicle) + 17, pos, 50.0f, 0.0f);
                
                
                
                //CVector two = {0.0f, 0.0f, 0.0f};
                //CPointLights::AddLight(0, pos, two, 10.0f, 1.0f, 1.0f, 0.5f, 0, 1);

               /* for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
                    if (vehicle && !vehicle->m_pDriver && vehicle->m_fHealth > 0.1f && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                        if ((playerVehicle->m_matrix.pos.x - vehicle->m_matrix.pos.x) < 6.0f) {
                            float posX = playerVehicle->m_matrix.pos.x;
                            vehicle->m_matrix.pos.x = posX - 3.0f;
                            vehicle->m_matrix.pos.y = playerVehicle->m_matrix.pos.y - 3.0f;
                            vehicle->m_matrix.pos.z = playerVehicle->m_matrix.pos.z;
                            vehicle->m_matrix.at.z = playerVehicle->m_matrix.at.z;
                        }
                    }
                }*/
                    
                //CPed *player = FindPlayerPed();
                //if (player) 
                // вывод текста
                /*CFont::SetScale(0.5f, 1.0f);
                CFont::SetColor(CRGBA(255, 255, 255, 255));
                CFont::SetJustifyOn();
                CFont::SetFontStyle(0);
                CFont::SetPropOn();
                CFont::SetWrapx(600.0f);
                wchar_t text[32];*/
                /*int currentWather = wather->NewWeatherType;
                switch (currentWather) {
                case 0: swprintf(text, L"weather %hs", "SUNNY");  break;
                case 1: swprintf(text, L"weather %hs", "CLOUDY"); break;
                case 2: swprintf(text, L"weather %hs", "RAINY");  break;
                case 3: swprintf(text, L"weather %hs", "FOGGY");  break;
                }*/
                //CFont::PrintString(10.0f, 10.0f, text);
                
                
                //swprintf(text, L"m_fAngle 2 %.2f", playerAutomobile->m_aDoors[2].m_fAngle);
                //CFont::PrintString(10.0f, 30.0f, text);
                //swprintf(text, L"m_fAngle 3 %.2f", playerAutomobile->m_aDoors[3].m_fAngle);
                //CFont::PrintString(10.0f, 50.0f, text);
                //swprintf(text, L"steer angle %.2f", playerVehicle->m_fSteerAngle);
                /*swprintf(text, L"dumperAngle %.2f", vehComps.Get(playerVehicle).dumperAngle);
                CFont::PrintString(10.0f, 50.0f, text);
                swprintf(text, L"manholePos %.2f", vehComps.Get(playerVehicle).manholePos);
                CFont::PrintString(10.0f, 80.0f, text);*/
                //KeyCheck::Update();
                //if (KeyCheck::CheckWithDelay(0x6E, 1000))
                //    //playerVehicle->BurstTyre(0);
                //    playerVehicle->m_matrix.SetRotateZOnly(1.57f);

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
        };

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

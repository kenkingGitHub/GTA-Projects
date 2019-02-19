#include "plugin.h"
#include "CWanted.h"
#include "CStreaming.h"
#include "CWorld.h"
#include "CVehicleModelInfo.h"

using namespace plugin;

class Test {
public:
    
    static int ChoosePoliceCarModel() {
        CPed *player = FindPlayerPed();
        if (player) {
            
            player = FindPlayerPed();
            if (CWanted::AreSwatRequired(player->m_pWanted)
                && CStreaming::ms_aInfoForModel[MODEL_ENFORCER].m_nLoadState == 1
                && CStreaming::ms_aInfoForModel[MODEL_SWAT].m_nLoadState == 1)
            {
                if (rand() & 15)
                    result = MODEL_POLICE;
                else
                    result = MODEL_ENFORCER;
            }
            else
            {
                player_1 = FindPlayerPed();
                if (CWanted::AreFbiRequired(player_1->m_pWanted)
                    && CStreaming::ms_aInfoForModel[MODEL_FBICAR].m_nLoadState == 1
                    && CStreaming::ms_aInfoForModel[MODEL_FBI].m_nLoadState == 1)
                {
                    result = MODEL_FBICAR;
                }
                else
                {
                    player_2 = FindPlayerPed();
                    if (CWanted::AreArmyRequired(player_2->m_pWanted)
                        && CStreaming::ms_aInfoForModel[MODEL_RHINO].m_nLoadState == 1
                        && CStreaming::ms_aInfoForModel[MODEL_BARRACKS].m_nLoadState == 1
                        && CStreaming::ms_aInfoForModel[MODEL_ARMY].m_nLoadState == 1)
                    {
                        if ((unsigned __int16)rand() < 16383u)
                            result = MODEL_BARRACKS;
                        else
                            result = MODEL_RHINO;
                    }
                    else
                    {
                        result = MODEL_POLICE;
                    }
                }
            }
            return result;


        }
    }

    Test() {
        patch::RedirectJump(0x5209C0, ChoosePoliceCarModel);
    }

}test;




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
//        CPed *player = FindPlayerPed();
//        if (player) {
//            KeyCheck::Update();
//            if (KeyCheck::CheckWithDelay('N', 200)) {
//                CVehicle* vehicle = GetVehicle(player);
//                if (vehicle) {
//                    CVehicleModelInfo *vehModel = reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]);
//                    unsigned char pPrim, pSec;
//                    vehModel->ChooseVehicleColour(pPrim, pSec);
//                    for (int i = 0; i < 8 && pPrim == vehicle->m_nPrimaryColor && pSec == vehicle->m_nSecondaryColor; ++i) {
//                        vehModel->ChooseVehicleColour(pPrim, pSec);
//                    }
//                    vehicle->m_nPrimaryColor = pPrim;
//                    vehicle->m_nSecondaryColor = pSec;
//                }
//            }
//        }
//    }
//
//    Test() {
//        Events::gameProcessEvent += Color;
//    }
//
//}test;



//#include "plugin.h"
//#include <string>
//#include "extensions\KeyCheck.h"
//#include "common.h"
//#include "CModelInfo.h"
////#include "CTimer.h"
//#include "CFont.h"
////#include "CSprite2d.h"
////#include "CStreaming.h"
////#include "CBoat.h"
//#include "CWorld.h"
////#include "CCarCtrl.h"
////#include "CTheScripts.h"
////#include "eVehicleModel.h"
//
//#include "CMessages.h"
//
////#include <bitset>
//
//float &ms_vehicleLod0Dist = *(float *)0x600150; // 70
//float &ms_vehicleLod1Dist = *(float *)0x600154; // 90
//float &ms_bigVehicleLod0Dist = *(float *)0x60015C; // 60
//float &ms_bigVehicleLod1Dist = *(float *)0x600160; // 150
//float &ms_cullCompsDist = *(float *)0x600138; // 20
//float &ms_vehicleFadeDist = *(float *)0x600158; // 100
//
//float &CurrentSpeed = *(float *)0x64D054;
//float &Acceleration = *(float *)0x64D04C;
//char &byte_64D03C = *(char *)0x64D03C;
//char &byte_64D044 = *(char *)0x64D044;
//float &dword_64D058 = *(float *)0x64D058;
//
//RwTexture *&gpShadowHeadLightsTex = *(RwTexture **)0x95CB98;
//void __cdecl StoreCarLightShadow(CVehicle *vehicle, int id, RwTexture *texture, CVector *posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle) {
//    ((void(__cdecl *)(CVehicle*, int, RwTexture*, CVector*, float, float, float, float, unsigned char, unsigned char, unsigned char, float))0x513A70)(vehicle, id, texture, posn, frontX, frontY, sideX, sideY, red, green, blue, maxViewAngle);
//}
//
//using namespace plugin;
//
//class MoreVehiclesSpawner {
//public:
//    /*static std::string typedBuffer;
//    static std::string errorMessage;
//    static std::string errorMessageBuffer;
//    static unsigned int errorMessageTimer;
//    static bool enabled;*/
//
//    //static void SpawnVehicle(unsigned int modelIndex, CVector position, float orientation) {
//    //    unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelIndex].m_nFlags;
//    //    CStreaming::RequestModel(modelIndex, GAME_REQUIRED);
//    //    CStreaming::LoadAllRequestedModels(false);
//    //    if (CStreaming::ms_aInfoForModel[modelIndex].m_nLoadState == LOADSTATE_LOADED) {
//    //        if (!(oldFlags & GAME_REQUIRED)) {
//    //            CStreaming::SetModelIsDeletable(modelIndex);
//    //            CStreaming::SetModelTxdIsDeletable(modelIndex);
//    //        }
//    //        CVehicle *vehicle = nullptr;
//    //        if (reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[modelIndex])->m_nVehicleType)
//    //            vehicle = new CBoat(modelIndex, 1);
//    //        else
//    //            vehicle = new CAutomobile(modelIndex, 1);
//    //        if (vehicle) {
//    //            // Размещаем транспорт в игровом мире
//    //            vehicle->SetPosition(position);
//    //            vehicle->SetOrientation(0.0f, 0.0f, orientation);
//    //            vehicle->m_nState = 4;
//    //            if (modelIndex == MODEL_RCBANDIT)
//    //                vehicle->m_nDoorLock = CARLOCK_LOCKED;
//    //            else
//    //                vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
//    //            CWorld::Add(vehicle);
//    //            CTheScripts::ClearSpaceForMissionEntity(position, vehicle);
//    //            if (vehicle->m_nVehicleClass != VEHICLE_BOAT)
//    //                reinterpret_cast<CAutomobile *>(vehicle)->PlaceOnRoadProperly();
//
//    //            //static char message[256];
//    //            //snprintf(message, 256, "Class: %d", reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[modelIndex])->m_nVehicleClass);
//    //            //snprintf(message, 256, "id vehicle: %d", vehicle->m_nModelIndex);
//    //            //CMessages::AddMessageJumpQ(message, 5000, false);
//
//    //        }
//    //    }
//    //}
//
//    //static void Update() {
//    //    KeyCheck::Update(); // апдейтим состояния клавиш
//    //    if (FindPlayerPed()) {
//    //        if (KeyCheck::CheckJustDown(VK_TAB)) { // Если нажата Tab - включаем или выключаем консоль
//    //            enabled = !enabled;
//    //            typedBuffer.clear();
//    //            errorMessageBuffer.clear();
//    //        }
//    //        if (enabled) {
//    //            errorMessage.clear();
//    //            if (KeyCheck::CheckWithDelay(VK_BACK, 200)) { // Если нажат Backspace - убираем последний символ в строке
//    //                if (typedBuffer.size() > 0) {
//    //                    typedBuffer.pop_back();
//    //                }
//    //            }
//    //            else {
//    //                for (int i = 0; i <= 9; i++) {
//    //                    if (KeyCheck::CheckWithDelay(i + 48, 200)) {
//    //                        if (typedBuffer.size() == 4)
//    //                            errorMessage = "Too many digits!";
//    //                        else {
//    //                            typedBuffer.push_back(i + 48); // Добавляем символ в конец строки
//    //                        }
//    //                        break;
//    //                    }
//    //                }
//    //            }
//    //            if (KeyCheck::CheckJustDown(45)) { // Если нажата Insert - спавним транспорт
//    //                if (typedBuffer.size() > 0) {
//    //                    unsigned int modelId = std::stoi(typedBuffer);
//    //                    if (modelId < 5500) {
//    //                        int modelType = CModelInfo::IsVehicleModelType(modelId);
//    //                        if (modelType != -1) {
//    //                            if (modelType == 0 || modelType == 1) {
//    //                                SpawnVehicle(modelId, FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 4.0f, 0.0f)), FindPlayerPed()->m_fRotationCur + 1.5707964f);
//    //                                errorMessageBuffer.clear(); // убираем надпись об ошибке (если она была на экране)
//    //                            }
//    //                            else
//    //                                errorMessage = "Can't spawn a train, heli and plane";
//    //                        }
//    //                        else
//    //                            errorMessage = "This model is not a vehicle!";
//    //                    }
//    //                    else
//    //                        errorMessage = "ID is too big!";
//    //                }
//    //                else
//    //                    errorMessage = "Please enter model Id!";
//    //            }
//    //        }
//    //    }
//    //    else
//    //        enabled = false;
//    //}
//
//    /* static void Render() {
//    if (enabled) {
//    CSprite2d::DrawRect(CRect(10.0f, 10.0f, 300.0f, 130.0f), CRGBA(0, 0, 0, 100));
//    CSprite2d::DrawRect(CRect(150.0f, 50.0f, 230.0f, 52.0f), CRGBA(255, 255, 255, 255));
//
//    CFont::SetScale(0.8f, 1.9f);
//    CFont::SetColor(CRGBA(255, 255, 255, 255));
//    CFont::SetJustifyOn();
//    CFont::SetFontStyle(0);
//    CFont::SetPropOn();
//    CFont::SetWrapx(300.0f);
//    CFont::PrintString(15.0f, 15.0f, "Model ID:");
//    if (typedBuffer.size() > 0)
//    CFont::PrintString(160.0f, 15.0f, const_cast<char*>(typedBuffer.c_str()));
//    if (errorMessage.size() > 0) {
//    errorMessageBuffer = errorMessage;
//    errorMessageTimer = CTimer::m_snTimeInMilliseconds;
//    }
//    if (errorMessageBuffer.size() > 0 && CTimer::m_snTimeInMilliseconds < (errorMessageTimer + 2000)) {
//    CFont::SetColor(CRGBA(255, 0, 0, 255));
//    CFont::PrintString(15.0f, 55.0f, const_cast<char*>(errorMessageBuffer.c_str()));
//    }
//    }
//    }*/
//
//    static void RenderVehicleFlags() {
//        /*CVehicle *car = FindPlayerVehicle();
//        if (car && car->m_nModelIndex == 90) {
//        car->m_nGettingInFlags |= 0x4;
//        car->m_nGettingOutFlags |= 0x4;
//        }*/
//        KeyCheck::Update();
//        if (KeyCheck::CheckWithDelay('M', 200)) {
//            static char message[256];
//            snprintf(message, 256, "random %d", plugin::Random(12345, 98765));
//            CMessages::AddMessageJumpQ(message, 2000, false);
//        }
//        CVehicle *vehicle = FindPlayerVehicle();
//        if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
//            //CVector center = vehicle->m_matrix.pos;
//            //float v119 = (CTimer::m_snTimeInMilliseconds & 0x3FF) * 0.0061328127;
//            //float v121 = sinf(v119);
//            ////if (v120 & 0x400)
//            //    //v121 = sin(deg2rad(v119));
//            //float v122 = v121;
//            //float v123 = 8.0 * v122;
//            //float v125 = cos(v119);
//            ////if (v124 & 0x400)
//            //    //v125 = cos(deg2rad(v119));
//            //float v126 = v125;
//            //float v127 = 8.0 * v126;
//            //float right_y = -v127;
//            //float right_x = v123;
//            //float up_y = v123;
//            //float up_x = v127;
//            
//            CVector Pos = CModelInfo::ms_modelInfoPtrs[vehicle->m_nModelIndex]->m_pColModel->m_boundBox.m_vecMax;
//            CVector center = vehicle->TransformFromObjectSpace(CVector(0.0f, 0.0f, 0.0f));
//            CVector up = vehicle->TransformFromObjectSpace(CVector(0.0f, Pos.y, 0.0f)) - center;
//            CVector right = vehicle->TransformFromObjectSpace(CVector(Pos.x, 0.0f, 0.0f)) - center;
//            StoreCarLightShadow(vehicle, reinterpret_cast<unsigned int>(vehicle) + 1, gpShadowHeadLightsTex, &center, up.x, up.y, right.x, right.y, 0, 0, 255, 8.0f);
//        }
//                
//
//        //    CFont::SetScale(0.5f, 1.0f);
//        //    CFont::SetColor(CRGBA(255, 255, 255, 255));
//        //    CFont::SetJustifyOn();
//        //    CFont::SetFontStyle(0);
//        //    CFont::SetPropOn();
//        //    CFont::SetWrapx(600.0f);
//        //    wchar_t text2[64];
//        //    swprintf(text2, L"index %d", playerCar->m_pHandlingData->m_dwVehicleId);
//        //    CFont::PrintString(200.0f, 10.0f, text2);
//        //    /*swprintf(text2, L"Acceleration %.5f", Acceleration);
//        //    CFont::PrintString(200.0f, 30.0f, text2);
//        //    swprintf(text2, L"CurrentSpeed %.2f", CurrentSpeed);
//        //    CFont::PrintString(200.0f, 50.0f, text2);
//        //    swprintf(text2, L"MaxSpeed %.2f", playerCar->m_pHandlingData->m_transmissionData.m_fMaxTrafficVelocity);
//        //    CFont::PrintString(200.0f, 70.0f, text2);
//        //    swprintf(text2, L"MaxGear %.7f", playerCar->m_pHandlingData->m_transmissionData.m_fMaxForwardsVelocity);
//        //    CFont::PrintString(200.0f, 90.0f, text2);
//        //    swprintf(text2, L"MinGear %.2f", playerCar->m_pHandlingData->m_transmissionData.m_fMaxBackwardsVelocity);
//        //    CFont::PrintString(200.0f, 110.0f, text2);
//        //    swprintf(text2, L"CurrentGearVelocity %.2f", playerCar->m_pHandlingData->m_transmissionData.m_fCurrentVelocity);
//        //    CFont::PrintString(200.0f, 130.0f, text2);
//        //    swprintf(text2, L"CurrentGear %d", playerCar->m_nCurrentGear);
//        //    CFont::PrintString(200.0f, 150.0f, text2);*/
//
//        //    /*swprintf(text2, L"byte_64D03C %d", byte_64D03C);
//        //    CFont::PrintString(200.0f, 170.0f, text2);
//        //    swprintf(text2, L"byte_64D044 %d", byte_64D044);
//        //    CFont::PrintString(200.0f, 190.0f, text2);
//        //    swprintf(text2, L"dword_64D058 %.2f", dword_64D058);
//        //    CFont::PrintString(200.0f, 210.0f, text2);*/
//        //    ///*swprintf(text2, L"UpVelocity0 %.2f", playerCar->m_pHandlingData->m_transmissionData.m_aGears[0].m_fChangeUpVelocity);
//        //    //CFont::PrintString(200.0f, 230.0f, text2);
//        //    //swprintf(text2, L"UpVelocity01 %.2f", playerCar->m_pHandlingData->m_transmissionData.m_aGears[1].m_fChangeUpVelocity);
//        //    //CFont::PrintString(200.0f, 250.0f, text2);
//        //    //swprintf(text2, L"UpVelocity02 %.2f", playerCar->m_pHandlingData->m_transmissionData.m_aGears[2].m_fChangeUpVelocity);
//        //    //CFont::PrintString(200.0f, 270.0f, text2);
//        //    //swprintf(text2, L"DownVelocity0 %.2f", playerCar->m_pHandlingData->m_transmissionData.m_aGears[0].m_fChangeDownVelocity);
//        //    //CFont::PrintString(200.0f, 290.0f, text2);
//        //    //swprintf(text2, L"DownVelocity01 %.2f", playerCar->m_pHandlingData->m_transmissionData.m_aGears[1].m_fChangeDownVelocity);
//        //    //CFont::PrintString(200.0f, 310.0f, text2);
//        //    //swprintf(text2, L"DownVelocity02 %.2f", playerCar->m_pHandlingData->m_transmissionData.m_aGears[2].m_fChangeDownVelocity);
//        //    //CFont::PrintString(200.0f, 330.0f, text2);*/
//        //    /*swprintf(text2, L"speed %.2f", playerCar->m_vecMoveSpeed.Magnitude());
//        //    CFont::PrintString(200.0f, 350.0f, text2);*/
//        //    
//        //}
//        
//        CPed *player = FindPlayerPed();
//        if (player /*&& KeyPressed('I')*/) {
//            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                CVehicle *car = CPools::ms_pVehiclePool->GetAt(i);
//                if (car && (DistanceBetweenPoints(player->GetPosition(), car->GetPosition()) < 5.0f) && !player->m_bInVehicle) {
//                    gamefont::Print({
//                        Format("HandlingIndex %d", car->m_pHandlingData->m_dwVehicleId),
//                        Format("HandlingId %d", reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[car->m_nModelIndex])->m_nHandlingId),
//                        Format("VehicleClass %d", reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[car->m_nModelIndex])->m_nVehicleClass),
//                        Format("min y %.2f", reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[car->m_nModelIndex])->m_pColModel->m_boundBox.m_vecMin.y)
//                    }, 300, 10, 1, FONT_DEFAULT, 0.75f, 0.75f, color::White);
//                    /*CFont::SetScale(0.5f, 1.0f);
//                    CFont::SetColor(CRGBA(255, 255, 255, 255));
//                    CFont::SetJustifyOn();
//                    CFont::SetFontStyle(0);
//                    CFont::SetPropOn();
//                    CFont::SetWrapx(600.0f);
//                    wchar_t text[64];
//                    swprintf(text, L"index %d", car->m_pHandlingData->m_dwVehicleId);
//                    CFont::PrintString(200.0f, 10.0f, text);*/
//
//                    /*swprintf(text, L"lod0 %.2f", ms_vehicleLod0Dist);
//                    CFont::PrintString(200.0f, 10.0f, text);
//                    swprintf(text, L"lod1 %.2f", ms_vehicleLod1Dist);
//                    CFont::PrintString(200.0f, 30.0f, text);
//                    swprintf(text, L"biglod0 %.2f", ms_bigVehicleLod0Dist);
//                    CFont::PrintString(200.0f, 50.0f, text);
//                    swprintf(text, L"biglod1 %.2f", ms_bigVehicleLod1Dist);
//                    CFont::PrintString(200.0f, 70.0f, text);
//                    swprintf(text, L"CompsDist %.2f", ms_cullCompsDist);
//                    CFont::PrintString(200.0f, 90.0f, text);
//                    swprintf(text, L"FadeDist %.2f", ms_vehicleFadeDist);
//                    CFont::PrintString(200.0f, 110.0f, text);*/
//
//                    /*swprintf(text, L"bIsLawEnforcer %.d", car->m_nVehicleFlags.bIsLawEnforcer);
//                    CFont::PrintString(10.0f, 10.0f, text);
//                    swprintf(text, L"bIsAmbulanceOnDuty %.d", car->m_nVehicleFlags.bIsAmbulanceOnDuty);
//                    CFont::PrintString(10.0f, 30.0f, text);
//                    swprintf(text, L"bIsFireTruckOnDuty %.d", car->m_nVehicleFlags.bIsFireTruckOnDuty);
//                    CFont::PrintString(10.0f, 50.0f, text);
//                    swprintf(text, L"bIsLocked %.d", car->m_nVehicleFlags.bIsLocked);
//                    CFont::PrintString(10.0f, 70.0f, text);
//                    swprintf(text, L"bEngineOn %.d", car->m_nVehicleFlags.bEngineOn);
//                    CFont::PrintString(10.0f, 90.0f, text);
//                    swprintf(text, L"bIsHandbrakeOn %.d", car->m_nVehicleFlags.bIsHandbrakeOn);
//                    CFont::PrintString(10.0f, 110.0f, text);
//                    swprintf(text, L"bLightsOn %.d", car->m_nVehicleFlags.bLightsOn);
//                    CFont::PrintString(10.0f, 130.0f, text);
//                    swprintf(text, L"8 %.d", car->m_nVehicleFlags.b08);
//                    CFont::PrintString(10.0f, 150.0f, text);
//
//                    swprintf(text, L"bIsVan %.d", car->m_nVehicleFlags.bIsVan);
//                    CFont::PrintString(10.0f, 170.0f, text);
//                    swprintf(text, L"bIsBus %.d", car->m_nVehicleFlags.bIsBus);
//                    CFont::PrintString(10.0f, 190.0f, text);
//                    swprintf(text, L"bIsBig %.d", car->m_nVehicleFlags.bIsBig);
//                    CFont::PrintString(10.0f, 210.0f, text);
//                    swprintf(text, L"bIsLow %.d", car->m_nVehicleFlags.bIsLow);
//                    CFont::PrintString(10.0f, 230.0f, text);
//                    swprintf(text, L"13 %.d", car->m_nVehicleFlags.b13);
//                    CFont::PrintString(10.0f, 250.0f, text);
//                    swprintf(text, L"14 %.d", car->m_nVehicleFlags.b14);
//                    CFont::PrintString(10.0f, 270.0f, text);
//                    swprintf(text, L"15 %.d", car->m_nVehicleFlags.b15);
//                    CFont::PrintString(10.0f, 290.0f, text);
//                    swprintf(text, L"16 %.d", car->m_nVehicleFlags.b16);
//                    CFont::PrintString(10.0f, 310.0f, text);
//
//                    swprintf(text, L"17 %.d", car->m_nVehicleFlags.b17);
//                    CFont::PrintString(10.0f, 330.0f, text);
//                    swprintf(text, L"bIsDamaged %.d", car->m_nVehicleFlags.bIsDamaged);
//                    CFont::PrintString(10.0f, 350.0f, text);
//                    swprintf(text, L"19 %.d", car->m_nVehicleFlags.b19);
//                    CFont::PrintString(10.0f, 370.0f, text);
//                    swprintf(text, L"20 %.d", car->m_nVehicleFlags.b20);
//                    CFont::PrintString(10.0f, 390.0f, text);
//                    swprintf(text, L"21 %.d", car->m_nVehicleFlags.b21);
//                    CFont::PrintString(10.0f, 410.0f, text);
//                    swprintf(text, L"22 %.d", car->m_nVehicleFlags.b22);
//                    CFont::PrintString(10.0f, 430.0f, text);
//                    swprintf(text, L"23 %.d", car->m_nVehicleFlags.b23);
//                    CFont::PrintString(10.0f, 450.0f, text);
//                    swprintf(text, L"24 %.d", car->m_nVehicleFlags.b24);
//                    CFont::PrintString(10.0f, 470.0f, text);
//
//                    swprintf(text, L"25 %.d", car->m_nVehicleFlags.b25);
//                    CFont::PrintString(10.0f, 490.0f, text);
//                    swprintf(text, L"26 %.d", car->m_nVehicleFlags.b26);
//                    CFont::PrintString(10.0f, 510.0f, text);
//                    swprintf(text, L"27 %.d", car->m_nVehicleFlags.b27);
//                    CFont::PrintString(10.0f, 530.0f, text);
//                    swprintf(text, L"28 %.d", car->m_nVehicleFlags.b28);
//                    CFont::PrintString(10.0f, 550.0f, text);
//                    swprintf(text, L"29 %.d", car->m_nVehicleFlags.b29);
//                    CFont::PrintString(10.0f, 570.0f, text);
//                    swprintf(text, L"30 %.d", car->m_nVehicleFlags.b30);
//                    CFont::PrintString(10.0f, 590.0f, text);
//                    swprintf(text, L"31 %.d", car->m_nVehicleFlags.b31);
//                    CFont::PrintString(10.0f, 610.0f, text);
//                    swprintf(text, L"32 %.d", car->m_nVehicleFlags.b32);
//                    CFont::PrintString(10.0f, 630.0f, text);*/
//
//                    /*swprintf(text, L"In %.d", car->m_nGettingInFlags);
//                    CFont::PrintString(10.0f, 680.0f, text);
//                    swprintf(text, L"Out %.d", car->m_nGettingOutFlags);
//                    CFont::PrintString(10.0f, 700.0f, text);*/
//
//                }
//            }
//        }
//    }
//
//    MoreVehiclesSpawner() {
//        
//        /*Events::menuDrawingEvent += [] {
//            CFont::SetScale(0.5f, 1.0f);
//            CFont::SetColor(CRGBA(238, 173, 53, 255));
//            CFont::SetJustifyOn();
//            CFont::SetFontStyle(0);
//            CFont::SetPropOn();
//            CFont::SetWrapx(600.0f);
//            wchar_t text[64];
//            swprintf(text, L"Additional Components by kenking (19.01.2019)");
//            CFont::PrintString(25.0f, 55.0f, text);
//        };*/
//        
//        
//        //ms_vehicleLod0Dist = 250.0f;
//        //ms_bigVehicleLod0Dist = 250.0f;
//        //*(float *)0x600150 = 400.0f; // 70
//        //*(float *)0x60015C = 400.0f; // 60
//        //Events::gameProcessEvent += Update;
//        //Events::drawingEvent += Render;
//        Events::drawingEvent += RenderVehicleFlags;
//
//        /*Events::gameProcessEvent += [] {
//            CVehicle *car = FindPlayerVehicle();
//            if (car) {
//                for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//                    CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//                    if (vehicle && (DistanceBetweenPoints(car->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
//                        KeyCheck::Update();
//                        if (KeyCheck::CheckWithDelay('U', 200)) {
//                            CVector offset = car->TransformFromObjectSpace(CVector(0.0f, 0.0f, 3.0f));
//                            CMatrix matrix;
//                            CPhysical::PlacePhysicalRelativeToOtherPhysical(car, vehicle, offset);
//                            matrix.~CMatrix();
//                        }
//                    }
//                }
//            }
//        };*/
//
//
//        //Events::gameProcessEvent += [] {
//        //    /*CPed *player = FindPlayerPed();
//        //    if (player) {
//        //        for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
//        //            CVehicle *vehicle = CPools::ms_pVehiclePool->GetAt(i);
//        //            if (vehicle && (DistanceBetweenPoints(player->GetPosition(), vehicle->GetPosition()) < 5.0f)) {
//        //                CVector offset = { 0.0f, 10.0f, 0.0f };
//        //                CVector point = vehicle->m_matrix * offset;
//
//        //                if (point.z <= -100.0f)
//        //                    point.z = CWorld::FindGroundZForCoord(point.x, point.y);
//        //                point.z = vehicle->GetDistanceFromCentreOfMassToBaseOfModel() + point.z;
//        //                if (CCarCtrl::JoinCarWithRoadSystemGotoCoors(vehicle, point))
//        //                    vehicle->m_autoPilot.m_nCarMission = 9;
//        //                else
//        //                    vehicle->m_autoPilot.m_nCarMission = 8;
//        //                vehicle->m_nType |= 0x18;
//        //                vehicle->m_nVehicleFlags.bEngineOn = 1;
//        //                if (vehicle->m_autoPilot.m_nCruiseSpeed <= 6)
//        //                    vehicle->m_autoPilot.m_nCruiseSpeed = 6;
//        //                else
//        //                    vehicle->m_autoPilot.m_nCruiseSpeed = vehicle->m_autoPilot.m_nCruiseSpeed;
//        //                vehicle->m_autoPilot.m_nTimeToStartMission = CTimer::m_snTimeInMilliseconds;
//
//        //            }
//        //        }
//        //    }*/
//        //};
//
//    };
//} moreVehiclesSpawner;
//
////std::string MoreVehiclesSpawner::typedBuffer;
////std::string MoreVehiclesSpawner::errorMessage;
////std::string MoreVehiclesSpawner::errorMessageBuffer;
////unsigned int MoreVehiclesSpawner::errorMessageTimer = 0;
////bool MoreVehiclesSpawner::enabled = false;
//

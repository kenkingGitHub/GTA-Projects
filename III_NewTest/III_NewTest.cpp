#include "plugin.h"
#include <string>
#include "extensions\KeyCheck.h"
#include "common.h"
//#include "CModelInfo.h"
//#include "CTimer.h"
#include "CFont.h"
//#include "CSprite2d.h"
//#include "CStreaming.h"
//#include "CBoat.h"
//#include "CWorld.h"
//#include "CTheScripts.h"
//#include "eVehicleModel.h"

//#include "CMessages.h"

using namespace plugin;

class MoreVehiclesSpawner {
public:
    /*static std::string typedBuffer;
    static std::string errorMessage;
    static std::string errorMessageBuffer;
    static unsigned int errorMessageTimer;
    static bool enabled;*/

    //static void SpawnVehicle(unsigned int modelIndex, CVector position, float orientation) {
    //    unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelIndex].m_nFlags;
    //    CStreaming::RequestModel(modelIndex, GAME_REQUIRED);
    //    CStreaming::LoadAllRequestedModels(false);
    //    if (CStreaming::ms_aInfoForModel[modelIndex].m_nLoadState == LOADSTATE_LOADED) {
    //        if (!(oldFlags & GAME_REQUIRED)) {
    //            CStreaming::SetModelIsDeletable(modelIndex);
    //            CStreaming::SetModelTxdIsDeletable(modelIndex);
    //        }
    //        CVehicle *vehicle = nullptr;
    //        if (reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[modelIndex])->m_nVehicleType)
    //            vehicle = new CBoat(modelIndex, 1);
    //        else
    //            vehicle = new CAutomobile(modelIndex, 1);
    //        if (vehicle) {
    //            // Размещаем транспорт в игровом мире
    //            vehicle->SetPosition(position);
    //            vehicle->SetOrientation(0.0f, 0.0f, orientation);
    //            vehicle->m_nState = 4;
    //            if (modelIndex == MODEL_RCBANDIT)
    //                vehicle->m_nDoorLock = CARLOCK_LOCKED;
    //            else
    //                vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
    //            CWorld::Add(vehicle);
    //            CTheScripts::ClearSpaceForMissionEntity(position, vehicle);
    //            if (vehicle->m_nVehicleClass != VEHICLE_BOAT)
    //                reinterpret_cast<CAutomobile *>(vehicle)->PlaceOnRoadProperly();

    //            //static char message[256];
    //            //snprintf(message, 256, "Class: %d", reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[modelIndex])->m_nVehicleClass);
    //            //snprintf(message, 256, "id vehicle: %d", vehicle->m_nModelIndex);
    //            //CMessages::AddMessageJumpQ(message, 5000, false);

    //        }
    //    }
    //}

    //static void Update() {
    //    KeyCheck::Update(); // апдейтим состояния клавиш
    //    if (FindPlayerPed()) {
    //        if (KeyCheck::CheckJustDown(VK_TAB)) { // Если нажата Tab - включаем или выключаем консоль
    //            enabled = !enabled;
    //            typedBuffer.clear();
    //            errorMessageBuffer.clear();
    //        }
    //        if (enabled) {
    //            errorMessage.clear();
    //            if (KeyCheck::CheckWithDelay(VK_BACK, 200)) { // Если нажат Backspace - убираем последний символ в строке
    //                if (typedBuffer.size() > 0) {
    //                    typedBuffer.pop_back();
    //                }
    //            }
    //            else {
    //                for (int i = 0; i <= 9; i++) {
    //                    if (KeyCheck::CheckWithDelay(i + 48, 200)) {
    //                        if (typedBuffer.size() == 4)
    //                            errorMessage = "Too many digits!";
    //                        else {
    //                            typedBuffer.push_back(i + 48); // Добавляем символ в конец строки
    //                        }
    //                        break;
    //                    }
    //                }
    //            }
    //            if (KeyCheck::CheckJustDown(45)) { // Если нажата Insert - спавним транспорт
    //                if (typedBuffer.size() > 0) {
    //                    unsigned int modelId = std::stoi(typedBuffer);
    //                    if (modelId < 5500) {
    //                        int modelType = CModelInfo::IsVehicleModelType(modelId);
    //                        if (modelType != -1) {
    //                            if (modelType == 0 || modelType == 1) {
    //                                SpawnVehicle(modelId, FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 4.0f, 0.0f)), FindPlayerPed()->m_fRotationCur + 1.5707964f);
    //                                errorMessageBuffer.clear(); // убираем надпись об ошибке (если она была на экране)
    //                            }
    //                            else
    //                                errorMessage = "Can't spawn a train, heli and plane";
    //                        }
    //                        else
    //                            errorMessage = "This model is not a vehicle!";
    //                    }
    //                    else
    //                        errorMessage = "ID is too big!";
    //                }
    //                else
    //                    errorMessage = "Please enter model Id!";
    //            }
    //        }
    //    }
    //    else
    //        enabled = false;
    //}

   /* static void Render() {
        if (enabled) {
            CSprite2d::DrawRect(CRect(10.0f, 10.0f, 300.0f, 130.0f), CRGBA(0, 0, 0, 100));
            CSprite2d::DrawRect(CRect(150.0f, 50.0f, 230.0f, 52.0f), CRGBA(255, 255, 255, 255));

            CFont::SetScale(0.8f, 1.9f);
            CFont::SetColor(CRGBA(255, 255, 255, 255));
            CFont::SetJustifyOn();
            CFont::SetFontStyle(0);
            CFont::SetPropOn();
            CFont::SetWrapx(300.0f);
            CFont::PrintString(15.0f, 15.0f, "Model ID:");
            if (typedBuffer.size() > 0)
                CFont::PrintString(160.0f, 15.0f, const_cast<char*>(typedBuffer.c_str()));
            if (errorMessage.size() > 0) {
                errorMessageBuffer = errorMessage;
                errorMessageTimer = CTimer::m_snTimeInMilliseconds;
            }
            if (errorMessageBuffer.size() > 0 && CTimer::m_snTimeInMilliseconds < (errorMessageTimer + 2000)) {
                CFont::SetColor(CRGBA(255, 0, 0, 255));
                CFont::PrintString(15.0f, 55.0f, const_cast<char*>(errorMessageBuffer.c_str()));
            }
        }
    }*/

    static void RenderVehicleFlags() {
        //CVehicle *car = FindPlayerVehicle();
        CPed *player = FindPlayerPed();
        if (player && KeyPressed('I')) {
            for (int i = 0; i < CPools::ms_pVehiclePool->m_nSize; i++) {
                CVehicle *car = CPools::ms_pVehiclePool->GetAt(i);
                if (car && (DistanceBetweenPoints(player->GetPosition(), car->GetPosition()) < 5.0f)) {
                    CFont::SetScale(0.5f, 1.0f);
                    CFont::SetColor(CRGBA(255, 255, 255, 255));
                    CFont::SetJustifyOn();
                    CFont::SetFontStyle(0);
                    CFont::SetPropOn();
                    CFont::SetWrapx(600.0f);
                    wchar_t text[64];
                    swprintf(text, L"bIsLawEnforcer %.d", car->m_nVehicleFlags.bIsLawEnforcer);
                    CFont::PrintString(10.0f, 10.0f, text);
                    swprintf(text, L"bIsAmbulanceOnDuty %.d", car->m_nVehicleFlags.bIsAmbulanceOnDuty);
                    CFont::PrintString(10.0f, 30.0f, text);
                    swprintf(text, L"bIsFireTruckOnDuty %.d", car->m_nVehicleFlags.bIsFireTruckOnDuty);
                    CFont::PrintString(10.0f, 50.0f, text);
                    swprintf(text, L"bIsLocked %.d", car->m_nVehicleFlags.bIsLocked);
                    CFont::PrintString(10.0f, 70.0f, text);
                    swprintf(text, L"bEngineOn %.d", car->m_nVehicleFlags.bEngineOn);
                    CFont::PrintString(10.0f, 90.0f, text);
                    swprintf(text, L"6 %.d", car->m_nVehicleFlags.b06);
                    CFont::PrintString(10.0f, 110.0f, text);
                    swprintf(text, L"bLightsOn %.d", car->m_nVehicleFlags.bLightsOn);
                    CFont::PrintString(10.0f, 130.0f, text);
                    swprintf(text, L"8 %.d", car->m_nVehicleFlags.b08);
                    CFont::PrintString(10.0f, 150.0f, text);

                    swprintf(text, L"bIsVan %.d", car->m_nVehicleFlags.b09);
                    CFont::PrintString(10.0f, 170.0f, text);
                    swprintf(text, L"bIsBus %.d", car->m_nVehicleFlags.b10);
                    CFont::PrintString(10.0f, 190.0f, text);
                    swprintf(text, L"bIsBig %.d", car->m_nVehicleFlags.b11);
                    CFont::PrintString(10.0f, 210.0f, text);
                    swprintf(text, L"bIsLow %.d", car->m_nVehicleFlags.b12);
                    CFont::PrintString(10.0f, 230.0f, text);
                    swprintf(text, L"13 %.d", car->m_nVehicleFlags.b13);
                    CFont::PrintString(10.0f, 250.0f, text);
                    swprintf(text, L"14 %.d", car->m_nVehicleFlags.b14);
                    CFont::PrintString(10.0f, 270.0f, text);
                    swprintf(text, L"15 %.d", car->m_nVehicleFlags.b15);
                    CFont::PrintString(10.0f, 290.0f, text);
                    swprintf(text, L"16 %.d", car->m_nVehicleFlags.b16);
                    CFont::PrintString(10.0f, 310.0f, text);

                    swprintf(text, L"17 %.d", car->m_nVehicleFlags.b17);
                    CFont::PrintString(10.0f, 330.0f, text);
                    swprintf(text, L"bIsDamaged %.d", car->m_nVehicleFlags.bIsDamaged);
                    CFont::PrintString(10.0f, 350.0f, text);
                    swprintf(text, L"19 %.d", car->m_nVehicleFlags.b19);
                    CFont::PrintString(10.0f, 370.0f, text);
                    swprintf(text, L"20 %.d", car->m_nVehicleFlags.b20);
                    CFont::PrintString(10.0f, 390.0f, text);
                    swprintf(text, L"21 %.d", car->m_nVehicleFlags.b21);
                    CFont::PrintString(10.0f, 410.0f, text);
                    swprintf(text, L"22 %.d", car->m_nVehicleFlags.b22);
                    CFont::PrintString(10.0f, 430.0f, text);
                    swprintf(text, L"23 %.d", car->m_nVehicleFlags.b23);
                    CFont::PrintString(10.0f, 450.0f, text);
                    swprintf(text, L"24 %.d", car->m_nVehicleFlags.b24);
                    CFont::PrintString(10.0f, 470.0f, text);
                    
                    swprintf(text, L"25 %.d", car->m_nVehicleFlags.b25);
                    CFont::PrintString(10.0f, 490.0f, text);
                    swprintf(text, L"26 %.d", car->m_nVehicleFlags.b26);
                    CFont::PrintString(10.0f, 510.0f, text);
                    swprintf(text, L"27 %.d", car->m_nVehicleFlags.b27);
                    CFont::PrintString(10.0f, 530.0f, text);
                    swprintf(text, L"28 %.d", car->m_nVehicleFlags.b28);
                    CFont::PrintString(10.0f, 550.0f, text);
                    swprintf(text, L"29 %.d", car->m_nVehicleFlags.b29);
                    CFont::PrintString(10.0f, 570.0f, text);
                    swprintf(text, L"30 %.d", car->m_nVehicleFlags.b30);
                    CFont::PrintString(10.0f, 590.0f, text);
                    swprintf(text, L"31 %.d", car->m_nVehicleFlags.b31);
                    CFont::PrintString(10.0f, 610.0f, text);
                    swprintf(text, L"32 %.d", car->m_nVehicleFlags.b32);
                    CFont::PrintString(10.0f, 630.0f, text);

                    swprintf(text, L"In %.d", car->m_nGettingInFlags);
                    CFont::PrintString(10.0f, 680.0f, text);
                    swprintf(text, L"Out %.d", car->m_nGettingOutFlags);
                    CFont::PrintString(10.0f, 700.0f, text);

               }
            }
        }
    }

    MoreVehiclesSpawner() {
        //Events::gameProcessEvent += Update;
        //Events::drawingEvent += Render;
        Events::drawingEvent += RenderVehicleFlags;
    };
} moreVehiclesSpawner;

//std::string MoreVehiclesSpawner::typedBuffer;
//std::string MoreVehiclesSpawner::errorMessage;
//std::string MoreVehiclesSpawner::errorMessageBuffer;
//unsigned int MoreVehiclesSpawner::errorMessageTimer = 0;
//bool MoreVehiclesSpawner::enabled = false;


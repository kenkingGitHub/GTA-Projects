#include <string>
#include "plugin_III.h"
#include "extensions\KeyCheck.h"
#include "game_III\common.h"
#include "game_III\CModelInfo.h"
#include "game_III\CTimer.h"
#include "game_III\CFont.h"
#include "game_III\CSprite2d.h"
#include "game_III\CStreaming.h"
#include "game_III\CBoat.h"
#include "game_III\CWorld.h"
#include "game_III\CTheScripts.h"
#include "game_III\eVehicleModel.h"

using namespace plugin;

class MoreVehiclesSpawner {
public:
    static std::string typedBuffer;
    static std::string errorMessage;
    static std::string errorMessageBuffer;
    static unsigned int errorMessageTimer;
    static bool enabled;

    static CVehicle *SpawnVehicle(unsigned int modelIndex, CVector position, float orientation) {
        unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelIndex].m_nFlags;
        CStreaming::RequestModel(modelIndex, GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);
        if (CStreaming::ms_aInfoForModel[modelIndex].m_nLoadState == LOADSTATE_LOADED) {
            if (!(oldFlags & GAME_REQUIRED)) {
                CStreaming::SetModelIsDeletable(modelIndex);
                CStreaming::SetModelTxdIsDeletable(modelIndex);
            }
            CVehicle *vehicle = nullptr;
            switch (reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[modelIndex])->m_nVehicleType) {
            case VEHICLE_BOAT:
                vehicle = new CBoat(modelIndex, 1);
                break;
            default:
                vehicle = new CAutomobile(modelIndex, 1);
                break;
            }
            if (vehicle) {
                // Размещаем транспорт в игровом мире
                vehicle->SetPosition(position);
                vehicle->SetOrientation(0.0f, 0.0f, orientation);
                vehicle->m_nState = 4;
                if (modelIndex == MODEL_RCBANDIT)
                    vehicle->m_nDoorLock = CARLOCK_LOCKED;
                else
                    vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
                CWorld::Add(vehicle);
                CTheScripts::ClearSpaceForMissionEntity(position, vehicle);
                if (vehicle->m_nVehicleClass != VEHICLE_BOAT)
                    reinterpret_cast<CAutomobile *>(vehicle)->PlaceOnRoadProperly();
                return vehicle;
            }
        }
        return nullptr;
    }

    static void Update() {
        KeyCheck::Update(); // апдейтим состояния клавиш
        if (FindPlayerPed()) {
            if (KeyCheck::CheckJustDown(VK_TAB)) { // Если нажата Tab - включаем или выключаем консоль
                enabled = !enabled;
                typedBuffer.clear();
                errorMessageBuffer.clear();
            }
            if (enabled) {
                errorMessage.clear();
                if (KeyCheck::CheckWithDelay(VK_BACK, 200)) { // Если нажат Backspace - убираем последний символ в строке
                    if (typedBuffer.size() > 0) {
                        typedBuffer.pop_back();
                    }
                }
                else {
                    for (int i = 0; i <= 9; i++) {
                        if (KeyCheck::CheckWithDelay(i + 48, 200)) {
                            if (typedBuffer.size() == 4)
                                errorMessage = "Too many digits!";
                            else {
                                typedBuffer.push_back(i + 48); // Добавляем символ в конец строки
                            }
                            break;
                        }
                    }
                }
                if (KeyCheck::CheckJustDown(45)) { // Если нажата Insert - спавним транспорт
                    if (typedBuffer.size() > 0) {
                        unsigned int modelId = std::stoi(typedBuffer);
                        if (modelId < 5501) {
                            int modelType = CModelInfo::IsVehicleModelType(modelId);
                            if (modelType != -1) {
                                if (modelType == 0 || modelType == 1) {
                                    CVehicle *vehicle = SpawnVehicle(modelId, FindPlayerPed()->TransformFromObjectSpace(CVector(0.0f, 5.0f, 0.0f)), FindPlayerPed()->m_fRotationCur + 1.5707964f);
                                    errorMessageBuffer.clear(); // убираем надпись об ошибке (если она была на экране)
                                }
                                else
                                    errorMessage = "Can't spawn a train, heli and plane model";
                            }
                            else
                                errorMessage = "This model is not a vehicle!";
                        }
                        else
                            errorMessage = "ID is too big!";
                    }
                    else
                        errorMessage = "Please enter model Id!";
                }
            }
        }
        else
            enabled = false;
    }

    static void Render() {
        if (enabled) {
            CSprite2d::DrawRect(CRect(100.0f, 100.0f, 470.0f, 200.0f), CRGBA(0, 0, 0, 100));
            CSprite2d::DrawRect(CRect(250.0f, 140.0f, 320.0f, 142.0f), CRGBA(255, 255, 255, 255));
            
            CFont::SetScale(0.8f, 1.9f);
            CFont::SetColor(CRGBA(255, 255, 255, 255));
            CFont::SetJustifyOn();
            CFont::SetFontStyle(0);
            CFont::SetPropOn();
            CFont::SetWrapx(600.0f);
            CFont::PrintString(105.0f, 105.0f, "Model ID:");
            if (typedBuffer.size() > 0)
                CFont::PrintString(250.0f, 105.0f, const_cast<char*>(typedBuffer.c_str()));
            if (errorMessage.size() > 0) {
                errorMessageBuffer = errorMessage;
                errorMessageTimer = CTimer::m_snTimeInMilliseconds;
            }
            if (errorMessageBuffer.size() > 0 && CTimer::m_snTimeInMilliseconds < (errorMessageTimer + 2000)) {
                CFont::SetColor(CRGBA(255, 0, 0, 255));
                CFont::PrintString(105.0f, 150.0f, const_cast<char*>(errorMessageBuffer.c_str()));
            }
        }
    }

    MoreVehiclesSpawner() {
        Events::gameProcessEvent += Update;
        Events::drawingEvent += Render;
    };
} moreVehiclesSpawner;

std::string MoreVehiclesSpawner::typedBuffer;
std::string MoreVehiclesSpawner::errorMessage;
std::string MoreVehiclesSpawner::errorMessageBuffer;
unsigned int MoreVehiclesSpawner::errorMessageTimer = 0;
bool MoreVehiclesSpawner::enabled = false;
